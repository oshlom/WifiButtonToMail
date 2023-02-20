
/* 
**  Connect the ESP8266 unit to an existing WiFi access point
**  For more information see http://42bots.com
*/
#include <Arduino.h>
#include <ESP_Mail_Client.h>

// Replace these with your WiFi network settings
const char* ssid = "your SSID"; //replace this with your WiFi network name
const char* password = "your password"; //replace this with your WiFi network password
#define SMTP_server "smtp.gmail.com"
#define SMTP_Port 465
#define sender_email "someone@gmail.com" // note - you will need an "app account" for gmail, available only after setting up two factor auth on gmail
#define sender_password "somepass"
#define sender_name "esp button to mail"
#define Recipient_email "recepiant@no.mail.con"
#define Recipient_name "Dude!"
#define email_subject "esp has been triggered to send this mail"
#define WAIT_CONNECT 60 // number of secondes to wait for wifi connection. exit in error without msg if exeeded.


float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float ReadBatteryV () {
  //readValue = analogRead(A0); //read pin A0 value
  Serial.print("im reading from A0 ");
  // Read the battery voltage
  int num_readings = 10; // Set the number of readings to take
  int adc_sum = 0;
  for (int i = 0; i < num_readings; i++) {
    int aread = analogRead(A0) ; 
    adc_sum += aread; // Add the reading to the sum
    delay(10); // Wait a short time between readings
    Serial.print("analog read value of A0 is ");
    Serial.print(aread);
    Serial.print(" and adc_sum is ");
    Serial.println(adc_sum);
  }
  int adc_average = adc_sum / num_readings; // Calculate the average of the readings
  // Convert the ADC reading to voltage (0.01V reference as the reading in hundreds seems to represent the voltage)
  float Readvoltage = adc_average * 0.01 ;
  return Readvoltage;
}
int CalcBatteryP(float volts_meassured){
  // calculate the presentage of the battery where 2.8 is empty (0%) and 3.7 is full (100%)
  int bat_percentage = mapfloat(volts_meassured, 2.7, 3.7, 0, 100); //2.8V as Battery Cut off Voltage & 4.2V as Maximum Voltage
  if (bat_percentage >= 100)
  {
    bat_percentage = 100;
  }
  if (bat_percentage <= 0)
  {
    bat_percentage = 1;
  }
  return bat_percentage;
}

int ConnectToWifi(int attempts)
{
  WiFi.begin(ssid, password);
  Serial.println();
  Serial.print("Connecting");
  int loopruns = 0;

  while (WiFi.status() != WL_CONNECTED)
  {
    if (loopruns < attempts) {
      delay(1000);
      Serial.println(".");
      loopruns = loopruns + 1 ;
    }
    else
    {
      Serial.print("Failed to connect to WiFi. Giving up after ");
      Serial.print(loopruns);
      Serial.print(" seconds");
      return 1;
    }
  }
  Serial.println("success!");
  Serial.print("IP Address is: ");
  Serial.println(WiFi.localIP());
  return 0;
}

void setup()
{
  delay(1000);
  // Initialize the ADC
  //analogReadAveraging(16); // Set the number of samples to average to 16
  analogReference(0); // Set the ADC reference voltage to the internal 1.1V reference
  Serial.begin(115200);
    // initialize digital pin LED_BUILTIN as an output.


  float voltage = ReadBatteryV() ;
  int bat_percentage = CalcBatteryP(voltage) ;

// Print the battery voltage to the serial monitor
  Serial.print("Battery voltage: ");
  Serial.print(voltage);
  Serial.println("V");
  Serial.print("Battery estimated precentage: ");
  Serial.print(bat_percentage);
  Serial.println("%");
  int didiconnect = ConnectToWifi(WAIT_CONNECT) ;
  if (didiconnect == 0) {
  // smtp start
    SMTPSession smtp;
    smtp.debug(1);
    ESP_Mail_Session session;
    session.server.host_name = SMTP_server ;
    session.server.port = SMTP_Port;
    session.login.email = sender_email;
    session.login.password = sender_password;
    session.login.user_domain = "";
  // Declare the message class *
    SMTP_Message message;
    message.sender.name = sender_name;
    message.sender.email = sender_email;
    message.subject = email_subject;
    message.addRecipient(Recipient_name,Recipient_email);

    //Send HTML message
    String htmlMsg = "<div style=\"color:#FF0000;\">";
    htmlMsg = htmlMsg + "<h1>Some header goes here</h1>" ;
    htmlMsg = htmlMsg + "<p style=\"color:#050505;\">battery's measured voltage is :" ;
    htmlMsg = htmlMsg + voltage;
    htmlMsg = htmlMsg + " ,<br/>estimate battery precentage is:";
    htmlMsg = htmlMsg + bat_percentage;
    htmlMsg = htmlMsg + "</br> Bye!</p></div>";
    message.html.content = htmlMsg.c_str();
    message.text.charSet = "us-ascii";
    message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit; 
    if (!smtp.connect(&session)) {
      return; }
    if (!MailClient.sendMail(&smtp, &message)) {
      Serial.println("Error sending Email, " + smtp.errorReason());
      }
  }
}
  
void loop() {
  ESP.deepSleep(0);
  
}