
#include <DHT.h>
#include <Wire.h>  

DHT dht(2, DHT22);
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
     if(ti <= beho)
    {
      Serial.println("ON");
        digitalWrite(4, LOW);
    }else if((ti+2) >= beho)
    {
      Serial.println("OFF");
        digitalWrite(4, HIGH);
    }
    delay(10);
}

