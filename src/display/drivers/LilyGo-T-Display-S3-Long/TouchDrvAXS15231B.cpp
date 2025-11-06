#include "TouchDrvAXS15231B.h"
#include "pin_config.h"
#include "Arduino.h"

#define AXS_TOUCH_TWO_POINT_LEN             14  // (AXS_TOUCH_ONE_POINT_LEN *  2) +  2  Bytes
#define AXS_TOUCH_ONE_POINT_LEN             6
#define AXS_TOUCH_BUF_HEAD_LEN              2

#define AXS_TOUCH_GESTURE_POS               0
#define AXS_TOUCH_POINT_NUM                 1
#define AXS_TOUCH_EVENT_POS                 2
#define AXS_TOUCH_X_H_POS                   2
#define AXS_TOUCH_X_L_POS                   3
#define AXS_TOUCH_ID_POS                    4
#define AXS_TOUCH_Y_H_POS                   4
#define AXS_TOUCH_Y_L_POS                   5
#define AXS_TOUCH_WEIGHT_POS                6
#define AXS_TOUCH_AREA_POS 

#define AXS_GET_POINT_NUM(buf) buf[AXS_TOUCH_POINT_NUM]
#define AXS_GET_GESTURE_TYPE(buf)  buf[AXS_TOUCH_GESTURE_POS]
#define AXS_GET_POINT_X(buf,point_index) (((uint16_t)(buf[AXS_TOUCH_ONE_POINT_LEN*point_index+AXS_TOUCH_X_H_POS] & 0x0F) <<8) + (uint16_t)buf[AXS_TOUCH_ONE_POINT_LEN*point_index+AXS_TOUCH_X_L_POS])
#define AXS_GET_POINT_Y(buf,point_index) (((uint16_t)(buf[AXS_TOUCH_ONE_POINT_LEN*point_index+AXS_TOUCH_Y_H_POS] & 0x0F) <<8) + (uint16_t)buf[AXS_TOUCH_ONE_POINT_LEN*point_index+AXS_TOUCH_Y_L_POS])
#define AXS_GET_POINT_EVENT(buf,point_index) (buf[AXS_TOUCH_ONE_POINT_LEN*point_index+AXS_TOUCH_EVENT_POS] >> 6)

bool TouchDrvAXS15231B::begin(TwoWire &wire, uint8_t address, int sda, int scl) {
    _wire = &wire;
    _addr = address;

    pinMode(TOUCH_RES, OUTPUT); //todo move touch_res to to be from outside
    digitalWrite(TOUCH_RES, HIGH); delay(2);
    digitalWrite(TOUCH_RES, LOW); delay(10);
    digitalWrite(TOUCH_RES, HIGH); delay(2);

    pinMode(TOUCH_INT, INPUT_PULLUP);

    _wire->begin(sda, scl);
    _wire->beginTransmission(_addr);
    if (_wire->endTransmission() == 0) {
        return true; // found
    }
    return false;
}

bool TouchDrvAXS15231B::isPressed() {
    static bool lastState = false;
    bool current = (digitalRead(TOUCH_INT) == LOW);

    if (current != lastState) {
        delay(2); // debounce
        current = (digitalRead(TOUCH_INT) == LOW);
    }

    lastState = current;
    return current;
}

uint8_t read_touchpad_cmd[11] = {0xb5, 0xab, 0xa5, 0x5a, 0x0, 0x0, 0x0, 0x8};
uint8_t TouchDrvAXS15231B::getPoint(int16_t *x_array, int16_t *y_array, uint8_t get_point) {
    uint8_t buff[20] = {0};

    _wire->beginTransmission(_addr);
    _wire->write(read_touchpad_cmd, 8);
    _wire->endTransmission();
    _wire->requestFrom(_addr, AXS_TOUCH_TWO_POINT_LEN);
    uint32_t start = millis();
    while (_wire->available() < AXS_TOUCH_TWO_POINT_LEN) {
        if (millis() - start > 50) return false;  // timeout
    }
    _wire->readBytes(buff, AXS_TOUCH_TWO_POINT_LEN);

    uint16_t pointX;
    uint16_t pointY;
    uint16_t type = 0;

    type = AXS_GET_GESTURE_TYPE(buff);
    pointX = AXS_GET_POINT_X(buff,0);
    pointY = AXS_GET_POINT_Y(buff,0);

    if (type != 0 || pointX != 0 || pointY != 0) {
        ESP_LOGV("Touch", "Registering listener: type %d x %d y %d", type, pointX, pointY);
    }

    if (!type && (pointX || pointY)) {
        printf("read touch\n");

        if (pointX < 2 && pointY < 2) {
            printf("ghost touch\n");
            return 0;
        }

        pointX = TFT_HEIGHT - pointX; // 640 - pointX
        if (pointX >= TFT_HEIGHT) pointX = TFT_HEIGHT - 1;
        if (pointY >= TFT_WIDTH)  pointY = TFT_WIDTH - 1; 

        *x_array = pointY; // might swap later
        *y_array = pointX;
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
