#include <ArduinoHttpClient.h>
#include <WiFiNINA.h>
#include <Arduino_LSM6DS3.h>

#define SECRET_SSID "MyAlti5124"
#define SECRET_PASS "31-sage-3597"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

char serverAddress[] = "192.168.1.224";
int port = 1880;

float x, y, z;
int plusThreshold = 30, minusThreshold = -30;
int collisionCode = 0; // Define an integer to represent different collision types

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;

void setup() {
  Serial.begin(9600);
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);

    status = WiFi.begin(ssid, pass);
    delay(1000);
  }

  Serial.println("WiFi connected.");

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");  
  Serial.println();
  Serial.println("Gyroscope in degrees/second");
}

void loop() {
  Serial.println("Making POST request");

  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(x, y, z);
  }
  
  // Determine collision type based on gyroscope readings
  if (y > plusThreshold) {
    collisionCode = 1; // Collision front
  } else if (y < minusThreshold) {
    collisionCode = 2; // Collision back
  } else if (x < minusThreshold) {
    collisionCode = 3; // Collision right
  } else if (x > plusThreshold) {
    collisionCode = 4; // Collision left
  } else {
    collisionCode = 0; // No collision
  }

  // Send collision code to Node-RED
  String contentType = "text/plain";
  String postData = String(collisionCode);
  
  char endpoint[] = "/newData";
  client.post(endpoint, contentType, postData);

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
  
  delay(500); // Adjust delay as needed
}
