#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <RCSwitch.h>
#include <EEPROM.h>

#define led1 14
#define led2 12
#define led3 13
#define led4 16

const char *ssid = "AFRA";
const char *passphrase = "admin1234";

ESP8266WebServer server(80);
RCSwitch mySwitch = RCSwitch();
int addr=0; //the EEPROM adress
bool exist=false; //flag to check a remote exist or not in memory
bool button1_state=false,button2_state=false,button3_state=false,button4_state=false; //flags to button states(ON or OFF)

//Check if header is present and correct
bool is_authentified() {
  Serial.println("Enter is authentified");
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentification Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");
  return false;
}

//login page, also called for disconnect
void handleLogin() {
  String msg;
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")) {
    Serial.println("Disconnection");
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    server.send(301);
    return;
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")) {
    if (server.arg("USERNAME") == "admin" &&  server.arg("PASSWORD") == "admin" ) // enter ur username and password you want
    {
      server.sendHeader("Location", "/");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
      server.send(301);
      Serial.println("Log in Successful");
      return;

    }

    msg = "Wrong username/password! try again.";
    Serial.println("Log in Failed");
  }
   String content = R"***(
   <!DOCTYPE html> <html>
  <head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">
  <title>AFRA remote control</title></head>
  <body style='background-color:#96aedd'><form action='/login' method='POST'><p  align ='center' style='font-size:300%;'><b>  Welcome to future  </b></p><br>
  <p  align ='center' style='font-size:300%;'><b>  Let's Login to continue...  </b></p><br>
  <p   align ='center' style='font-size:160%'><b> UserName: <input type='text' name='USERNAME' placeholder='user name' required></b></p><br>
  <p  align ='center' style='font-size:160%'><b>Password: <input type='password' name='PASSWORD' placeholder='password' required></b></p><br>
  <p  align ='center' style='font-size:160%'><input style='border: 1px solid red;background-color:transparent;padding:10px;color:#ff0000;border-radius:10px;font-size: 1.2em;' type='submit' name='submit' value='Log in'></form></p><br> </body></html>
  )***";
  server.send(200, "text/html", content);
}

//root page can be accessed only if authentification is ok
void handleRoot() {
  Serial.println("Enter handleRoot");
  String header;
  if (!is_authentified()) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
 String content = R"***(
<!DOCTYPE html> <html>
<head><meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>AFRA Relay Control</title>
<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}
body{margin-top: 50px;background-color:#96aedd;}
.button {display: inline-block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 0px;cursor: pointer;border-radius: 10px;}
.button-on {background-color: #1abc9c;}
.button-on:active {background-color: #16a085;}
.button-off {background-color: #34495e;}
.button-off:active {background-color: #2c3e50;}
p {font-size: 1.2em;color: #000;margin-bottom: 1%;}
.log_out {border: 1px solid red;padding:10px;color:#ff0000;border-radius:10px;font-size: 1.2em;text-decoration: none;}
.log_out:hover {background-color:#ff0000;color:#fff;}
</style>
</head>
<body>
<b><u><i><strong>AFRA Relay Control</strong></i></u></b></h1><br><p align ='center'>Switch 1 <a class="button button-on" href="/switch1On">ON</a>&nbsp;<a class="button button-off" href="/switch1Off">OFF</a></p>
<br><p  align ='center'>Switch 2 <a class="button button-on" href="/switch2On">ON</a>&nbsp;<a class="button button-off" href="/switch2Off">OFF</a></p>
<br><p  align ='center'>Switch 3 <a class="button button-on" href="/switch3On">ON</a>&nbsp;<a class="button button-off" href="/switch3Off">OFF</a></p>
<br><p  align ='center'>Switch 4 <a class="button button-on" href="/switch4On">ON</a>&nbsp;<a class="button button-off" href="/switch4Off">OFF</a></p>

</body>
</html>
<br><p><marquee direction='right'>Developed by  Afra Industrial Group </marquee></p>
<br><br><br><br> <footer><p>Posted by: sina majidi</p><p><a href=\"mailto:someone@example.com\">afraIOT.com</a></p></footer></body>
<br><br>You can access this page until you <a class="log_out" href="/login?DISCONNECT=YES">Log Out</a></body></html>
)***";
  server.send(200, "text/html", content);
}

//no need authentification
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


void setup()
{
  pinMode(led1, OUTPUT); 
  pinMode(led2, OUTPUT); 
  pinMode(led3, OUTPUT); 
  pinMode(led4, OUTPUT); 
  pinMode(10, OUTPUT);
  pinMode(4,INPUT);
  pinMode(5,INPUT);// D1 pin in nodemcu
  digitalWrite(led1,LOW);
  digitalWrite(led2,LOW);
  digitalWrite(led3,LOW);
  digitalWrite(led4,LOW);
   EEPROM.begin(512);
  mySwitch.enableReceive(2); // D4 pin in nodemcu
  Serial.begin(115200);
  Serial.println();
  Serial.print("Access point configuration...");
 Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(ssid,passphrase) ? "Ready" : "Failed!");
  //WiFi.softAP(ssid);
  //WiFi.softAP(ssid, passphrase, channel, ssdi_hidden, max_connection)
  
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/login", handleLogin);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works without need of authentification");
  });

  server.onNotFound(handleNotFound);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent", "Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize );
  server.on("/switch1On", switch1On);
  server.on("/switch1Off", switch1Off);
  server.on("/switch2On", switch2On);
  server.on("/switch2Off", switch2Off);
  server.on("/switch3On", switch3On);
  server.on("/switch3Off", switch3Off);
  server.on("/switch4On", switch4On);
  server.on("/switch4Off", switch4Off);
  server.begin();
//  
  Serial.println("HTTP server starting.");
}

void loop()
{
  server.handleClient();
  digitalWrite(10,HIGH);
  delay(200);
  digitalWrite(10,LOW);
  delay(200);
  if(digitalRead(4)==HIGH)
  {
     //the way to clear EEPROM
    for(int i=0;i<EEPROM.length();i++)
    {
      EEPROM.write(i,255);
      EEPROM.commit();
    }
    for(int i=0;i<10;i++)
    {
      digitalWrite(10,HIGH);
      delay(50);
      digitalWrite(10,LOW);
      delay(50);
    }
  }
  if(digitalRead(5)==HIGH)
  {   
      if (mySwitch.available()) 
      {   
        for(int i=0;i<EEPROM.length();i++)
        {
            if(EEPROM.read(i)==(mySwitch.getReceivedValue()&255))
              exist=true;
        }
      if(!exist)
      {
        if((mySwitch.getReceivedValue()&15)==1)
        {
          EEPROM.write(addr,mySwitch.getReceivedValue());
          addr=addr+1;
          EEPROM.write(addr,mySwitch.getReceivedValue()+1);
          addr=addr+1;
          EEPROM.write(addr,mySwitch.getReceivedValue()+3);
          addr=addr+1;
          EEPROM.write(addr,mySwitch.getReceivedValue()+7);
        }
        if((mySwitch.getReceivedValue()&15)==2)
        {
          EEPROM.write(addr,mySwitch.getReceivedValue()-1);
          addr=addr+1;
          EEPROM.write(addr,mySwitch.getReceivedValue());
          addr=addr+1;
          EEPROM.write(addr,mySwitch.getReceivedValue()+2);
          addr=addr+1;
          EEPROM.write(addr,mySwitch.getReceivedValue()+6);
        }
        if((mySwitch.getReceivedValue()&15)==4)
        {
          EEPROM.write(addr,mySwitch.getReceivedValue()-3);
          addr=addr+1;
          EEPROM.write(addr,mySwitch.getReceivedValue()-2);
          addr=addr+1;
          EEPROM.write(addr,mySwitch.getReceivedValue());
          addr=addr+1;
          EEPROM.write(addr,mySwitch.getReceivedValue()+4);
        }
        if((mySwitch.getReceivedValue()&15)==8)
        {
          EEPROM.write(addr,mySwitch.getReceivedValue()-7);
          addr=addr+1;
          EEPROM.write(addr,mySwitch.getReceivedValue()-6);
          addr=addr+1;
          EEPROM.write(addr,mySwitch.getReceivedValue()-4);
          addr=addr+1;
          EEPROM.write(addr,mySwitch.getReceivedValue());
        }
          if (EEPROM.commit())
          {
              Serial.println("EEPROM successfully committed");
          } 
          else 
          {
              Serial.println("ERROR! EEPROM commit failed");
          }
          addr=addr+1;
          Serial.println(addr);
          Serial.println(mySwitch.getReceivedValue());
      }  
       
      
         
          
      while(digitalRead(5)==HIGH)
      {
            digitalWrite(10,HIGH);
            delay(100);
            digitalWrite(10,LOW);
            delay(100);
      }
      mySwitch.resetAvailable();
      exist=false;
      }
  }
  if (mySwitch.available())
  {   
    Serial.println((mySwitch.getReceivedValue()&255));
    for(int i=0;i<EEPROM.length();i++)
    {
          if(EEPROM.read(i)!=255)
            Serial.println(EEPROM.read(i));
          if(EEPROM.read(i)==(mySwitch.getReceivedValue()&255))
            {
              if((mySwitch.getReceivedValue()&15)==1)
              {
                  Serial.println("OK!it's button1");
                  if(button1_state)
                     button1_state=false;
                  else
                     button1_state=true;
              }
              if((mySwitch.getReceivedValue()&15)==2)
              {
                  Serial.println("OK!it's button2");
                  if(button2_state)
                     button2_state=false;
                  else
                     button2_state=true;
              }
              if((mySwitch.getReceivedValue()&15)==4)
              {
                  Serial.println("OK!it's button3");
                  if(button3_state)
                     button3_state=false;
                  else
                     button3_state=true;
              }
              if((mySwitch.getReceivedValue()&15)==8)
              {
                  Serial.println("OK!it's button4");
                  if(button4_state)
                     button4_state=false;
                  else
                     button4_state=true;
              }
            }
    }
    mySwitch.resetAvailable();
  }
 if(button1_state)
   digitalWrite(led1,HIGH);
 else
   digitalWrite(led1,LOW);
 if(button2_state)
   digitalWrite(led2,HIGH);
 else
   digitalWrite(led2,LOW);

 if(button3_state)
   digitalWrite(led3,HIGH);
 else
   digitalWrite(led3,LOW);

 if(button4_state)
   digitalWrite(led4,HIGH);
 else
   digitalWrite(led4,LOW);
}



void switch1On()
{
  button1_state=true;
  handleRoot();
}
void switch1Off()
{

  button1_state=false;
  handleRoot();
}
void switch2On()
{
  button2_state=true;
  handleRoot();
}
void switch2Off()
{
  button2_state=false;
  handleRoot();
}
void switch3On()
{
  button3_state=true; 
  handleRoot();
}
void switch3Off()
{
  button3_state=false;
  handleRoot();
}
void switch4On()
{
  button4_state=true;
  handleRoot();
}
void switch4Off()
{
  button4_state=false;
  handleRoot();
}
