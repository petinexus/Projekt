#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <Wire.h>  
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "SSD1306Wire.h" 
#include "Gsender.h"
#include <ESP8266HTTPClient.h>

SSD1306Wire display(0x3c, 12, 14); 
DHT dht(2, DHT22);

const char* ssid     = "Redmi";
const char* password = "12345678";

ESP8266WebServer server(80);
String webString = "";  
String webMessage = "";
String s = "";
String t = "";
String h = "";
String al = "ON";
String foabiztonsag="3r05v3d3l3m";

int C = 1;
String cs = "1";
int beho = 20;
int ba = 0;
int bb = 0;
int ti = 0;

String email = "roland.borbely12@gmail.com"; 
int aktime = 10;

const long utcOffsetInSeconds = 3600;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
int akt = -1;
int aktsql = -1;


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

  webString += "<div>Email k&ucircld&eacutes minden " + String(aktime) + " percbe</div> \n";
  webString += "<div><a href=\"http://" + WiFi.localIP().toString() + "\">Frissit</a></div>\n";
    webString += "<div><a href=\"http://192.168.43.34/kiir.php\">SQL</a></div>\n";
  webString += "</td><td>";
  webString += "<div id=\"chart_divTemp\" style=\"width: 250px;\"></div>\n";
  webString += "</td><td>";
  webString += "<div id=\"chart_divHumid\" style=\"width: 250px;\"></div>\n";
  webString += "</td></tr></table>\n";
  webString += "Email k&ucircld&eacutes rendszeress&eacutege : <select name='select'>";
  webString += "  <option value=\"semmi\"></option>";
  webString += "  <option value=\"0\">most</option>";
  webString += "  <option value=\"10\">Minden 10 percben</option>";
  webString += "  <option value=\"30\">Minden 30 percben</option>";
  webString += "  <option value=\"60\">Minden 60 percben</option>";
  webString += "</select> <br>";
  webString += "Email cim :  <input type='text' name='emailcim' size='33' value=" + email + "><br>";
  webString += "<input type='submit' style='margin-left:120px' value='Nyomd meg ezt is!'  >";
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

if(server.argName(i) == "select"){
  if(server.arg(i) == "0"){
    for ( uint8_t j = 0; j < server.args(); j++ ){
      if(server.argName(j) == "emailcim"){
        email = String(server.arg(j));
        Gsender *gsender = Gsender::Instance();  
        String subject = "ESP8266 értesítés - azonnali e-mail";
        gsender->Subject(subject)->Send(email, "Az aktuális hőmérséklet: "+t+" °C, a páratartalom: "+h+" %.");
        webMessage += "Email elk&ucircldve!<br>";
      }  
    }
  }
  if(server.arg(i) == "10"){
    for ( uint8_t j = 0; j < server.args(); j++ ){
      if(server.argName(j) == "emailcim")
        email = String(server.arg(j));
      aktime = 10;  
    }
  }
  if(server.arg(i) == "30"){
    for ( uint8_t j = 0; j < server.args(); j++ ){
      if(server.argName(j) == "emailcim")
        email = String(server.arg(j));
      aktime = 30;  
    }
  }
  if(server.arg(i) == "60"){
    for ( uint8_t j = 0; j < server.args(); j++ ){
      if(server.argName(j) == "emailcim")
        email = String(server.arg(j));
      aktime = 60;  
    }
  }
}
    }
  }

  if (webMessage == "") 
    webMessage = "Be&aacutell&iacutet&aacutesok friss&uumlltek!";
  
  setHTML();
  server.send(200, "text/html", webString);            
  delay(100);
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
  IPAddress ip(192, 168, 43, 120);   
  IPAddress gateway(192, 168, 43, 1);   
  IPAddress subnet(255, 255, 255, 0);  
  IPAddress dns(8, 8, 8, 8); 
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet, dns); 
 
  WiFi.mode(WIFI_STA);
   
  while (WiFi.status() != WL_CONNECTED) 
    delay(500);
  
  server.on("/", handle_root);
  server.on("/submit", handle_submit);
  server.begin();
  
  Gsender *gsender = Gsender::Instance();  
  String subject = "Indulas";
  gsender->Subject(subject)->Send("roland.borbely12@gmail.com", "elindult");

  timeClient.begin();
}


void loop() {
    timeClient.update();
    delay(250);

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

if(aktime == 60){
if(timeClient.getMinutes()%60 == 0 && timeClient.getMinutes() != akt)
  {
    akt = timeClient.getMinutes();
    Gsender *gsender = Gsender::Instance();  
    String subject = "ESP8266 értesítés - 1 perc";
    gsender->Subject(subject)->Send(email, "Az aktuális hőmérséklet: "+t+" °C, a páratartalom: "+h+" %.");
  }
}

if(aktime == 30){
if(timeClient.getMinutes()%30 == 0 && timeClient.getMinutes() != akt)
  {
    akt = timeClient.getMinutes();
    Gsender *gsender = Gsender::Instance();  
    String subject = "ESP8266 értesítés - 30 perc";
    gsender->Subject(subject)->Send(email, "Az aktuális hőmérséklet: "+t+" °C, a páratartalom: "+h+" %.");
  }
}

if(aktime == 10){
if(timeClient.getMinutes()%10 == 0 && timeClient.getMinutes() != akt)
  {
    akt = timeClient.getMinutes();
    Gsender *gsender = Gsender::Instance();  
    String subject = "ESP8266 értesítés - 10 perc";
    gsender->Subject(subject)->Send(email, "Az aktuális hőmérséklet: "+t+" °C, a páratartalom: "+h+" %.");
  }
}


if(timeClient.getMinutes() % 1 == 0 && timeClient.getMinutes() != aktsql)
{
  aktsql=timeClient.getMinutes();
  HTTPClient http;
  http.begin("http://192.168.43.34/post.php");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String data = "foabiztonsag=" + foabiztonsag + "&temperature=" + t + " °C" + "&humidity=" + h + "%";
  http.POST(data);
  http.end();
}

    delay(10);
    server.handleClient();
}
