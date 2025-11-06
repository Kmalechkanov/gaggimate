#include "LilyGo_TDisplayLongPanel.h"
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include "TouchDrvAXS15231B.h"
#include "AXS15231B.h"
#include <Wire.h>
#include "pin_config.h"
#include <esp_adc_cal.h>

LilyGo_TDisplayLongPanel::LilyGo_TDisplayLongPanel()
    : displayBus(nullptr), display(nullptr), _touchDrv(nullptr), _wakeupMethod(LILYGOWIDE_T_DISPLAY_WAKEUP_FROM_NONE),
      _sleepTimeUs(0), currentBrightness(255) {
    _rotation = 0;
}

LilyGo_TDisplayLongPanel::~LilyGo_TDisplayLongPanel() {
    uninstallSD();

    if (_touchDrv) {
        delete _touchDrv;
        _touchDrv = nullptr;
    }
    if (display) {
        // display->Display_Brightness(0);
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
    printf("LillyGoTDisplayLongDriver InitDisplay\n");
    bool displayOK = initDisplay();
    if (!displayOK) {
        printf("LillyGoTDisplayLongDriver InitDisplay Failed\n");
        log_e("Display initialization failed!");
        return false;
    }
    
    printf("LillyGoTDisplayLongDriver InitTuch\n");
    bool touchOK = initTouch();
    if (!touchOK) {
        printf("LillyGoTDisplayLongDriver InitTuch failed\n");
        log_w("Touch initialization failed — continuing without touch support.");

    }

    return true;
}

bool LilyGo_TDisplayLongPanel::installSD() {
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);

    SD_MMC.setPins(SD_SCLK, SD_MOSI, SD_MISO);

    return SD_MMC.begin("/sdcard", true, false);
}

void LilyGo_TDisplayLongPanel::uninstallSD() {
    SD_MMC.end();                       // Stop SPI SD communication
    digitalWrite(SD_CS, LOW);       // Pull CS low to avoid bus conflicts
    pinMode(SD_CS, INPUT);  
}

void LilyGo_TDisplayLongPanel::setBrightness(uint8_t level) {
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

    return points;
}

bool LilyGo_TDisplayLongPanel::isPressed() {
    if (_touchDrv) {
        return _touchDrv->isPressed();
    }
    return 0;
}

uint16_t LilyGo_TDisplayLongPanel::getBattVoltage(void) {
    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, 1100, &adc_chars);

    const int number_of_samples = 20;
    uint32_t sum = 0;
    for (int i = 0; i < number_of_samples; i++) {
        sum += analogRead(PIN_BAT_VOLT);
        delay(2);
    }
    sum = sum / number_of_samples;

    return esp_adc_cal_raw_to_voltage(sum, &adc_chars) * 2;
}

void LilyGo_TDisplayLongPanel::pushColors(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t *data) {
    printf("Panel pushColors\n");    
    if (displayBus && display) {
        //optimization instead of draw16bit
        // display->startWrite();
        // display->setAddrWindow(x, y, width, height);
        // display->writePixels(data, width * height);
        // display->endWrite();
        display->draw16bitRGBBitmap(x, y, data, width, height);
        printf("Panel draw16BitRGB\n");    
    }
}

void LilyGo_TDisplayLongPanel::setRotation(uint8_t rotation) {
    _rotation = rotation;

    if (displayBus && display) {
        display->setRotation(rotation);
    }
}

bool LilyGo_TDisplayLongPanel::initTouch() {
    TouchDrvAXS15231B *tmp = new TouchDrvAXS15231B();

    if (tmp->begin(Wire, 0x3B, TOUCH_IICSDA, TOUCH_IICSCL)) {
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

        display = new Arduino_AXS15231(displayBus, TFT_QSPI_RST, 0 /*_rotation*/, false, TFT_HEIGHT, TFT_WIDTH); 
    }
    
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    
    bool success = display->begin();
    if (!success) {
        ESP_LOGE("LilyGo_TDisplayLongPanel", "Failed to initialize display");
        return false;
    }

    display->fillScreen(WHITE);

    this->setRotation(_rotation);

    displayBus->writeCommand(CO5300_C_PTLON);
    display->fillScreen(RED);

    ESP_LOGI("LilyGo_TDisplayLongPanel", "Success");

    // for (int i = 0; i <= 255; i++)
    // {
    //     ledcWrite(1, i);
    //     delay(3);
    // }
    return success;
}