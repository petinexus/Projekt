
#include <DHT.h>
#include <Wire.h>  
#include "SSD1306Wire.h" 

SSD1306Wire display(0x3c, 12, 14); 
DHT dht(2, DHT22);
String al = "OFF";
int beho = 20;
int ba = 0;
int bb = 0;
int ti = 0;


void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  pinMode(13, INPUT);
  pinMode(15, INPUT);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  dht.begin();
  delay(10);
}


void loop() {
    delay(1000);
    bb = digitalRead(13);
    if (bb == HIGH) {
    beho = beho + 1;
    }
    ba = digitalRead(15);
    if (ba == HIGH) {
    beho = beho - 1;
    }
    Serial.print("P:");
    Serial.println(String(dht.readHumidity(), 1));
    Serial.print("H:");
    Serial.println(String(dht.readTemperature(), 1));
    Serial.print("BeH:");
    Serial.println(String(beho));
    ti = dht.readTemperature();
    String behot = String(beho);
    String h = String(dht.readHumidity(), 1);
    String t = String(dht.readTemperature(), 1);
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "P:");
    display.drawString(25, 0, h);
    display.drawString(0, 15, "H:");
    display.drawString(25, 15, t);
    display.drawString(0, 30, "Bh:");
    display.drawString(25, 30, behot);
    display.setFont(ArialMT_Plain_24);
    
     if(ti <= beho)
    {
      al = "ON";
      Serial.println(al);
      display.drawString(70, 20, "ON");
        digitalWrite(4, LOW);
    }else if((ti+2) >= beho)
    {
      al = "OFF";
      Serial.println(al);
      display.drawString(70, 20, "OFF");
        digitalWrite(4, HIGH);
    }
    display.display();
    delay(10);
}

