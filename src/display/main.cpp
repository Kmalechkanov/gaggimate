#include "main.h"

#ifndef GAGGIMATE_HEADLESS
#include <lvgl.h>
#endif

Controller controller;

void setup() {
    Serial.begin(115200);
    while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
    }
    ESP_LOGE("TEST", "Error message");
    ESP_LOGW("TEST", "Warning message");
    ESP_LOGI("TEST", "Info message");
    ESP_LOGD("TEST", "Debug message");
    ESP_LOGV("TEST", "Verbose message");
    controller.setup();
    Serial.println("finish setup");  
}

void loop() {
    controller.loop();
    delay(2);
}
