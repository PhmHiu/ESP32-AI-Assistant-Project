/*
 * ESP32 AI Assistant Template
 * 
 * This is a template for creating your own AI-powered assistant using ESP32.
 * The assistant can:
 * - See through a camera (OV3660)
 * - Read sensor data (you add your own sensors!)
 * - Chat with you using advanced AI (OpenRouter API)
 * - Have a custom personality and role
 * 
 * Workshop Instructions:
 * 1. Fill in the CONFIGURATION section below
 * 2. Add your sensor reading code in the SENSOR FUNCTIONS section
 * 3. Customize the system prompt and personality
 * 4. Upload to your ESP32 and start chatting!
 */

// ==================== LIBRARIES ====================
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "esp_camera.h"
#include "mbedtls/base64.h"

// ==================== CAMERA PIN DEFINITIONS ====================
// Pin configuration for Freenove ESP32-S3 WROOM with camera
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     15
#define SIOD_GPIO_NUM      4
#define SIOC_GPIO_NUM      5
#define Y9_GPIO_NUM       16
#define Y8_GPIO_NUM       17
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       12
#define Y5_GPIO_NUM       10
#define Y4_GPIO_NUM        8
#define Y3_GPIO_NUM        9
#define Y2_GPIO_NUM       11
#define VSYNC_GPIO_NUM     6
#define HREF_GPIO_NUM      7
#define PCLK_GPIO_NUM     13

// ==================== CONFIGURATION SECTION ====================
// STUDENTS: EDIT THIS SECTION WITH YOUR SETTINGS

// WiFi Credentials
const char* WIFI_SSID = "YOUR_WIFI_NAME";        // Replace with your WiFi name
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD"; // Replace with your WiFi password

// OpenRouter API Configuration
const char* OPENROUTER_API_KEY = "YOUR_API_KEY_HERE"; // Get your API key from https://openrouter.ai/
const char* OPENROUTER_ENDPOINT = "https://openrouter.ai/api/v1/chat/completions";

// Model Selection - Choose one or add your own!
// Uncomment the model you want to use (remove the // before it)
const char* AI_MODEL = "google/gemini-2.5-flash";  // Fast, supports vision

// Other models:
// const char* AI_MODEL = "openai/gpt-4o";
// const char* AI_MODEL = "google/gemini-2.5-pro";

// ==================== CUSTOMIZE YOUR AI ASSISTANT ====================
// STUDENTS: THIS IS WHERE YOU MAKE YOUR AI UNIQUE!

// System Prompt: Describes what your AI assistant should DO
// Example roles: Weather reporter, Plant doctor, Security guard, Pet monitor, etc.
const char* SYSTEM_PROMPT = 
  "You are a helpful AI assistant with access to a camera and sensors. "
  "Your job is to help the user understand what's happening in their environment. "
  "When you receive sensor data and images, analyze them carefully and provide useful insights.";

// Personality: How your AI should TALK and BEHAVE
// Make it fun! Give it character!
const char* AI_PERSONALITY = 
  "You are friendly, enthusiastic, and curious. "
  "You use simple language and occasional emojis to make conversations fun. "
  "You're always excited to help and learn about the world through your sensors!";

// ==================== GLOBAL VARIABLES ====================
String lastResponse = "";
bool cameraInitialized = false;

// ==================== CAMERA INITIALIZATION ====================
bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Image quality settings (balance between quality and memory)
  // QVGA is good for ESP32's limited memory
  if(psramFound()){
    config.frame_size = FRAMESIZE_QVGA;  // 320x240
    config.jpeg_quality = 10;             // 0-63, lower = better quality
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return false;
  }
  
  Serial.println("Camera initialized successfully!");
  return true;
}

// ==================== SENSOR FUNCTIONS ====================
// STUDENTS: ADD YOUR SENSOR CODE HERE!
// These are placeholder functions. Replace them with your actual sensor code.

// Example: Temperature Sensor
// TODO: Connect your temperature sensor and implement this function
float readTemperature() {
  // YOUR CODE HERE
  // Example: return dht.readTemperature();
  // For now, returning a placeholder value
  return -999.0; // -999 indicates: not implemented yet
}

// Example: Light Level Sensor
// TODO: Connect your light sensor and implement this function
int readLightLevel() {
  // YOUR CODE HERE
  // Example: return analogRead(LIGHT_SENSOR_PIN);
  // For now, returning a placeholder value
  return -1; // -1 indicates: not implemented yet
}

// Custom Sensor 1
// TODO: Add your first custom sensor here
String readCustomSensor1() {
  // YOUR CODE HERE
  // Examples: Motion sensor, humidity, distance, sound level, etc.
  return "not_implemented"; // Replace with actual sensor reading
}

// Custom Sensor 2
// TODO: Add your second custom sensor here
String readCustomSensor2() {
  // YOUR CODE HERE
  // Examples: Gas sensor, pressure, soil moisture, etc.
  return "not_implemented"; // Replace with actual sensor reading
}

// You can add more sensor functions as needed!
// Just copy the pattern above and give them descriptive names.

// ==================== SENSOR DATA AGGREGATION ====================
String getSensorData() {
  // This function collects all sensor readings and formats them
  // into a readable string that will be sent to the AI
  
  String sensorReport = "\n\n=== SENSOR READINGS ===\n";
  
  // Read temperature
  float temp = readTemperature();
  if (temp != -999.0) {
    sensorReport += "Temperature: " + String(temp) + "°C\n";
  }
  
  // Read light level
  int light = readLightLevel();
  if (light != -1) {
    sensorReport += "Light Level: " + String(light) + "\n";
  }
  
  // Read custom sensors
  String custom1 = readCustomSensor1();
  if (custom1 != "not_implemented") {
    sensorReport += "Sensor 1: " + custom1 + "\n";
  }
  
  String custom2 = readCustomSensor2();
  if (custom2 != "not_implemented") {
    sensorReport += "Sensor 2: " + custom2 + "\n";
  }
  
  // STUDENTS: Add more sensor readings here following the same pattern
  
  sensorReport += "=======================\n";
  
  return sensorReport;
}

// ==================== IMAGE CAPTURE AND ENCODING ====================
String captureAndEncodeImage() {
  if (!cameraInitialized) {
    Serial.println("Camera not initialized!");
    return "";
  }
  
  Serial.println("Capturing image...");
  
  // Capture image
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed!");
    return "";
  }
  
  Serial.printf("Image captured: %d bytes\n", fb->len);
  
  // Calculate base64 output size
  size_t base64_len = 0;
  mbedtls_base64_encode(NULL, 0, &base64_len, fb->buf, fb->len);
  
  // Allocate buffer for base64 string
  char* base64_buffer = (char*)malloc(base64_len + 1);
  if (!base64_buffer) {
    Serial.println("Failed to allocate memory for base64 encoding!");
    esp_camera_fb_return(fb);
    return "";
  }
  
  // Encode to base64
  size_t output_len = 0;
  int ret = mbedtls_base64_encode(
    (unsigned char*)base64_buffer, 
    base64_len, 
    &output_len, 
    fb->buf, 
    fb->len
  );
  
  // Return camera buffer
  esp_camera_fb_return(fb);
  
  if (ret != 0) {
    Serial.println("Base64 encoding failed!");
    free(base64_buffer);
    return "";
  }
  
  base64_buffer[output_len] = '\0';
  
  // Create data URI
  String base64Image = "data:image/jpeg;base64," + String(base64_buffer);
  
  free(base64_buffer);
  
  Serial.println("Image encoded successfully!");
  return base64Image;
}

// ==================== JSON PAYLOAD BUILDER ====================
String buildRequestPayload(String userMessage, String base64Image, String sensorData) {
  // Using ArduinoJson to build the request payload
  // Adjust capacity based on your needs (this is generous for large payloads)
  DynamicJsonDocument doc(8192);
  
  // Set model
  doc["model"] = AI_MODEL;
  
  // Create messages array
  JsonArray messages = doc.createNestedArray("messages");
  
  // Add system message (combines system prompt and personality)
  JsonObject systemMessage = messages.createNestedObject();
  systemMessage["role"] = "system";
  systemMessage["content"] = String(SYSTEM_PROMPT) + "\n\n" + String(AI_PERSONALITY);
  
  // Add user message with text, sensor data, and image
  JsonObject userMsg = messages.createNestedObject();
  userMsg["role"] = "user";
  
  // Content is an array with multiple parts
  JsonArray content = userMsg.createNestedArray("content");
  
  // Part 1: Text message
  JsonObject textPart = content.createNestedObject();
  textPart["type"] = "text";
  textPart["text"] = userMessage + sensorData;
  
  // Part 2: Image (if available)
  if (base64Image.length() > 0) {
    JsonObject imagePart = content.createNestedObject();
    imagePart["type"] = "image_url";
    JsonObject imageUrl = imagePart.createNestedObject("image_url");
    imageUrl["url"] = base64Image;
  }
  
  // Serialize to string
  String payload;
  serializeJson(doc, payload);
  
  return payload;
}

// ==================== API COMMUNICATION ====================
void chatWithAI(String userMessage) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected!");
    return;
  }
  
  Serial.println("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("Processing your request...");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
  
  // Step 1: Capture image
  String base64Image = captureAndEncodeImage();
  
  // Step 2: Read sensors
  String sensorData = getSensorData();
  Serial.println(sensorData);
  
  // Step 3: Build request payload
  String payload = buildRequestPayload(userMessage, base64Image, sensorData);
  
  // Step 4: Send HTTP POST request
  HTTPClient http;
  http.setTimeout(30000); // 30 second timeout for AI response
  http.begin(OPENROUTER_ENDPOINT);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(OPENROUTER_API_KEY));
  
  Serial.println("Sending request to AI...");
  int httpResponseCode = http.POST(payload);
  
  if (httpResponseCode > 0) {
    Serial.printf("Response received (HTTP %d)\n\n", httpResponseCode);
    
    String response = http.getString();
    
    // Parse response
    DynamicJsonDocument responseDoc(8192);
    DeserializationError error = deserializeJson(responseDoc, response);
    
    if (!error) {
      const char* aiResponse = responseDoc["choices"][0]["message"]["content"];
      
      if (aiResponse) {
        Serial.println("╔════════════════════════════════════════╗");
        Serial.println("║           AI ASSISTANT SAYS:           ║");
        Serial.println("╚════════════════════════════════════════╝");
        Serial.println();
        Serial.println(aiResponse);
        Serial.println();
        Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
        
        lastResponse = String(aiResponse);
      } else {
        Serial.println("No response content in API reply");
        Serial.println("Raw response:");
        Serial.println(response);
      }
    } else {
      Serial.print("JSON parsing error: ");
      Serial.println(error.c_str());
      Serial.println("Raw response:");
      Serial.println(response);
    }
  } else {
    Serial.printf("HTTP Request failed: %d\n", httpResponseCode);
    Serial.println(http.errorToString(httpResponseCode));
  }
  
  http.end();
}

// ==================== SETUP ====================
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // IMPORTANT: Wait for Serial connection (ESP32-S3 USB CDC needs this)
  delay(2000);  // Initial delay for USB CDC to initialize
  
  // Wait for Serial port to be ready (up to 5 seconds)
  unsigned long startTime = millis();
  while (!Serial && (millis() - startTime < 5000)) {
    delay(10);
  }
  
  Serial.println("\n\n");
  Serial.println("╔═══════════════════════════════════════════╗");
  Serial.println("║     ESP32 AI ASSISTANT WORKSHOP           ║");
  Serial.println("║     Starting up...                        ║");
  Serial.println("╚═══════════════════════════════════════════╝");
  Serial.println();
  
  // Connect to WiFi with better diagnostics
  Serial.println("Connecting to WiFi...");
  Serial.print("SSID: ");
  Serial.println(WIFI_SSID);
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);  // Clear any previous WiFi config
  delay(100);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int wifiRetries = 0;
  while (WiFi.status() != WL_CONNECTED && wifiRetries < 40) {
    delay(500);
    Serial.print(".");
    wifiRetries++;
    
    // Show detailed status every 10 attempts
    if (wifiRetries % 10 == 0) {
      Serial.println();
      Serial.print("WiFi Status: ");
      switch(WiFi.status()) {
        case WL_NO_SSID_AVAIL:
          Serial.println("Network not found!");
          break;
        case WL_CONNECT_FAILED:
          Serial.println("Connection failed - check password!");
          break;
        case WL_CONNECTION_LOST:
          Serial.println("Connection lost!");
          break;
        case WL_DISCONNECTED:
          Serial.println("Disconnected - retrying...");
          break;
        default:
          Serial.print("Code: ");
          Serial.println(WiFi.status());
      }
    }
  }
  
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("WiFi connection failed!");
    Serial.println("Please check:");
    Serial.println("  1. SSID is correct");
    Serial.println("  2. Password is correct");
    Serial.println("  3. Network is 2.4GHz (ESP32 doesn't support 5GHz)");
    Serial.println("  4. Try using a mobile hotspot");
    Serial.println("\nThe system will continue, but you won't be able to chat with AI.");
  }
  
  // Initialize camera
  Serial.println("\nInitializing camera...");
  cameraInitialized = initCamera();
  
  if (!cameraInitialized) {
    Serial.println("Camera initialization failed!");
    Serial.println("Check camera connections and try again.");
  }
  
  // Initialize your sensors here
  Serial.println("\nInitializing sensors...");
  // STUDENTS: Add sensor initialization code here
  // Examples:
  // pinMode(LIGHT_SENSOR_PIN, INPUT);
  // dht.begin();
  // etc.
  Serial.println("Sensor setup complete!");
  
  // Ready!
  Serial.println("\n╔═══════════════════════════════════════════╗");
  Serial.println("║       SYSTEM READY!                      ║");
  Serial.println("╚═══════════════════════════════════════════╝");
  Serial.println();
  Serial.println("Type your message and press Enter to chat with your AI assistant!");
  Serial.println("Example: 'What do you see?' or 'Tell me about the environment'");
  Serial.println();
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
}

// ==================== MAIN LOOP ====================
void loop() {
  // Check if user has sent a message via Serial Monitor
  if (Serial.available() > 0) {
    String userMessage = Serial.readStringUntil('\n');
    userMessage.trim();
    
    if (userMessage.length() > 0) {
      Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
      Serial.print("You: ");
      Serial.println(userMessage);
      
      // Send to AI
      chatWithAI(userMessage);
      
      // Ready for next message
      Serial.println("\nSend another message or ask a follow-up question!");
      Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    }
  }
  
  // Small delay to prevent overwhelming the system
  delay(100);
}
