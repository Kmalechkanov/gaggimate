#include "LillyGoTDisplayLongDriver.h"
#include <display/drivers/common/LV_Helper.h>
#include "Arduino.h"

LillyGoTDisplayLongDriver *LillyGoTDisplayLongDriver::instance = nullptr;

bool LillyGoTDisplayLongDriver::isCompatible() {
    return true;
}

void LillyGoTDisplayLongDriver::init() {
    printf("LillyGoTDisplayLongDriver initialzing\n");
    if (!panel.begin()) {
        for (uint8_t i = 0; i < 20; i++) {
            printf("Error, failed to initialize LillyGoTDisplayLongDriver\n");
            delay(1000);
        }
        ESP.restart();
    }
    delay(1000);

    beginLvglHelper(panel);
    panel.setBrightness(16);
}
