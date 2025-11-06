#ifndef LILYGOTDISPLAYLONGDRIVER_H
#define LILYGOTDISPLAYLONGDRIVER_H
#include "Driver.h"
#include <display/drivers/LilyGo-T-Display-S3-Long/LilyGo_TDisplayLongPanel.h>

class LillyGoTDisplayLongDriver : public Driver {
  public:
    bool isCompatible() override;
    void init() override;
    void setBrightness(int brightness) override { panel.setBrightness(brightness); };

    static LillyGoTDisplayLongDriver *getInstance() {
        if (instance == nullptr) {
            instance = new LillyGoTDisplayLongDriver();
        }
        return instance;
    };

  private:
    static LillyGoTDisplayLongDriver *instance;
    LilyGo_TDisplayLongPanel panel;

    LillyGoTDisplayLongDriver() {};
};

#endif // LILYGOTDISPLAYLONGDRIVER_H
