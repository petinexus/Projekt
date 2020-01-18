#include "Gsender.h"
Gsender* Gsender::_instance = 0;
Gsender::Gsender(){}
Gsender* Gsender::Instance()
{
    if (_instance == 0) 
        _instance = new Gsender;
    return _instance;
}

Gsender* Gsender::Subject(const char* subject)
{
  delete [] _subject;
  _subject = new char[strlen(subject)+1];
  strcpy(_subject, subject);
  return _instance;
}
Gsender* Gsender::Subject(const String &subject)
{
  return Subject(subject.c_str());
}

bool Gsender::AwaitSMTPResponse(WiFiClientSecure &client, const String &resp, uint16_t timeOut)
{
  uint32_t ts = millis();
  while (!client.available())
  {
    if(millis() > (ts + timeOut)) 
      return false;
  }
  _serverResponce = client.readStringUntil('\n');
  if (resp && _serverResponce.indexOf(resp) == -1) return false;
  return true;
}

String Gsender::getLastResponce()
{
  return _serverResponce;
}

bool Gsender::Send(const String &to, const String &message)
{
  WiFiClientSecure client;
  if(!client.connect(SMTP_SERVER, SMTP_PORT)) 
    return false;
  
  if(!AwaitSMTPResponse(client, "220")) 
    return false;
  

  client.println("HELO friend");
  if(!AwaitSMTPResponse(client, "250"))
    return false;
  

  client.println("AUTH LOGIN");
  AwaitSMTPResponse(client);

  client.println(EMAILBASE64_LOGIN);
  AwaitSMTPResponse(client);

  client.println(EMAILBASE64_PASSWORD);
  if (!AwaitSMTPResponse(client, "235")) 
    return false;
  
  
  String mailFrom = "MAIL FROM: <" + String(FROM) + '>';
  client.println(mailFrom);
  AwaitSMTPResponse(client);

  String rcpt = "RCPT TO: <" + to + '>';
  client.println(rcpt);
  AwaitSMTPResponse(client);

  client.println("DATA");
  if(!AwaitSMTPResponse(client, "354")) 
    return false;
  
  
  client.println("From: <" + String(FROM) + '>');
  client.println("To: <" + to + '>');
  
  client.print("Subject: ");
  client.println(_subject);
  
  client.println("Mime-Version: 1.0");
  client.println("Content-Type: text/html; charset=\"UTF-8\"");
  client.println("Content-Transfer-Encoding: 7bit");
  client.println();
  String body = "<!DOCTYPE html><html lang=\"en\">" + message + "</html>";
  client.println(body);
  client.println(".");
  if (!AwaitSMTPResponse(client, "250")) 
    return false;
  
  client.println("QUIT");
  if (!AwaitSMTPResponse(client, "221")) 
    return false;
  
  return true;
}
