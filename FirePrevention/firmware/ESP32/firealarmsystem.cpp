// ******* FIRE PREVENTION AND ALARM SYSTEM
#include <Arduino.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include <HTTPClient.h>
#include <Firebase_ESP_Client.h>
#include "DHT.h"
#define DHTPIN 34 // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert Firebase project API Key
#define API_KEY "FIREBASE_PROJECT_API_KEY"

// Insert RTDB URLefine the RTDB URL 
#define DATABASE_URL "FIREBASE_DATABASE_URL" 

// For the email service
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465
// The sign in credentials 
#define AUTHOR_EMAIL "mynamesgmail.com"
#define AUTHOR_PASSWORD "my_email_password"

// Recipient's email
#define RECIPIENT_EMAIL "receiveremail@gmail.com"

// The SMTP Session object used for Email sending 
SMTPSession smtp;

// Callback function to get the Email sending status 
void smtpCallback(SMTP_Status status);

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

bool signupOK = false;

// Temperature sensor instance
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "WIFI_NAME";
const char* password = "WIFI_PASS";

const int ALARM = 12;
const int GAS = 33;
const int FLAME = 32;

int counter = 0;  
static bool hasWifi = false;

//////////////
// Network //
////////////

static void InitWifi()
{
  Serial.println("Connecting...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  hasWifi = true;
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);
  pinMode(FLAME, INPUT);
  pinMode(GAS, INPUT);
  Serial.println("ESP32 Device");
  Serial.println("Initializing...");

    // Assign the api key (required) 
  config.api_key = API_KEY;

  // Assign the RTDB URL (required) 
  config.database_url = DATABASE_URL;

  // Sign up 
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  // Assign the callback function for the long running token generation task 
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  dht.begin();

  // Initialize the WiFi module
  Serial.println(" > WiFi");
  hasWifi = false;
  InitWifi();
  if (!hasWifi)
  {
    return;
  }

  smtp.callback(smtpCallback);

  // Declare the session config data 
  ESP_Mail_Session session;

  // Set the session config 
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  // Declare the message class 
  SMTP_Message message;

    // Set email parameters
    message.sender.name = "ESP_FIRE";
    message.sender.email = AUTHOR_EMAIL;
    message.subject = "FIRE DETECTED";
    message.addRecipient("Somebody", RECIPIENT_EMAIL);

}

//////////////
//Send Data//
////////////

void loop(){
  if (WiFi.status() == WL_CONNECTED) { //if we are connected
    counter = 0; //reset counter
    Serial.println("Wifi is still connected with IP: "); 
    Serial.println(WiFi.localIP());   //inform user about his IP address
  }else if (WiFi.status() != WL_CONNECTED) { //if we lost connection, retry
    WiFi.begin(ssid);      
  }
  while (WiFi.status() != WL_CONNECTED) { //during lost connection, print dots
    delay(500);
    Serial.print(".");
    counter++;
    if(counter>=60){ //30 seconds timeout - reset board
    ESP.restart();
    }
  }

  // Reading temperature, flame and gas values
  float t = dht.readTemperature();
  int f = analogRead(FLAME);
  int g = analogRead(GAS);
  
  // if connection and signup is successful, post sensor values 
  if (Firebase.ready() && signupOK){
    Firebase.RTDB.setFloat(&fbdo, "data/temp", t);
    Firebase.RTDB.setInt(&fbdo, "data/flame", f);
    Firebase.RTDB.setInt(&fbdo, "data/gas", g);
  }

  // if flame or gas or high temperatures is detected, activete fire alarm status
  if(f<1000 || g>1000 || t>100){
    // if connection and signup is successful, change alarm status to active(1)  
    if (Firebase.ready() && signupOK){
        Firebase.RTDB.setInt(&fbdo, "data/alarm", 1);
    }

    //Send the email
    String textMsg = "Fire has been detected. Do something asap! ";
    message.text.content = textMsg.c_str();
    message.text.charSet = "us-ascii";
    message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
    if (!MailClient.sendMail(&smtp, &message))
      Serial.println("Error sending Email, " + smtp.errorReason());

    digitalWrite (ALARM, HIGH); //sound alarm for 10 seconds
    delay(10000);
    digitalWrite (ALARM, LOW); //stop alarm

  }
  else{
    // if connection and signup is successful, change alarm status to inactive(0)
    // if fire is not detected anymore
    if (Firebase.ready() && signupOK){
        Firebase.RTDB.setInt(&fbdo, "data/alarm", 0);
    }
  }

}