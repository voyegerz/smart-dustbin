

#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <NewPing.h>
#include <ESP32Servo.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "ESP"
#define WIFI_PASSWORD "abcdefghijkl"

// For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyBKda-TNcLHazffBsAHs_tjhuklyL8ahIg"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://smart-bin-88a0d-default-rtdb.asia-southeast1.firebasedatabase.app/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "neeraj@gmail.com"
#define USER_PASSWORD "123456"


// for fill level ultrasonic
#define fill_TRIGGER_PIN 12
#define fill_ECHO_PIN 13
// #define MAX_DISTANCE 200


// for the distance & detection of hand ultrasonic
#define detect_TRIGGER_PIN 32
#define detect_ECHO_PIN 33

#define SERVO_PIN 27

NewPing fill_level_sonar(fill_TRIGGER_PIN, fill_ECHO_PIN);

NewPing detect_sonar(detect_TRIGGER_PIN, detect_ECHO_PIN);

Servo myservo;


// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

void setup()
{

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  // Or use legacy authenticate method
  // config.database_url = DATABASE_URL;
  // config.signer.tokens.legacy_token = "<database secret>";

  // To connect without auth in Test Mode, see Authentications/TestMode/TestMode.ino

  //////////////////////////////////////////////////////////////////////////////////////////////
  // Please make sure the device free Heap is not lower than 80 k for ESP32 and 10 k for ESP8266,
  // otherwise the SSL connection will fail.
  //////////////////////////////////////////////////////////////////////////////////////////////

  Firebase.begin(&config, &auth);

  // Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

  /*
  Note:
  The function that starting the new TCP session i.e. first time server connection or previous session was closed, the function won't exit until the
  time of config.timeout.socketConnection.

  You can also set the TCP data sending retry with
  config.tcp_data_sending_retry = 1;

  */
  myservo.attach(SERVO_PIN);
  myservo.write(0); // Initialize the servo position to 0 degrees

}

void loop()
{

  //delay(50);
  unsigned int distance = detect_sonar.ping_cm();
  unsigned int fill_value = fill_level_sonar.ping_cm();

  Serial.println(distance);
  Serial.println(fill_value);

  myservo.attach(SERVO_PIN);

  if (distance <= 10) {
    myservo.write(90); // Rotate the servo to 90 degrees
    //delay(50);
  } else {
    myservo.write(0); // Rotate the servo to 0 degrees
    //delay(50);
  }
    delay(200);
  myservo.detach();
  // delay(1000);
  // // Firebase.ready() should be called repeatedly to handle authentication tasks.


    // For the usage of FirebaseJson, see examples/FirebaseJson/BasicUsage/Create_Edit_Parse.ino 
    FirebaseJson json;

      json.set(F("detect_distance"), + distance);
      json.set(F("fill_value"), fill_value);
      Serial.printf("Set json... %s\n", Firebase.set(fbdo, F("smart-bin"), json) ? "ok" : fbdo.errorReason().c_str());

    Serial.println();
    delay(1000);

}
