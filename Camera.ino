#include "esp_camera.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#include "wifi_credentials.h"    // WiFi SSID and password
#define CAMERA_MODEL_AI_THINKER   // Camera model for AI Thinker module
#include "camera_pins.h"         // Pin definitions for ESP32-CAM

#include "SquatCounter.h"        // Embedded HTML for squat counter UI

// Create an asynchronous web server on port 80
AsyncWebServer server(80);

/**
 * @brief Configure and start the squat-counter web server
 */
static void startSquatServer() {
  // Serve the embedded HTML at root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });
  // Handle 404 errors
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
  });
  server.begin();
}

void setup() {
  // Initialize serial for debugging
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // Camera configuration via field assignments
  camera_config_t config;
  config.ledc_timer   = LEDC_TIMER_0;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size   = FRAMESIZE_UXGA;
  config.grab_mode    = CAMERA_GRAB_LATEST;
  config.fb_count     = 2;

  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return;
  }

  // Adjust sensor settings for higher frame rate
  sensor_t *sensor = esp_camera_sensor_get();
  sensor->set_framesize(sensor, FRAMESIZE_QVGA);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(300);
  }
  Serial.println();
  Serial.print("Wi-Fi connected. IP: ");
  Serial.println(WiFi.localIP());

  // Start the squat-counter web server
  startSquatServer();
  Serial.print("Server running at http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop() {
  // Everything is handled asynchronously by the web server
  delay(10000);
}
