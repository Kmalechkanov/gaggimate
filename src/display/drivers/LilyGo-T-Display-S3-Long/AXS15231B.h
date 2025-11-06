#pragma once

#include "stdint.h"
//#include "pins_config.h"
#include "lvgl.h"/* https://github.com/lvgl/lvgl.git */

#define LCD_SPI_DMA 
#define AX15231B

#define TFT_MADCTL 0x36
#define TFT_MAD_MY 0x80
#define TFT_MAD_MX 0x40
#define TFT_MAD_MV 0x20
#define TFT_MAD_ML 0x10
#define TFT_MAD_BGR 0x08
#define TFT_MAD_MH 0x04
#define TFT_MAD_RGB 0x00

#define TFT_INVOFF 0x20
#define TFT_INVON 0x21

#define TFT_SCK_H digitalWrite(TFT_SCK, 1);
#define TFT_SCK_L digitalWrite(TFT_SCK, 0);
#define TFT_SDA_H digitalWrite(TFT_MOSI, 1);
#define TFT_SDA_L digitalWrite(TFT_MOSI, 0);

#define TFT_RES_H digitalWrite(TFT_QSPI_RST, 1);
#define TFT_RES_L digitalWrite(TFT_QSPI_RST, 0);
#define TFT_DC_H digitalWrite(TFT_DC, 1);
#define TFT_DC_L digitalWrite(TFT_DC, 0);
#define TFT_CS_H digitalWrite(TFT_QSPI_CS, 1);
#define TFT_CS_L digitalWrite(TFT_QSPI_CS, 0);

#define LCD_USB_QSPI_DREVER   1

#define SPI_FREQUENCY           32000000
#define TFT_SPI_MODE          SPI_MODE0
#define TFT_SPI_HOST          SPI2_HOST

#define SEND_BUF_SIZE         (28800/2) //16bit(RGB565)

#define TFT_QSPI_CS           12
#define TFT_QSPI_SCK          17
#define TFT_QSPI_D0           13
#define TFT_QSPI_D1           18
#define TFT_QSPI_D2           21
#define TFT_QSPI_D3           14
#define TFT_QSPI_RST          16
#define TFT_BL                1

// #define TFT_WIDTH             640
// #define TFT_HEIGHT            180

#ifdef TFT_WIDTH
#define EXAMPLE_LCD_H_RES     TFT_WIDTH
#else
#define EXAMPLE_LCD_H_RES     180
#endif
#ifdef TFT_HEIGHT
#define EXAMPLE_LCD_V_RES     TFT_HEIGHT
#else
#define EXAMPLE_LCD_V_RES     640
#endif
#define LVGL_LCD_BUF_SIZE     (EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES)

typedef struct
{
    uint8_t cmd;
    uint8_t data[36];
    uint8_t len;
} lcd_cmd_t;

void axs15231_init(void);

// Set the display window size
void lcd_address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void lcd_setRotation(uint8_t r);
void lcd_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
void lcd_fill(uint16_t xsta,
              uint16_t ysta,
              uint16_t xend,
              uint16_t yend,
              uint16_t color);
void lcd_PushColors(uint16_t x,
                    uint16_t y,
                    uint16_t width,
                    uint16_t high,
                    uint16_t *data);
void lcd_PushColors(uint16_t *data, uint32_t len);
void lcd_sleep();

bool get_lcd_spi_dma_write(void);
