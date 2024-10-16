#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define SERVER_URL "http://192.168.1.31:3000/"  // Replace with your backend URL or IP address
#define REGISTER_ENDPOINT "register"
#define MESSAGE_ENDPOINT "send_message"
#define GET_DEVICES_ENDPOINT "get_registered_devices"
#define GET_MESSAGES_ENDPOINT "get_messages"

// Function to get the unique device ID based on the ESP32's MAC address
String getDeviceID() {
  uint64_t chipId = ESP.getEfuseMac(); // The chip ID is the MAC address of the ESP32

  // Extract the full 48-bit MAC address
  uint8_t mac[6];
  for (int i = 0; i < 6; i++) {
    mac[i] = (chipId >> (8 * (5 - i))) & 0xFF;
  }

  // Format the MAC address as a hexadecimal string for uniqueness
  String deviceID = "esp32_device_";
  for (int i = 0; i < 6; i++) {
    if (mac[i] < 16) deviceID += "0";  // Add a leading zero if needed
    deviceID += String(mac[i], HEX);
  }
  
  // Optional: Convert the MAC address to uppercase
  deviceID.toUpperCase();

  return deviceID;
}

String DEVICE_ID;  // Dynamically assigned device ID
String recipient_device_id;
bool registered = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Setup started");

  // Generate and assign the dynamic device ID
  DEVICE_ID = getDeviceID();
  Serial.println("Assigned DEVICE_ID: " + DEVICE_ID);

  // Initialize WiFi
  WiFi.begin("YOUR WIFI SSID HERE", "YOUR PASS HERE");  // Replace with your WiFi credentials
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected!");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connection failed!");
    return;
  }

  // Register the device on the blockchain
  registerDevice();

  // Retrieve the list of registered devices and let the user select a recipient
  listRegisteredDevices();
}

void loop() {
  // Check for input via serial monitor and send a message
  if (Serial.available() > 0) {
    String message = Serial.readString();
    sendMessage(message);
  }

  // Continuously monitor for incoming messages
  receiveMessages();
  delay(10000); // Check for new messages every 10 seconds
}

// Register device on blockchain via backend
void registerDevice() {
  Serial.println("Attempting to register device...");
  Serial.println("Device ID: " + DEVICE_ID);  // Print the dynamic device ID

  if (WiFi.status() == WL_CONNECTED && !registered) {
    HTTPClient http;

    http.begin(String(SERVER_URL) + String(REGISTER_ENDPOINT));
    http.addHeader("Content-Type", "application/json");

    // Prepare the JSON payload
    String postData = "{\"device_id\":\"" + DEVICE_ID + "\"}";
    Serial.println("Sending JSON payload: " + postData);

    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Registration Response: " + response);

      // Parse the response and check if registration was successful
      if (response.indexOf("Device registered successfully") > -1) {
        registered = true;
      }
    } else {
      Serial.println("Error in sending registration request, HTTP response code: " + String(httpResponseCode));
    }
    http.end();
    
    delay(10000); // 10 seconds delay to ensure transaction completion
  } else {
    Serial.println("Device is not connected to WiFi or already registered.");
  }
}

// Retrieve the list of registered devices
void listRegisteredDevices() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(String(SERVER_URL) + String(GET_DEVICES_ENDPOINT));
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Registered Devices: " + response);

      // Let user choose the recipient
      Serial.println("Enter the device ID to send a message to:");
      while (!Serial.available()) {
        // Wait for user input
      }
      recipient_device_id = Serial.readString();
      Serial.println("Selected device: " + recipient_device_id);
    } else {
      Serial.println("Failed to retrieve registered devices, HTTP response code: " + String(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("WiFi not connected, unable to retrieve registered devices.");
  }
}

// Send a plain text message to the recipient
void sendMessage(String message) {
  if (message.length() == 0) {
    Serial.println("Message is empty, cannot send.");
    return; // Do not proceed if the message is empty
  }

  // Trim any newline characters from the message and recipient_device_id
  message.trim();
  recipient_device_id.trim();

  // Prepare the JSON payload
  String postData = "{\"sender\":\"" + DEVICE_ID + "\", \"recipient\":\"" + recipient_device_id + "\", \"message\":\"" + message + "\"}";

  // Debugging: Print the full JSON payload before sending
  Serial.println("Sending message payload: " + postData);

  // Send the plain text message to the backend
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(String(SERVER_URL) + String(MESSAGE_ENDPOINT));
    http.addHeader("Content-Type", "application/json");

    // Send the HTTP POST request
    int httpResponseCode = http.POST(postData);

    // Check the response from the server
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Message Sent: " + response);
    } else {
      Serial.println("Error sending message to blockchain, HTTP response code: " + String(httpResponseCode));
    }

    http.end();
  } else {
    Serial.println("WiFi not connected, unable to send message.");
  }
}

// Retrieve and display messages from the blockchain
void receiveMessages() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(String(SERVER_URL) + String(GET_MESSAGES_ENDPOINT) + "?recipient=" + DEVICE_ID);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String messages = http.getString();
      Serial.println("Received Messages: " + messages);
    } else {
      Serial.println("Failed to get messages from blockchain, HTTP response code: " + String(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("WiFi not connected, unable to receive messages.");
  }
}