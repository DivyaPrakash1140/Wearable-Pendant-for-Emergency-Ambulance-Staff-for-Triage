import pika
import json
import requests
import time
RABBITMQ_HOST = 'localhost'
QUEUE_NAME = 'Triage'
BLYNK_AUTH_TOKEN = "3pHWaC19i4kR6KLHLwQ4mBctCxz-RJDo"
BLYNK_URL = f"https://blynk.cloud/external/api/update?token={BLYNK_AUTH_TOKEN}&V2="
def send_to_blynk(command):
    try:
        full_url = BLYNK_URL + command
        print(f"☁️ Sending to Blynk: {command} ...") 
        response = requests.get(full_url)
        if response.status_code == 200:
            print("✅ Blynk updated successfully!")
        else:
            print(f"⚠️ Blynk Error {response.status_code}: {response.text}")      
    except Exception as e:
        print(f"❌ HTTP Request Failed: {e}")
def callback(ch, method, properties, body):
    try:
        message = json.loads(body.decode())
        command = message.get("command")  
        print(f"\n📥 Received from RabbitMQ: {command}")
        if command:
            send_to_blynk(command)
        else:
            print("⚠️ Message contained no 'command' field.")     
    except Exception as e:
        print(f"❌ Error processing message: {e}")
def main():
    print("🔌 Connecting to RabbitMQ...")
    try:
        connection = pika.BlockingConnection(pika.ConnectionParameters(RABBITMQ_HOST))
        channel = connection.channel()
        channel.queue_declare(queue=QUEUE_NAME)
        
        channel.basic_consume(queue=QUEUE_NAME, 
                              on_message_callback=callback, 
                              auto_ack=True)
        print(f"👂 Listening on queue '{QUEUE_NAME}'...")
        channel.start_consuming()
    except KeyboardInterrupt:
        print("\n👋 Exiting...")
    except Exception as e:
        print(f"❌ Connection Error: {e}")
if __name__ == "__main__":
    main()