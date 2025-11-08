#include "AXS15231.h"

AXS15231::AXS15231(Arduino_DataBus *bus, int8_t rst, uint8_t r, bool ips, int16_t w, int16_t h)
    : Arduino_AXS15231(bus, rst, r, ips, w, h)  {
}

void AXS15231::setBrightness(int t) {
    this->Display_Brightness(t);
    // ledcWrite(1, t);
}
