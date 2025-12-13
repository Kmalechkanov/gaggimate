#pragma once
#include <Wire.h>

class TouchDrvAXS15231B {
public:
    bool begin();
    bool isPressed();
    uint8_t getPoint(int16_t *x_array, int16_t *y_array, uint8_t get_point = 1);
    void sleep();
    const char *getModelName();

private:
};
