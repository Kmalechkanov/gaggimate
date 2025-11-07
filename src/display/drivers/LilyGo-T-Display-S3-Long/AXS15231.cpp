#include "AXS15231.h"

AXS15231::AXS15231(Arduino_DataBus *bus, int8_t rst, uint8_t r, bool ips, int16_t w, int16_t h)
    : Arduino_AXS15231(bus, rst, r, ips, w, h)  {
}

void AXS15231::setRotation(uint8_t r) {
    Arduino_TFT::setRotation(r);
    switch(r) {
    case 1:
        r = 0x60;
        break;
    case 2:
        r =  0xC0;
        break;
    case 3:
        r = 0xA0;
        break;
    default: // case 0:
        r = 0x00;
        break;
    }
    _bus->beginWrite();
    _bus->writeC8D8(CO5300_W_MADCTL, r);
    _bus->endWrite();
}

void AXS15231::setBrightness(int t) {
    // ledcAttachPin(1, 1);
    // ledcSetup(1, 2000, 8);
    // ledcWrite(1, 0);
    
    ledcWrite(1, t);
}
