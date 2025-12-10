# Wearable-Pendant-for-Emergency-Ambulance-Staff-for-Triage
The project deals with developing both voice and button controlled werable pendant for Emergency Ambulance staff, who use this device to send the patient condition to the hospital prior to patient reaching hospital so that the Management can take necessary actions to save lives.
# Wearable Pendant for Emergency Ambulance Staff for Triage 

# Project Overview
This project is a **Hybrid-Control Wearable Triage Pendant** designed to assist ambulance staff during Mass Casualty Incidents (MCI). It allows paramedics to rapidly classify patients based on injury severity using standard triage colors (Green, Yellow, Red, Blue).
The system features a **hybrid control scheme**:
1.  **Physical Button:** A push button on the wearable pendant for manual control.
2.  **Voice Commands:** A hands-free voice recognition system using a local AI model (Whisper) running on a laptop.
    
All confirmed triage data is transmitted in real-time to a centralized **Blynk IoT Dashboard**, providing hospitals with live situational awareness.

# Features
* **Wearable ESP32 Device:** Simple and portable design with visual LED feedback.
* **Dual Control:** The pendant triage states can be controlled via **Button Press** OR **Voice Command** (e.g., "Red", "Confirm").
* **Real-Time Dashboard:** Instantly updates a cloud dashboard (Blynk) with patient status and description.
* **Local AI Processing:** Uses `openai-whisper` running locally on a PC for free, offline speech-to-text (No API key required).
* **RabbitMQ Integration:** Uses a robust message broker to handle voice command traffic between the voice recorder and the Blynk bridge.

# Tech Stack
* **Hardware:** ESP32 Dev Module, Push Button, 4 LEDs (Red, Yellow, Green, Blue).
* **Firmware:** C++ (Arduino IDE).
* **Software:** Python 3.14, RabbitMQ (Message Broker).
* **Cloud:** Blynk IoT Platform.
* **AI Model:** OpenAI Whisper (Local Base Model).

# Project Demonstration Video
* **https://drive.google.com/file/d/1rDv2vFWevz3JLzruvXmByA92DMwJiq2h/view?usp=sharing**

## 📂 Project File Structure
```text
├── producer.py         # Records audio, transcribes with Whisper, sends to RabbitMQ
├── consumer.py         # Listens to RabbitMQ, forwards commands to Blynk (V2)
├── requirements.txt    # List of required Python packages
├── README.md           # Project documentation
└── Wearable_Pendant.ino #The code to program Esp32 and link the device to blynk cloud
