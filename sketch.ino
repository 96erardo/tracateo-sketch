#include <SPI.h>
#include <TMRpcm.h>
#include <SoftwareSerial.h>

const int INPUT_PIN = A0;
const int BUTTON_PIN = 7;
const int SD_CS_PIN = 4;
const char SYNC_KEYWORD[] = "SYNC";
const byte RX_PIN = 3;
const byte TX_PIN = 5;

bool is_recording = false;
byte buff[100];

File root;
File tracker;
int track;

TMRpcm audio;
SoftwareSerial BT(TX_PIN, RX_PIN);

void setup() {
  Serial.begin(9600);
  BT.begin(9600);

  pinMode(INPUT_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);

  if (SD.begin(SD_CS_PIN)) {
    Serial.println("SD Inicializada Correctamente!");

  }else{
    Serial.println("SD No Se Pudo Iniciar");
    return; 
  }

  byte chunk;

  if (SD.exists("tracker.txt")) {
    tracker = SD.open("tracker.txt");
    String read = "";
    
    while (tracker.available()) {
      read += char(tracker.read());
    }

    track = read.toInt();

  } else {
    track = 4386;
  }


  audio.CSPin = SD_CS_PIN;
}

void loop() {
  bool record = digitalRead(BUTTON_PIN);

  if (record == HIGH) {
    if (is_recording == false) {
      Serial.println("Recording...");
      
      is_recording = true;

      track = track + 1;
      
      audio.startRecording(track + ".wav", 16000, INPUT_PIN);
    }
  } else {
    if (is_recording) {
      Serial.println("Stop");
      
      audio.stopRecording(track + ".wav");
      
      is_recording = false;
      
      SD.remove("tracker.txt");
      
      tracker = SD.open("tracker.txt", FILE_WRITE);
      tracker.print(track);
      tracker.close();

      Serial.println(track);
    }
  }

  if (BT.available()) {
    String message = getMessage();

    if (message == SYNC_KEYWORD) {
      Serial.println("Let's Sync!");

      root = SD.open("/");
      File entry = root.openNextFile();
      int quantity = 0;
      
      while (entry) {
        String name = entry.name();

        if (
          entry.isDirectory() == false &&
          name.endsWith(".WAV")
        ) {
          quantity += 1;          
        }

        entry.close();

        entry = root.openNextFile();
      }

      BT.print(quantity);
      BT.print("@");

      root.close();

      root = SD.open("/");

      entry = root.openNextFile();
      
      while (entry) {
        String name = entry.name();

        if (
          entry.isDirectory() == false &&
          name.endsWith(".WAV")
        ) {

          BT.print(entry.name());
          BT.print(",");
          BT.print(entry.size());
          BT.print("@");

          while (entry.available()) {
            int bytes = entry.read(buff, 100);

            BT.write(buff, bytes);
          }
        }

        entry.close();

        entry = root.openNextFile();
      }

      root.close();

    } else {
      Serial.print(message);
    }
  }
}

String getMessage () {
  String msg = "";

  while (BT.available()) {
    char c = (char) BT.read();

    msg += c;

    delay(1);
  }

  msg.trim();

  return msg;
}
