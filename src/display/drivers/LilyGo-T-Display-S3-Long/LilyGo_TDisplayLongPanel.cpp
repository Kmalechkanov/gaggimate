#include "LilyGo_TDisplayLongPanel.h"
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include "TouchDrvAXS15231B.h"
#include "AXS15231.h"
#include <Wire.h>
#include "pin_config.h"
#include <esp_adc_cal.h>

#undef min
#undef max
#include <algorithm>
using std::min;
using std::max;

LilyGo_TDisplayLongPanel::LilyGo_TDisplayLongPanel()
    : displayBus(nullptr), display(nullptr), _touchDrv(nullptr), _wakeupMethod(LILYGOWIDE_T_DISPLAY_WAKEUP_FROM_NONE),
      _sleepTimeUs(0), currentBrightness(0) {
    _rotation = 0;
}

LilyGo_TDisplayLongPanel::~LilyGo_TDisplayLongPanel() {
    uninstallSD();

    if (_touchDrv) {
        delete _touchDrv;
        _touchDrv = nullptr;
    }
    if (display) {
        display->setBrightness(0);
        digitalWrite(TFT_BL, LOW);
        delete display;
        display = nullptr;
    }
    if (displayBus) {
        delete displayBus;
        displayBus = nullptr;
    }
}

bool LilyGo_TDisplayLongPanel::begin() {
    bool displayOK = initDisplay();
    if (!displayOK) {
        log_e("Display initialization failed!");
        return false;
    }
    
    bool touchOK = initTouch();
    if (!touchOK) {
        printf("LillyGoTDisplayLongDriver InitTuch failed\n");
        log_w("Touch initialization failed — continuing without touch support.");

    }

    return true;
}


bool LilyGo_TDisplayLongPanel::installSD() {
    return false;
}

void LilyGo_TDisplayLongPanel::uninstallSD() {
}

void LilyGo_TDisplayLongPanel::setBrightness(uint8_t level) {
    uint16_t brightness = level * 16;

    brightness = brightness > 255 ? 255 : brightness;
    brightness = brightness < 0 ? 0 : brightness;

    if (brightness > this->currentBrightness) {
        for (int i = this->currentBrightness; i <= brightness; i++) {
            display->setBrightness(i);
            delay(1);
        }
    } else {
        for (int i = this->currentBrightness; i >= brightness; i--) {
            display->setBrightness(i);
            delay(1);
        }
    }
    this->currentBrightness = brightness;
}

uint8_t LilyGo_TDisplayLongPanel::getBrightness() { return (this->currentBrightness + 1) / 16; }

const char *LilyGo_TDisplayLongPanel::getTouchModelName() { return _touchDrv->getModelName(); }

void LilyGo_TDisplayLongPanel::enableTouchWakeup() { _wakeupMethod = LILYGOWIDE_T_DISPLAY_WAKEUP_FROM_TOUCH; }

void LilyGo_TDisplayLongPanel::enableButtonWakeup() { _wakeupMethod = LILYGOWIDE_T_DISPLAY_WAKEUP_FROM_BUTTON; }

void LilyGo_TDisplayLongPanel::enableTimerWakeup(uint64_t time_in_us) {
    _wakeupMethod = LILYGOWIDE_T_DISPLAY_WAKEUP_FROM_TIMER;
    _sleepTimeUs = time_in_us;
}

void LilyGo_TDisplayLongPanel::sleep() {
}

void LilyGo_TDisplayLongPanel::wakeup() {}

uint8_t LilyGo_TDisplayLongPanel::getPoint(int16_t *x_array, int16_t *y_array, uint8_t get_point) {
    if (!_touchDrv || !_touchDrv->isPressed()) {
        return 0;
    }

    uint8_t points = _touchDrv->getPoint(x_array, y_array, get_point);

     for (uint8_t i = 0; i < points; i++) {
        int16_t rawX = x_array[i];// + hwConfig.lcd_gram_offset_x;
        int16_t rawY = y_array[i];// + hwConfig.lcd_gram_offset_y;

        switch (_rotation) {
        case 1: // 90°
            x_array[i] = rawY;
            y_array[i] = width() - rawX;
            break;
        case 2: // 180°
            x_array[i] = width() - rawX;
            y_array[i] = height() - rawY;
            break;
        case 3: // 270°
            x_array[i] = height() - rawY;
            y_array[i] = rawX;
            break;
        default: // 0°
            x_array[i] = rawX;
            y_array[i] = rawY;
            break;
        }
    }

    return points;
}

bool LilyGo_TDisplayLongPanel::isPressed() {
    if (_touchDrv) {
        return _touchDrv->isPressed();
    }
    return 0;
}

uint16_t LilyGo_TDisplayLongPanel::getBattVoltage(void) {
    return 0;
}

void LilyGo_TDisplayLongPanel::pushColors(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t *data) {
    if (!(displayBus && display)) {
        return;
    }

     int16_t startX = max((int16_t)0, (int16_t)x);
    int16_t startY = max((int16_t)0, (int16_t)y);
    int16_t clipW  = min((int16_t)(width  - (startX - x)), (int16_t)(TFT_HEIGHT  - startX));
    int16_t clipH  = min((int16_t)(height - (startY - y)), (int16_t)(TFT_WIDTH - startY));

    if (clipW <= 0 || clipH <= 0) {
        printf("Nothing to draw (clipped completely outside screen)\n");
        return;
    }

    uint16_t *rotated = (uint16_t*)malloc(clipW * clipH * sizeof(uint16_t));
    if (!rotated) {
        printf("Failed to allocate rotation buffer\n");
        return;
    }

    // Rotate 90° clockwise: data[y*width + x] -> rotated[x*clipH + (clipH - y - 1)]
    for (int16_t row = 0; row < clipH; row++) {
        for (int16_t col = 0; col < clipW; col++) {
            rotated[col * clipH + (clipH - row - 1)] = 
                data[(row + (startY - y)) * width + (col + (startX - x))];
        }
    }

    // Draw rotated bitmap (width and height swapped)
    display->draw16bitRGBBitmap(startX, startY, rotated, clipH, clipW);

    free(rotated);
}

void LilyGo_TDisplayLongPanel::setRotation(uint8_t rotation) {
    _rotation = rotation;

    if (displayBus && display) {
        display->setRotation(rotation);
    }
}

bool LilyGo_TDisplayLongPanel::initTouch() {
    TouchDrvAXS15231B *tmp = new TouchDrvAXS15231B();

    if (tmp->begin()) {
        _touchDrv = tmp;
        log_i("Successfully initialized %s touch controller!", _touchDrv->getModelName());
        return true;
    }

    log_e("Unable to initialize AXS15231B touch controller at address 0x3B.");
    return false;
}

bool LilyGo_TDisplayLongPanel::initDisplay() {

    if (displayBus == nullptr) {
        printf("LilyGo_TDisplayLongPanel Create bus\n");

        displayBus = new Arduino_ESP32QSPI(
            TFT_QSPI_CS,   // CS
            TFT_QSPI_SCK,  // SCK
            TFT_QSPI_D0,   // D0
            TFT_QSPI_D1,   // D1
            TFT_QSPI_D2,   // D2
            TFT_QSPI_D3   // D3
        );

        display = new AXS15231(displayBus, TFT_QSPI_RST, 0 /*_rotation*/, false, TFT_WIDTH, TFT_HEIGHT); 
    }
    
    
    ledcAttachPin(TFT_BL, 1);
    ledcSetup(1, 2000, 8);
    ledcWrite(1, 0);  // Start off
    for (int i = 0; i <= 150; i++) {
        ledcWrite(1, i);
        delay(3);
    }

    bool success = display->begin();
    if (!success) {
        ESP_LOGE("LilyGo_TDisplayLongPanel", "Failed to initialize display");
        return false;
    }

    // this->setBrightness(150);
    // this->setRotation(1);
    display->fillScreen(BLACK);

    ESP_LOGI("LilyGo_TDisplayLongPanel", "Success");
    return success;
}