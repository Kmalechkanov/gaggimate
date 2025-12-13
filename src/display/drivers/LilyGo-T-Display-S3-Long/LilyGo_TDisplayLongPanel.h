/**
 * @file      LilyGo_TDisplayLongPanel.h
 * @author    Borys Tymchenko // todo
 * @license   MIT
 * @copyright
 * @date      2025-05-19 // todo
 *
 */

#pragma once

#include <Arduino.h>
#include <memory>

#ifndef BOARD_HAS_PSRAM
#error "Please turn on PSRAM to OPI !"
#endif

#include "pin_config.h"
#include <SD_MMC.h>
#include <TouchDrvInterface.hpp>
#include <display/drivers/common/Display.h>
#include <display/drivers/common/ext.h>
#include "Arduino_DataBus.h"
#include "TouchDrvAXS15231B.h"
#include "AXS15231.h"
#include "Arduino_GFX_Library.h"

enum LilyGo_TDisplayLongPanel_Wakeup_Method {
    LILYGOWIDE_T_DISPLAY_WAKEUP_FROM_NONE,
    LILYGOWIDE_T_DISPLAY_WAKEUP_FROM_TOUCH,
    LILYGOWIDE_T_DISPLAY_WAKEUP_FROM_BUTTON,
    LILYGOWIDE_T_DISPLAY_WAKEUP_FROM_TIMER,
};

class LilyGo_TDisplayLongPanel : public Display {

  public:
    LilyGo_TDisplayLongPanel();

    ~LilyGo_TDisplayLongPanel();

    bool begin();

    bool installSD();

    void uninstallSD();

    void setBrightness(uint8_t level);

    uint8_t getBrightness();

    const char *getTouchModelName();

    void enableTouchWakeup();
    void enableButtonWakeup();
    void enableTimerWakeup(uint64_t time_in_us);

    void sleep();

    void wakeup();

    uint16_t width() override { return TFT_HEIGHT; }; // todo use this in drivers and respact rotation

    uint16_t height() override { return TFT_WIDTH; };

    uint8_t getPoint(int16_t *x_array, int16_t *y_array, uint8_t get_point = 1);

    bool isPressed();

    uint16_t getBattVoltage(void);

    void pushColors(uint16_t x, uint16_t y, uint16_t width, uint16_t hight, uint16_t *data);

    bool supportsDirectMode() { return true; }

    void setRotation(uint8_t rotation);

  private:
    bool initTouch();
    bool initDisplay();

  private:
    TouchDrvAXS15231B *_touchDrv = nullptr;

    Arduino_DataBus *displayBus = nullptr;
    AXS15231 *display = nullptr;

    uint8_t currentBrightness = 0;

    LilyGo_TDisplayLongPanel_Wakeup_Method _wakeupMethod;
    uint64_t _sleepTimeUs;
};
