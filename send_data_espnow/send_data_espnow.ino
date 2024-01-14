
// Include Libraries
#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_AHT10.h>

Adafruit_AHT10 aht;



 
// Variables for test data
int int_value;
float float_value;
bool bool_value = true;
float humi;
float temp1;
 
// MAC Address of responder - edit as required
uint8_t broadcastAddress[] = {0xB0, 0xA7, 0x32, 0x15, 0xAD, 0x34};
 
// Define a data structure
typedef struct struct_message {
  char a[32];
  float b;
  float c;
  bool d;
} struct_message;
 
// Create a structured object
struct_message myData;
 
// Peer info
esp_now_peer_info_t peerInfo;
 
// Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  
  // Set up Serial Monitor
  Serial.begin(115200);

  if (!aht.begin()) {
    Serial.println("Couldn't find AHT10 sensor, check wiring!");
    while (1);
  }
 
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
 
  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
 
  // Register the send callback
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

 
  // Create test data
  humi=humidity.relative_humidity;
  temp1= temp.temperature;

 
 
  // Generate a random integer
  int_value = random(1,20);
 
  // Use integer to make a new float
  float_value = 1.3 * int_value;
 
  // Invert the boolean value
  bool_value = !bool_value;
  
  // Format structured data
  strcpy(myData.a, "Patient's Chart");
  myData.b = humi;
  myData.c = temp1;
  myData.d = bool_value;
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sending confirmed");
  }
  else {
    Serial.println("Sending error");
  }
  delay(2000);
}