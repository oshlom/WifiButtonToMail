
/* 
**  Connect the ESP8266 unit to an existing WiFi access point
**  For more information see http://42bots.com
*/
#include <Arduino.h>
#include <ESP_Mail_Client.h>
//#include <ESP8266WiFi.h>


// Replace these with your WiFi network settings
const char* ssid = "test"; //replace this with your WiFi network name
const char* password = "PW"; //replace this with your WiFi network password
int readValue;
float voltage;
float calibration = 0.41; // Check Battery voltage using multimeter & add/subtract the value
int bat_percentage;
#define SMTP_server "smtp.gmail.com"
#define SMTP_Port 465
#define sender_email "sender@gmail.com" // note - commercial gmail user only. could not set this auth support on gmail on free account
#define sender_password "password_of_sender"
#define Recipient_email "someone@has.no.mail.con"

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
SMTPSession smtp;

void setup()
{
  delay(1000);
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println(".");
  }

  Serial.println("success!");
  Serial.print("IP Address is: ");
  Serial.println(WiFi.localIP());
  
  readValue = analogRead(A0); //read pin A0 value
  Serial.print("im reading from A0 ");
  Serial.println(readValue);
  float voltage = (((readValue * 3.3) / 1024) * 2 + calibration); //multiply by two as voltage divider network is 100K & 100K Resistor
  bat_percentage = mapfloat(voltage, 2.8, 3.5, 0, 100); //2.8V as Battery Cut off Voltage & 4.2V as Maximum Voltage
  if (bat_percentage >= 100)
  {
    bat_percentage = 100;
  }
  if (bat_percentage <= 0)
  {
    bat_percentage = 1;
  }
  //float voltage = readValue * (3.7 / 1023.0); //calculates real world voltage
  Serial.print("Voltage = "); //show “voltage before value on serial monitor
  Serial.println(voltage); //show value on serial monitor
  delay(250); //250ms delay

// smtp start
  smtp.debug(1);
  ESP_Mail_Session session;
  session.server.host_name = SMTP_server ;
  session.server.port = SMTP_Port;
  session.login.email = sender_email;
  session.login.password = sender_password;
  session.login.user_domain = "";

  /* Declare the message class */
  SMTP_Message message;


  message.sender.name = "EmptyGasDetector";
  message.sender.email = sender_email;
  message.subject = "LowGas detector triggered";
  message.addRecipient("Dude!",Recipient_email);

//    //Send HTML message
  String htmlMsg = "<div style=\"color:#FF0000;\">";
  htmlMsg = htmlMsg + "<h1>LowGas detector was just triggered</h1>" ;
  htmlMsg = htmlMsg + "<p style=\"color:#050505;\">battery's measured voltage is :" ;
  htmlMsg = htmlMsg + voltage;
  htmlMsg = htmlMsg + " ,<br/>estimate battery precentage is:";
  htmlMsg = htmlMsg + bat_percentage;
  htmlMsg = htmlMsg + "</br> Bye!</p></div>";
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit; 

 /* 
  //Send simple text message
  String textMsg = "Hello Microcontrollerslab! This is a simple text sent from ESP board";
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit; */


  if (!smtp.connect(&session))
    return;


  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
 }
void loop() {
  ESP.deepSleep(0);
  
}
