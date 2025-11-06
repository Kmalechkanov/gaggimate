#pragma once
#include <Wire.h>

class TouchDrvAXS15231B {
public:
    bool begin(TwoWire &wire, uint8_t address, int sda, int scl);
    bool isPressed();
    uint8_t getPoint(int16_t *x_array, int16_t *y_array, uint8_t get_point = 1);
    void sleep();
    const char *getModelName();

private:
    TwoWire *_wire = nullptr;
    uint8_t _addr = 0x3B;
    bool _pressed = false;
};
