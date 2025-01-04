/* vim: set ai et ts=4 sw=4: */
#ifndef HT_ST7735_FONTS_H
#define HT_ST7735_FONTS_H

#include <stdint.h>

typedef struct
{
    const uint8_t width;
    uint8_t height;
    const uint16_t* data;
} FontDef;

extern FontDef Font_7x10;
extern FontDef Font_11x18;
extern FontDef Font_16x26;

#endif // __HT_ST7735_FONTS_H__
