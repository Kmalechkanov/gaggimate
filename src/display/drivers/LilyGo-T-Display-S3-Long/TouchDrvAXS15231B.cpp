#include "TouchDrvAXS15231B.h"
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"
#include "Arduino.h"
#include <Wire.h>

#define LCD_WIDTH 180
#define LCD_HEIGHT 640

#define LCD_CS TFT_QSPI_CS
#define LCD_SCLK TFT_QSPI_SCK
#define LCD_SDIO0 TFT_QSPI_D0
#define LCD_SDIO1 TFT_QSPI_D1
#define LCD_SDIO2 TFT_QSPI_D2
#define LCD_SDIO3 TFT_QSPI_D3
#define LCD_RST TFT_QSPI_RST

static const uint8_t read_touchpad_cmd[] = {0xB5, 0xAB, 0xA5, 0x5A, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00};

static uint8_t Image_Flag = 0;
static size_t CycleTime = 0;

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(TOUCH_IICSDA, TOUCH_IICSCL, &Wire);

volatile bool Touch_INT = false;

void AXS15231_Touch(void)
{
    Touch_INT = true;
}

bool TouchDrvAXS15231B::begin() {
    pinMode(TOUCH_RES, OUTPUT);
    pinMode(TOUCH_INT, INPUT_PULLUP);

    attachInterrupt(TOUCH_INT, AXS15231_Touch, FALLING);
    IIC_Bus->begin();
    
    return true;
}

bool TouchDrvAXS15231B::isPressed() {
    return Touch_INT;
}

uint8_t TouchDrvAXS15231B::getPoint(int16_t *x_array, int16_t *y_array, uint8_t get_point) {
    Touch_INT = false;
    uint8_t temp_buf[8] = {0};

    if (millis() > CycleTime)
    {
        IIC_Bus->IIC_ReadCData_Data(0x3B, read_touchpad_cmd, sizeof(read_touchpad_cmd),
                                    temp_buf, sizeof(temp_buf));

        CycleTime = millis() + 20;
    }

    uint8_t fingers_number = temp_buf[1];
    uint8_t touch_event = temp_buf[2] >> 4;
    
    if ((fingers_number == 1) && (touch_event == 0x08)) // Touch inspection and judgment
    {
        uint16_t touch_x = ((uint16_t)(temp_buf[4] & 0x0F) << 8) | (uint16_t)temp_buf[5];
        uint16_t touch_y = LCD_HEIGHT - (((uint16_t)(temp_buf[2] & 0B00001111) << 8) | (uint16_t)temp_buf[3]);

        // if (rotation) {
         // Map physical to LVGL coordinates (rotation 90° clockwise)
        int16_t lv_x = touch_y;                  // physical Y → LVGL X
        int16_t lv_y = (LCD_WIDTH - 1) - touch_x; // flipped physical X → LVGL Y
        //}

        *x_array = lv_x;
        *y_array = lv_y;
        return 1;
    }

    return 0;
}


void TouchDrvAXS15231B::sleep() {
    // No sleep mode known for this chip
}

const char *TouchDrvAXS15231B::getModelName() {
    return "AXS15231B";
}
