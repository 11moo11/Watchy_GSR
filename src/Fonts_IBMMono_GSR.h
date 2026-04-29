#ifndef FONTS_IBMMONO_GSR_H
#define FONTS_IBMMONO_GSR_H

#include <Adafruit_GFX.h>

// IBM Plex Mono / IBM Mono in GFX format is large; use Adafruit FreeMono as the bundled monospace
// until a dedicated converted IBM Plex Mono font is added (fontconvert from .ttf).
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeMono9pt7b.h>

static const GFXfont *const GSR_IBMMono_Menu = &FreeMono12pt7b;
static const GFXfont *const GSR_IBMMono_Small = &FreeMono9pt7b;

#endif
