#include <RCSwitch.h>
#include <EEPROM.h>
RCSwitch mySwitch = RCSwitch();
int addr=0; //the EEPROM adress
bool exist=false; //flag to check a remote exist or not in memory
bool button1_state=false,button2_state=false,button3_state=false,button4_state=false; //flags to button states(ON or OFF)

void setup()
{
    Serial.begin(9600);
    mySwitch.enableReceive(0); // D4 pin in nodemcu
    pinMode(4,INPUT);
    pinMode(5,INPUT);// D1 pin in nodemcu
    pinMode(10,OUTPUT);// sd3 pin in nodemcu
    pinMode(16,OUTPUT);
    pinMode(13,OUTPUT);
    pinMode(12,OUTPUT);
    pinMode(14,OUTPUT);
    digitalWrite(16,LOW);
    digitalWrite(13,LOW);
    digitalWrite(12,LOW);
    digitalWrite(14,LOW);
    
    EEPROM.begin(512);
    
    
    


}

void loop() 
{
  digitalWrite(10,HIGH);
  delay(500);
  digitalWrite(10,LOW);
  delay(500);
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
  {   Serial.println("ER");
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
   digitalWrite(14,HIGH);
 else
   digitalWrite(14,LOW);
 if(button2_state)
   digitalWrite(12,HIGH);
 else
   digitalWrite(12,LOW);

 if(button3_state)
   digitalWrite(13,HIGH);
 else
   digitalWrite(13,LOW);

 if(button4_state)
   digitalWrite(16,HIGH);
 else
   digitalWrite(16,LOW);
}
