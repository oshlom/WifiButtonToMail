
/* 
**  Connect the ESP8266 unit to an existing WiFi access point
**  For more information see http://42bots.com
*/
#include <Arduino.h>
#include <ESP_Mail_Client.h>


// Replace these with your WiFi network settings
const char* ssid = "test"; //replace this with your WiFi network name
const char* password = "PW"; //replace this with your WiFi network password
int readValue;
float voltage;
float calibration = 0.41; // Check Battery voltage using multimeter & add/subtract the value
int bat_percentage;
#define SMTP_server "smtp.gmail.com"
#define SMTP_Port 465
#define sender_email "sender@gmail.com" // note - you will need an "app account" for gmail, available only after setting up two factor auth on gmail
#define sender_password "password_of_sender"
#define sender_name "esp32 button to mail"
#define Recipient_email "someone@has.no.mail.con"
#define Recipient_name "Dude!"
#define email_subject "esp32 has been triggered to send this mail"

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
SMTPSession smtp;

void setup()
{
  delay(1000);
  // Initialize the ADC
  analogReadResolution(12); // Set the ADC resolution to 12 bits (0-4095)
  analogSetAttenuation(ADC_11db); // Set the input attenuation to 11 dB (input voltage range 0-3.6V)
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
  
  //readValue = analogRead(A0); //read pin A0 value
  Serial.print("im reading from A0 ");
  // Read the battery voltage
  int raw_adc = analogRead(A0); // Read the voltage from analog pin A0
  float voltage = raw_adc * 0.000805664; // Convert the ADC reading to voltage (3.3V / 4095)
  bat_percentage = mapfloat(voltage, 2.8, 3.3, 0, 100); //2.8V as Battery Cut off Voltage & 4.2V as Maximum Voltage
  if (bat_percentage >= 100)
  {
    bat_percentage = 100;
  }
  if (bat_percentage <= 0)
  {
    bat_percentage = 1;
  }
  
  // Print the battery voltage to the serial monitor
  Serial.print("Battery voltage: ");
  Serial.print(voltage);
  Serial.println("V");
  Serial.print("Battery estimated precentage: ");
  Serial.print(bat_percentage);
  Serial.println("%");
  
  delay(100);
  /*
  Serial.println(readValue);
  float voltage = (((readValue * 3.3) / 1024) * 2 + calibration); //multiply by two as voltage divider network is 100K & 100K Resistor
  //float voltage = readValue * (3.7 / 1023.0); //calculates real world voltage
  Serial.print("Voltage = "); //show “voltage before value on serial monitor
  Serial.println(voltage); //show value on serial monitor
  delay(250); //250ms delay
*/
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


  message.sender.name = sender_name;
  message.sender.email = sender_email;
  message.subject = email_subject;
  message.addRecipient(Recipient_name,Recipient_email);

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