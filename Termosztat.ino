#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <Wire.h>  
#include "SSD1306Wire.h" 

WiFiClient espClient;
SSD1306Wire display(0x3c, 12, 14); 
DHT dht(2, DHT22);

const char* ssid     = "Redmi";
const char* password = "12345678";
const char* smtpServer = "mail.smtp2go.com";

ESP8266WebServer server(80);
String webString = "";  
String webMessage = "";
String s = "";
String t = "";
String h = "";
String al = "ON";

int C = 1;
String cs = "1";
int beho = 20;
int ba = 0;
int bb = 0;
int ti = 0;

void setHTML()
{
  webString = "<html><head>\n";
  webString += "<meta http-equiv=\"refresh\" content=\"60;url=http://" + WiFi.localIP().toString() + "\"> \n";
 
  ///////////// google charts script
  webString += "<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script> \n";
  webString += "   <script type=\"text/javascript\"> \n";
  webString += "    google.charts.load('current', {'packages':['gauge']}); \n";
  webString += "    google.charts.setOnLoadCallback(drawTempChart); \n";
  
  // draw temp guage
  webString += "   function drawTempChart() { \n";
  webString += "      var data = google.visualization.arrayToDataTable([ \n";
  webString += "        ['Label', 'Value'], ";
  webString += "        ['\xB0C',  ";
  webString += t;
  webString += " ], ";
  webString += "       ]); \n";
  
  // setup the google chart options
  webString += "    var options = {";
  webString += "      width: 250, height: 150,";
  webString += "      min: -10, max: 50,";
  webString += "      greenFrom: -10, greenTo: " + String(beho-C) + ",";
  webString += "      yellowFrom: " + String(beho-C) + ", yellowTo: " + String(beho+C) + ",";
  webString += "      redFrom: " + String(beho+C) + ", redTo: 120,";
  webString += "       minorTicks: 5";
  webString += "    }; \n";
  webString += "   var chart = new google.visualization.Gauge(document.getElementById('chart_divTemp')); \n";
  webString += "  chart.draw(data, options); \n";
  webString += "  } \n";
  
  webString += "    </script> \n";
  webString += "</head><body>\n";
  webString += "<form action=\"http://" + WiFi.localIP().toString() + "/submit\" method=\"POST\">";
  webString += "<h1>Okos termoszt&aacutet</h1>\n";
  webString += "<div style=\"color:red\">" + webMessage + "</div>\n";
  webString += "<table style=\"width:500px;\"><tr><td>";
  webString += "<div>Kaz&aacuten bekapcsol&aacutesa: &le; " + String(beho-C) + "&deg; C</div>\n";
  webString += "<div>Kaz&aacuten kikapcsol&aacutesa: &ge; " + String(beho+C) + "&deg; C</div>\n";
  webString += "</td><td style=\"text-align:right\">";
  webString += "H&ocircm&eacuters&eacuteklet be.:  <input type='text' value='" + String(beho) + "' name='hofok' maxlength='3' size='2'><br>\n";
  webString += "Hiszter&eacutezis :  <input type='text' value='" + String(C) + "' name='hiszti' maxlength='3' size='2'><br>\n";
  webString += "</td><td style=\"text-align:right\">";
  webString += "</td><td>";
  webString += "<input type='submit' value='Nyomd meg!' >";
  webString += "</td></tr></table>\n";
  webString += "<table style=\"width:800px;\"><tr><td>";
  webString += "<div style=\"width:300px\"><b>Utols&oacute beolvas&aacutes</b></div>\n";
  webString += "<div>H&ocircm&eacuters&eacuteklet: " + t + "&deg; C</div>\n";
  webString += "<div>P&aacuteratartalom: " + h + "%</div> \n";

  if (al == "OFF")
    webString += "<div>F&ucirct&eacutes kikapcsolva</div>";
  else
    webString += "<div>F&ucirct&eacutes bekapcsolva</div>";

  webString += "<div><a href=\"http://" + WiFi.localIP().toString() + "\">Frissit</a></div>\n";
  webString += "</td><td>";
  webString += "<div id=\"chart_divTemp\" style=\"width: 250px;\"></div>\n";
  webString += "</td><td>";
  webString += "<div id=\"chart_divHumid\" style=\"width: 250px;\"></div>\n";
  webString += "</td></tr></table>\n";
  webString += "</body></html>\n";
}

void handle_root() {
  webMessage = "";
  setHTML();
  server.send(200, "text/html", webString);            

  delay(100);
}

void handle_submit() {

  webMessage = "";

  if (server.args() > 0 ) {
    for ( uint8_t i = 0; i < server.args(); i++ ) {

      if (server.argName(i) == "hofok") {

        if (server.arg(i) != "")
        {
          if (server.arg(i).toInt() >= -10 && server.arg(i).toInt() <= 50 )
            beho = server.arg(i).toInt();
          else
            webMessage += "-10 <= H&ocircm&eacuters&eacuteklet be <= 50<br>";
        }
        else
          webMessage += "Sz&aacutemnak kell lennie!<br>";
      }

      if (server.argName(i) == "hiszti") {

        if (server.arg(i) != "")
        {
          if (server.arg(i).toInt() >= 0 && server.arg(i).toInt() <= 10 )
            C = server.arg(i).toInt();
          else
            webMessage += "0 <= Hiszter&eacutezis <= 10<br>";

        }
        else
          webMessage += "Sz&aacutemnak kell lennie!<br>";
      }
 
    }
  }

  if (webMessage == "") 
    webMessage = "Be&aacutell&iacutet&aacutesok friss&uumlltek!";
  
  setHTML();
  server.send(200, "text/html", webString);            
  delay(100);
}


byte sendEmail(){
  espClient.connect(smtpServer,2525);
  if(!emailTeszt())
    return 0;
   
  espClient.println("HELO Termosztat");
  if(!emailTeszt())
    return 0;
    
  espClient.println("AUTH LOGIN");
  if(!emailTeszt())
    return 0;
    
  espClient.println("TWlrcm9Qcm9qZWt0"); //base64-ben encode-olt username az SMTP szerverhez
  if(!emailTeszt())
    return 0;
    
  espClient.println("dGVybW9zenRhdA0K"); //ez meg a jelszó
  if(!emailTeszt())
    return 0;
    
  espClient.println(F("MAIL From: petinexus@gmail.com")); //emailt kuld
  if(!emailTeszt())
    return 0;
    
  espClient.println(F("RCPT To: roland.borbely12@gmail.com")); //ide
  if(!emailTeszt())
    return 0;
 
  espClient.println("DATA");
  if(!emailTeszt())
    return 0;
    
  espClient.println("To: roland.borbely12@gmail.com");
  espClient.println("From: petinexus@gmail.com");
  espClient.println("Subject: Teszt email\r\n");  
  espClient.println("ESP8266-rol kuldott teszt e-mail\n");
  espClient.println("kovetkezo sor, teszt erdekeben");

  espClient.println("."); //az email veget jelzi es elkuldi
  if(!emailTeszt())
    return 0;
    
  espClient.println("QUIT"); //lecsatlakozas
  if(!emailTeszt())
    return 0;
    
  espClient.stop;
  return 1;
}

byte emailTeszt(){
  byte responseCode;
  byte readByte;
  int loopCount = 0; 

  while (!espClient.available()) //ha nem elerheto 1 ms-sel no az ertek, 20000 (2 mp) utan eldobja
  {
    delay(1);
    loopCount++;
    if (loopCount > 20000)
    {
      espClient.stop();
      return 0;
    }
  }

  responseCode = espClient.peek();
  
  while (espClient.available())
    readByte = espClient.read();
 
  if (responseCode >= '4')
      return 0;


  return 1;

}


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

  WiFi.disconnect();
  IPAddress ip(192,168,43,119);   
  IPAddress gateway(192,168,43,1);   
  IPAddress subnet(255,255,255,0);  
  WiFi.config(ip, subnet, gateway);
  WiFi.begin(ssid, password);
 
  WiFi.mode(WIFI_STA);
   
  while (WiFi.status() != WL_CONNECTED) 
    delay(500);
  

  server.on("/", handle_root);
  server.on("/submit", handle_submit);

  
  server.begin();
  
  byte email=sendEmail();
  
}


void loop() {
    delay(1000);

    bb = digitalRead(13);
    if (bb == HIGH) 
      beho = beho + 1;
    
    ba = digitalRead(15);
    if (ba == HIGH)
      beho = beho - 1;
    
    String behot = String(beho);
    cs = String(C);
    h = String(dht.readHumidity(), 1);
    t = String(dht.readTemperature(), 1);
    ti = dht.readTemperature();
    
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "P:");
    display.drawString(25, 0, h);
    display.drawString(0, 15, "H:");
    display.drawString(25, 15, t);
    display.drawString(0, 30, "Bh:");
    display.drawString(25, 30, behot);
    display.drawString(0, 45, "Hi:");
    display.drawString(25, 45, cs);
    display.setFont(ArialMT_Plain_24);
    
    if(ti <= beho)
    {
      al = "ON";
      display.drawString(70, 20, "ON");
        digitalWrite(4, LOW);
    }else if((ti+C+1) >= beho)
    {
      al = "OFF";
      display.drawString(70, 20, "OFF");
        digitalWrite(4, HIGH);
    }
    display.display();
    delay(10);
    server.handleClient();
}
