import sounddevice as sd
import soundfile as sf
import pika
import json
import whisper
import numpy as np
import os

RABBITMQ_HOST = 'localhost'
QUEUE_NAME = 'triage_commands'
AUDIO_FILE = "temp.wav"
SAMPLE_RATE = 16000
DURATION = 4
WHISPER_MODEL_SIZE = "small"

VALID_COMMANDS = ["red", "yellow", "green", "blue", "black", "confirm", "cycle", "next"]

def main():
    try:
        connection = pika.BlockingConnection(pika.ConnectionParameters(RABBITMQ_HOST))
        channel = connection.channel()
        channel.queue_declare(queue=Triage)
    except Exception as e:
        print(f"❌ Error connecting to RabbitMQ: {e}")
        return

    print("-" * 40)
    print(f"🎤 Recording for {DURATION} seconds... Speak NOW!")
    
    recording = sd.rec(int(DURATION * SAMPLE_RATE), 
                       samplerate=SAMPLE_RATE, 
                       channels=1, 
                       dtype='int16')
    sd.wait()
    
    sf.write(AUDIO_FILE, recording, SAMPLE_RATE, subtype='PCM_16')
    print("✅ Recording finished.")

    print("⏳ Transcribing...")
    try:
        model = whisper.load_model(WHISPER_MODEL_SIZE)
        result = model.transcribe(
            AUDIO_FILE,
            language="en", 
            fp16=False,
            condition_on_previous_text=False
        )
        raw_text = result["text"].strip().lower()
        print(f"📝 Raw Transcription: '{raw_text}'")
    except Exception as e:
        print(f"❌ Whisper Error: {e}")
        connection.close()
        return

    detected_cmd = None
    for valid_cmd in VALID_COMMANDS:
        if valid_cmd in raw_text:
            detected_cmd = valid_cmd
            break
            
    if detected_cmd:
        payload = {
            "command": detected_cmd,
            "source": "voice_terminal",
            "raw_text": raw_text
        }
        message_body = json.dumps(payload)
        
        channel.basic_publish(exchange='',
                              routing_key=QUEUE_NAME,
                              body=message_body)
        print(f"🚀 Sent to RabbitMQ: {message_body}")
    else:
        print("⚠️ No valid triage command detected.")

    connection.close()
    if os.path.exists(AUDIO_FILE):
        os.remove(AUDIO_FILE)

if __name__ == "__main__":
    main()