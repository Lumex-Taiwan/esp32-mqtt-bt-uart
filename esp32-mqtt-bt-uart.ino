// Rev.1  
// Wireless Internet Connection with MQTT Client.    
// Live Configurable through Bluetooth Serial - Wifi(SSID, KEY), MQTT(Server, Topic)
// Bluetooth Serial is able to be turn off through MQTT.
 
#include "BluetoothSerial.h"
#include <WiFi.h>
#include <EEPROM.h>
#include <PubSubClient.h>

#define EEPROM_SIZE 512

BluetoothSerial SerialBT;
WiFiClient espClient;
PubSubClient client(espClient);

char* ssid = "wifi_ssid";                     //Can be changed by inputing "ssid=(your own SSID)" command when connect with Bluetooth Serial mode 
char* password = "wifi_password";             //Can be changed by inputing "password=(your own password)" command when connect with Bluetooth Serial mode
char* Bluetooth_ssid = "ID of This Device";   //Can be changed by inputing "bluetooth name=(your own Bluetooth ID)" command when connect with Bluetooth Serial mode
char* mqtt_server = "broker.hivemq.com";      //Can be changed by inputing "broker=(your own broker name)" command when connect with Bluetooth Serial mode
char* Topic_subscribe1 = "                ";
char* Topic_subscribe2 = "                ";
char* Topic_broadcast1 = "   ";
char* Topic_broadcast2 ="     ";
char* Topic_publish1 = "                  ";
char* Topic_publish2 = "                  ";
char* Topic_publish3 = "                  ";
char* Topic_publish4 = "                  ";
char* Topic_publish5 = "                  ";
int MQTT_flag = 0;
char MQTT_command_flag = 0;
char *mqtt_message = "";
long MQTT_loop_count = 0;
char MQTT_connect_count = 0;
int color = 35;
int Tmp;
int i=0, j, statemachine = 4;
char ssid_password_flag;
char Array_UART1[30];
char Array_UART2[30];
char Array_UART3[10];
char Array_UART4[10];
char Array_UART5[30];
char Array_UART6[30];
char Array_UART7[50]={'e','z','D','i','s','p','l','a','y','/'};              // MQTT Topic_subscribe1 
char Array_UART8[50]={'T','e','m','p','e','r','a','t','u','r','e','/'};      //Reserved for future use 
char Array_UART9[50]={'H','u','m','i','d','i','t','y','/'};                  //Reserved for future use 
char Array_UART10[50]={'E','c','h','o','/'};                                 // MQTT Topic_publish3
char Array_UART11[50]={'B','i','t','m','a','p','/'};                         // MQTT Topic_subscribe2
char Array_UART12[50]={'P','r','e','s','s','u','r','e','/'};                 //Reserved for future use 
char Array_UART13[50]={'A','l','t','i','t','u','d','e','/'};                 //Reserved for future use 
char Array_UART14[50];
char Array_UART15[50];
char Temp_eeprom[512];
String message = "";
char incomingChar;

void setup()
{
  delay(2000);
  EEPROM.begin(EEPROM_SIZE);
  Tmp = EEPROM.read(100);    //Start address that store Bluetooth Name
  if (Tmp != 0xff)
  {
    for (i = 0; i < 30; i++) {
      Tmp = EEPROM.read(i + 100);
      if (Tmp == ')') {
        i = 30;
      }
      else{
        Array_UART5[i] = Tmp;
        Array_UART7[i+10] = Tmp;
        Array_UART8[i+12] = Tmp;  
        Array_UART9[i+9] = Tmp;   
        Array_UART10[i+5] = Tmp;
        Array_UART11[i+7] = Tmp; 
        Array_UART12[i+9] = Tmp; 
        Array_UART13[i+9] = Tmp;                         
      }
    }
    Bluetooth_ssid = Array_UART5;
    Topic_subscribe1 = Array_UART7;
    Topic_subscribe2 = Array_UART11;
    Topic_publish1 = Array_UART8;
    Topic_publish2 = Array_UART9; 
    Topic_publish3 = Array_UART10;
    Topic_publish4 = Array_UART12;
    Topic_publish5 = Array_UART13;  
  }
  else {
    for(i=0;i<30;i++){
      if (Bluetooth_ssid[i]=='\0'){
        j = i;        
        i=30;
      }  
    }   
    for (i = 0; i < j; i++){
      Tmp = Bluetooth_ssid[i];
      Array_UART7[i+10] = Tmp;
      Array_UART8[i+12] = Tmp;  
      Array_UART9[i+9] = Tmp;   
      Array_UART10[i+5] = Tmp;
      Array_UART11[i+7] = Tmp; 
      Array_UART12[i+9] = Tmp; 
      Array_UART13[i+9] = Tmp;      
    }
    Topic_subscribe1 = Array_UART7;
    Topic_subscribe2 = Array_UART11;
    Topic_publish1 = Array_UART8;
    Topic_publish2 = Array_UART9; 
    Topic_publish3 = Array_UART10;
    Topic_publish4 = Array_UART12;
    Topic_publish5 = Array_UART13;    
  }
   
  Tmp = EEPROM.read(130);    //Start address that store broker Name
  if (Tmp != 0xff)
  {
    for (i = 0; i < 30; i++) {
      Tmp = EEPROM.read(i + 130);
      if (Tmp == ')') {
        i = 30;
      }
      else
        Array_UART6[i] = Tmp;
    }
    mqtt_server = Array_UART6;
  } 
  Tmp = EEPROM.read(175);    //Start address that store Broadcast1 topic
  if (Tmp != 0xff)
  {
    for (i = 0; i < 50; i++) {
      Tmp = EEPROM.read(i + 175);
      if (Tmp == ')') {
        i = 50;
      }
      else
        Array_UART14[i] = Tmp;
    }
    Topic_broadcast1 = Array_UART14;
  } 
  Tmp = EEPROM.read(225);    //Start address that store Broadcast2 topic
  if (Tmp != 0xff)
  {
    for (i = 0; i < 50; i++) {
      Tmp = EEPROM.read(i + 225);
      if (Tmp == ')') {
        i = 50;
      }
      else
        Array_UART15[i] = Tmp;
    }
    Topic_broadcast2 = Array_UART15;
  }   
  SerialBT.begin(Bluetooth_ssid);         //Bluetooth device name
  delay(1000);
  Serial.begin(115200);
  Serial.printf("atfd=(0)");
  while (Serial.read() != 'E') {}  
  delay(2000);   

  ssid_password_flag = EEPROM.read(1);    // begining address of SSIS & Password data in EEPROM
  if ((ssid_password_flag != 0xff)) {     //EEPROM default is 0xff, if it is 0xff ,means the SSID　and Password are not set yet
    statemachine = 88;
  }    
  else {
    Serial.printf("atd0=()");
    while (Serial.read() != 'E') {}
    Serial.print("at81=(0,0,Type ssid=)");
    while (Serial.read() != 'E') {}
    Serial.printf("at81=(1,0,(SSID)    )");
    while (Serial.read() != 'E') {}
    Serial.printf("at81=(2,0,Type password=)");
    while (Serial.read() != 'E') {}
    Serial.printf("at81=(3,0,(PASSWORD))");
    while (Serial.read() != 'E') {}
    delay(3000);
    statemachine = 88;
  }
}

void loop()
{
  switch (statemachine)
  {
    case 88:
      wifi_setup();
      if (MQTT_flag == 1){
        delay(1000);          
        Serial.printf("atd0=()");
        while (Serial.read() != 'E') {}
        delay(100);       
        Serial.printf("at81=(0,0,MQTT and  )");
        while (Serial.read() != 'E') {}
        delay(100);        
        Serial.printf("at81=(1,0,Bluetooth )");
        while (Serial.read() != 'E') {}
        delay(100);
        Serial.printf("at81=(2,0,Serial    )");
        while (Serial.read() != 'E') {}  
        delay(100);              
        Serial.printf("at81=(3,0,Mode      )");
        while (Serial.read() != 'E') {} 
        delay(100);      
      }
      else {
        delay(1000);
        Serial.printf("atd0=()");
        while (Serial.read() != 'E') {}  
        delay(100);      
        Serial.printf("at81=(0,0,Only      )");
        while (Serial.read() != 'E') {}
        delay(100);         
        Serial.printf("at81=(1,0,Bluetooth )");
        while (Serial.read() != 'E') {}
        delay(100); 
        Serial.printf("at81=(2,0,Serial    )");
        while (Serial.read() != 'E') {} 
        delay(100);                
        Serial.printf("at81=(3,0,Available )");
        while (Serial.read() != 'E') {}  
        delay(100);                    
      }
      statemachine = 4;
      break;    

    case 4:
      if (MQTT_flag == 1) {
        if (!client.connected()) {
          reconnect();
        }
        client.loop();
      }  
      if (Serial.available()) {
        char LEDChar = Serial.read();
        if (LEDChar == 'E'){
          SerialBT.write(LEDChar);
          message = "";
        }
        else 
          SerialBT.write(LEDChar);
      }  
      if (SerialBT.available()) {
        char incomingChar = SerialBT.read();
        message += String(incomingChar);
        Serial.write(incomingChar);  
      }      
      if ((message == "ssid=(")|(message == "Ssid=(")) {
        statemachine = 5;
        message = "";
        i = 1;
        break;
      }
      else if ((message == "password=(")|(message == "Password=(")) {
        statemachine = 6;
        message = "";
        i = 30;
        break;
      }

      else if ((message == "bluetooth name=(") | (message == "Bluetooth name=(")) {
        statemachine = 16;
        message = "";
        i = 100;
        break;
      }
      else if ((message == "Broker=(")|(message == "broker=(")) {
        statemachine = 17;
        message = "";
        i = 130;
        break;
      }  
      else if ((message == "Broadcast1=(")|(message == "broadcast1=(")) {
        statemachine = 19;
        message = "";
        i = 175;
        break;
      } 
      else if ((message == "Broadcast2=(")|(message == "broadcast2=(")) {
        statemachine = 20;
        message = "";
        i = 225;
        break;
      }                      
      break;
    case 5:
      if (Serial.available()) {
        SerialBT.write(Serial.read());
        message = "";
      }    
      if (SerialBT.available()) {
        incomingChar = SerialBT.read();
        Temp_eeprom[i] = incomingChar;
        Serial.write(incomingChar);
        i++;
      }
      if (incomingChar == ')') {
        for (i - 1; i >= 1; i--) {
          EEPROM.write(i, Temp_eeprom[i]);
          EEPROM.commit();
        }            
        statemachine = 4;
        message = "";
      } 
      break;     
    case 6:
      if (Serial.available()) {
        SerialBT.write(Serial.read());
        message = "";
      }    
      if (SerialBT.available()) {
        incomingChar = SerialBT.read();
        Temp_eeprom[i] = incomingChar;
        Serial.write(incomingChar);
        i++;
      }
      if (incomingChar == ')') {
        for (i - 1; i >= 30; i--) {
          EEPROM.write(i, Temp_eeprom[i]);
          EEPROM.commit();
        }
        statemachine = 4;
        message = "";
      }            
      break;
    case 16:
      if (Serial.available()) {
        SerialBT.write(Serial.read());
        message = "";
      }
      if (SerialBT.available()) {
        incomingChar = SerialBT.read();
        Temp_eeprom[i] = incomingChar;
        i++;
      }
      if (incomingChar == ')') {
        for (i - 1; i >= 100; i--) {
          EEPROM.write(i, Temp_eeprom[i]);
          EEPROM.commit();
        }        
        statemachine = 4;
        message = "";
      }
      break;
    case 17:
      if (Serial.available()) {
        SerialBT.write(Serial.read());
        message = "";
      }
      if (SerialBT.available()) {
        incomingChar = SerialBT.read();
        Temp_eeprom[i] = incomingChar;
        i++;
      }
      if (incomingChar == ')') {
        for (i - 1; i >= 130; i--) {
          EEPROM.write(i, Temp_eeprom[i]);
          EEPROM.commit();
        }       
        statemachine = 4;
        message = "";
      }            
      break;  
    case 19:
      if (Serial.available()) {
        SerialBT.write(Serial.read());
        message = "";
      }
      if (SerialBT.available()) {
        incomingChar = SerialBT.read();
        Temp_eeprom[i] = incomingChar;
        i++;
      }
      if (incomingChar == ')') {
        for (i - 1; i >= 175; i--) {
          EEPROM.write(i, Temp_eeprom[i]);
          EEPROM.commit();
        }       
        statemachine = 4;
        message = "";
      }            
      break; 
    case 20:
      if (Serial.available()) {
        SerialBT.write(Serial.read());
        message = "";
      }
      if (SerialBT.available()) {
        incomingChar = SerialBT.read();
        Temp_eeprom[i] = incomingChar;
        i++;
      }
      if (incomingChar == ')') {
        for (i - 1; i >= 225; i--) {
          EEPROM.write(i, Temp_eeprom[i]);
          EEPROM.commit();
        }       
        statemachine = 4;
        message = "";
      }            
      break;                                            
    default:
      statemachine = 4;
      break;
  }
}


void wifi_setup() {
  /*connect to WiFi*/
  int connect_count = 0;
  Serial.printf("atfc=(0)");
  while (Serial.read() != 'E') {}
  Serial.printf("atd0=()");
  while (Serial.read() != 'E') {}
  delay(1000);
  /* read SSID*/           
  if (EEPROM.read(1) != 0xff){          //Start address where store SSID 
    for (i = 0; i < 30; i++) {   
      Tmp = EEPROM.read(i+1);
      if (Tmp == ')') {
        i = 30;
      }
      else
        Array_UART1[i] = Tmp;
    }
    ssid = Array_UART1;
  }
  /* read password*/           
  if (EEPROM.read(30) != 0xff){         //Start address where store Password
    for (i = 0; i < 30; i++) {
      Tmp = EEPROM.read(i + 30);
      if (Tmp == ')') {
        i = 30;
      }
      else
        Array_UART2[i] = Tmp;
    }
    password = Array_UART2;
  } 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf("atd0=()");
    while (Serial.read() != 'E') {}
    Serial.printf("at81=(2,0,WiFi)");
    while (Serial.read() != 'E') {}
    Serial.printf("at81=(3,0,Connecting)");
    while (Serial.read() != 'E') {}
    delay(250);
    Serial.printf("at80=(1,0,.)");
    while (Serial.read() != 'E') {}
    delay(250);
    Serial.printf("at80=(1,1,.)");
    while (Serial.read() != 'E') {}
    delay(250);
    Serial.printf("at80=(1,2,.)");
    while (Serial.read() != 'E') {}
    delay(250);
    Serial.printf("at80=(1,3,.)");
    while (Serial.read() != 'E') {}
    delay(250);
    Serial.printf("at81=(1,5,Rev.5)");
    while (Serial.read() != 'E') {}
    delay(250);      
    connect_count++;
    if (connect_count >= 5) {
      Serial.printf("atd0=()");
      while (Serial.read() != 'E') {}
      Serial.printf("at81=(0,0,Wrong SSID)");
      while (Serial.read() != 'E') {}
      delay(100);
      Serial.printf("at81=(1,0,or Password)");
      while (Serial.read() != 'E') {}
      delay(100);
      Serial.printf("at81=(2,0,  Or No)");
      while (Serial.read() != 'E') {}
      delay(100);
      Serial.printf("at81=(3,0,  WIFI?)");
      while (Serial.read() != 'E') {}       
      delay(1500);
      MQTT_flag = 0;
      break;
    }
    if (SerialBT.available()) {
      char incomingChar = SerialBT.read();
      message += String(incomingChar);
    }
    if (message != "") {
      statemachine = 4;
      message = "";
      MQTT_flag = 0;
      break;
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("at81=(0,0,CONNECTED)");
    while (Serial.read() != 'E') {}
    Serial.printf("atfc=(0)");
    while (Serial.read() != 'E') {}
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    client.subscribe(Topic_subscribe2);
    client.subscribe(Topic_subscribe1);
    MQTT_flag = 1;    
  }
}

void callback(char* topic, byte * MQTT_message, unsigned int length) {
  String messageTemp;
  if ((String(topic) == Topic_subscribe1) | (String(topic) == Topic_broadcast1)) {
    for (int i = 0; i < length; i++) {
      Serial.write((char)MQTT_message[i]);
      messageTemp += (char)MQTT_message[i];
    }          
    if ( (MQTT_message[0] == 'a' | MQTT_message[0] == 'A') & (MQTT_message[1] == 't' | MQTT_message[1] == 'T') & (MQTT_message[4] == '=')){
      while (!Serial.available()){} 
      message = "";
      mqtt_message = "";          
      client.publish(Topic_publish3,"E");                
    }               
    else if ((messageTemp == "Bluetooth on") | (messageTemp == "bluetooth on")) {
      while (!Serial.available()){}
      SerialBT.begin(Bluetooth_ssid); 
      client.publish(Topic_publish3,"E");
    }  
    else if ((messageTemp == "Bluetooth off") | (messageTemp == "bluetooth off")) {
      while (!Serial.available()){}
      SerialBT.end(); 
      client.publish(Topic_publish3,"E");
    }                        
  }
  
  if ((String(topic) == Topic_subscribe2) | (String(topic) == Topic_broadcast2)) {
    byte* p = (byte*)malloc(length);
    char val;
    memcpy(p,MQTT_message,length);
    for (i = 0, j=0; i < length; i += 2,j++) {
      if (p[i] >= 'a'){
        val = (p[i]-'a'+10) * 16;
        if (p[i+1] >= 'a'){
          val = val + (p[i+1]-'a'+10);
        }
        else {
          val = val + (p[i+1] - '0');
        }      
      }
      else if (p[i] >= 'A'){
        val = (p[i]-'A'+10) * 16;
        if(p[i+1] >= 'A'){
          val = val + (p[i+1]-'A'+10);
        }
        else {
          val = val + (p[i+1] - '0');
        }        
      }
      else {
        val = (p[i]-'0') * 16;
        if(p[i+1] >= 'a'){
          val = val + (p[i+1]-'a'+10);
        }
        else if(p[i+1] >= 'A') {
          val = val + (p[i+1] - 'A'+10);
        } 
        else {
          val = val + (p[i+1] - '0');       
        }
      }   
      p[j] = val;
    } 
    for (i = 0 ; i<(length/2); i++){
      Serial.write(p[i]);
    }
    free(p);
    while (!Serial.available()){} 
    message = "";
    mqtt_message = "";          
    client.publish(Topic_publish3,"E");            
  } 
}

void reconnect() {
  // Loop until we're reconnected 
  while (!client.connected()) {
    if (client.connect(Bluetooth_ssid)) {  
      client.subscribe(Topic_subscribe1);
      client.subscribe(Topic_subscribe2);  
      client.subscribe(Topic_broadcast1);
      client.subscribe(Topic_broadcast2);           
      MQTT_flag = 1;
    }
    else {      
      delay(1000);
      MQTT_connect_count++;
      if (MQTT_connect_count == 2) {    
        wifi_setup(); 
        MQTT_connect_count = 0;
        break;
      }      
    }
  }
}
