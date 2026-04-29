#include <iostream>
#include <stdint.h>
#include <string.h>

// Mock Adafruit_GFX types
typedef struct {
    uint16_t bitmapOffset;
    uint8_t  width, height;
    uint8_t  xAdvance;
    int8_t   xOffset, yOffset;
} GFXglyph;

typedef struct {
    uint8_t  *bitmap;
    GFXglyph *glyph;
    uint16_t  first, last;
    uint8_t   yAdvance;
} GFXfont;

#include "src/Fonts/FreeMono9pt7b.h"

void getTextBounds(const GFXfont* font, const char *str, int16_t x, int16_t y,
        int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
    uint8_t c;
    int16_t minx = 0x7FFF, miny = 0x7FFF, maxx = -1, maxy = -1;
    int16_t cx = 0, cy = 0;
    while((c = *str++)) {
        if(c >= font->first && c <= font->last) {
            GFXglyph *glyph = &font->glyph[c - font->first];
            int16_t gw = glyph->width, gh = glyph->height;
            int16_t xa = glyph->xAdvance;
            int16_t xo = glyph->xOffset, yo = glyph->yOffset;
            int16_t x1_ = cx + xo;
            int16_t y1_ = cy + yo;
            int16_t x2_ = x1_ + gw - 1;
            int16_t y2_ = y1_ + gh - 1;
            if(x1_ < minx) minx = x1_;
            if(y1_ < miny) miny = y1_;
            if(x2_ > maxx) maxx = x2_;
            if(y2_ > maxy) maxy = y2_;
            cx += xa;
        }
    }
    *x1 = minx + x;
    *y1 = miny + y;
    *w  = maxx - minx + 1;
    *h  = maxy - miny + 1;
}

int main() {
    int16_t tx, ty;
    uint16_t tw, th;
    getTextBounds(&FreeMono9pt7b, "9:27 PM, 04/28/2026", 0, 0, &tx, &ty, &tw, &th);
    std::cout << "tx=" << tx << " ty=" << ty << " tw=" << tw << " th=" << th << std::endl;
    
    int16_t y = 160;
    int16_t h = 40;
    int16_t px = (200 - tw - 16) / 2;
    int16_t py = y + (h - th) / 2 + th - 2;
    
    int16_t pill_y = py - th - 4;
    int16_t pill_h = th + 8;
    
    std::cout << "py=" << py << std::endl;
    std::cout << "Pill Y=" << pill_y << " Pill H=" << pill_h << " Bottom=" << pill_y + pill_h << std::endl;
    
    return 0;
}
