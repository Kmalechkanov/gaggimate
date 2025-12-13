
#ifndef AXS15231_H
#define AXS15231_H

#include "Arduino_GFX_Library.h"

#define LCD_WIDTH 180
#define LCD_HEIGHT 640

class AXS15231 : public Arduino_AXS15231 {
  public:
    AXS15231(Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0, bool ips = false, int16_t w = LCD_WIDTH,
           int16_t h = LCD_HEIGHT);
    void setBrightness(int t);
  private:
    uint8_t _color_order;
};

#endif