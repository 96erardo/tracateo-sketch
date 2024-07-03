#include <SPI.h>
#include <TMRpcm.h>

TMRpcm audio;

const int INPUT_PIN = A0;
const int BUTTON_PIN = 7;
const int SD_CS_PIN = 4;

bool is_recording = false;

void setup() {
  Serial.begin(9600);

  pinMode(INPUT_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);

  if (SD.begin(SD_CS_PIN)) {
    Serial.println("SD Inicializada Correctamente!");

  }else{
    Serial.println("SD No Se Pudo Iniciar");
    return; 
  }

  audio.CSPin = SD_CS_PIN;
}

void loop() {
  bool record = digitalRead(BUTTON_PIN);

  if (record == HIGH) {
    if (is_recording == false) {
      Serial.println("Recording...");
      audio.startRecording("test.wav", 16000, INPUT_PIN);
      is_recording = true;
    }
  } else {
    if (is_recording) {
      Serial.println("Stop");
      audio.stopRecording("test.wav");
      is_recording = false;
    }
  }
}
