#include "main.h"

#ifndef GAGGIMATE_HEADLESS
#include <lvgl.h>
#endif

Controller controller;

void setup() {
    Serial.begin(115200);
    // while (!Serial) {
    // ;  // wait for serial port to connect. Needed for native USB port only
    // }
    controller.setup();
}

void loop() {
    controller.loop();
    delay(2);
}
