#ifndef UI_SIMPLE_GSR_H
#define UI_SIMPLE_GSR_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <GxEPD2_BW.h>
#include <epd/GxEPD2_154_D67.h>
#include "Defines_GSR.h"
#include "Fonts_IBMMono_GSR.h"

namespace GSRUILayout {
constexpr int16_t SCREEN = 200;
constexpr int16_t HEADER_H = 28;
constexpr int16_t FOOTER_H = 40;
constexpr int16_t BODY_Y = HEADER_H;
constexpr int16_t BODY_H = SCREEN - HEADER_H - FOOTER_H;
constexpr int16_t SIDEBAR_W = 14;
constexpr int16_t SCROLLBAR_W = 5;
constexpr int16_t ROW_H = 18;
constexpr int16_t BODY_PAD_X = 2;
constexpr uint8_t PARTIAL_STEPS_BEFORE_FULL = 14;
}

enum class SimpleUIState : uint8_t {
  WatchFace = 0,
  MainMenu = 1,
  Options = 2,
  SystemInfo = 3
};

enum class ListItemVisual : uint8_t {
  Normal = 0,
  Selected = 1
};

struct SimpleMenuNav {
  SimpleUIState screen;
  uint8_t selectedIndex;
  uint8_t scrollOffset;
  uint8_t partialStepsSinceFull;
  bool legacyMode;
  bool partialNextDraw;
  uint8_t lastSelectedIndex;
};

struct UIFooterState {
  bool dirty;
  uint8_t lastMinute;
  uint8_t lastHour;
  uint8_t lastDay;
  uint8_t lastMonth;
  uint16_t lastYear;
};

class WatchyGSR;

class GSRUIModule {
public:
  explicit GSRUIModule(WatchyGSR &app);

  void drawFullMenuShell(const SimpleMenuNav &nav, const char *breadcrumb, const char *const *labels, uint8_t count);
  void drawMenuBodyRegion(const SimpleMenuNav &nav, const char *const *labels, uint8_t count);
  void drawFooterBar(bool forceRedraw);
  void invalidateFooter();
  void refreshMenuBodyPartial(const SimpleMenuNav &nav, const char *const *labels, uint8_t count);

  static void drawDitherSidebar(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> &d,
                                int16_t x, int16_t y, int16_t w, int16_t h);
  static void drawListItem(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> &d,
                            int16_t x, int16_t y, int16_t w, int16_t h,
                            const char *text, ListItemVisual vis);
  static void drawHeader(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> &d,
                         bool showBack, const char *title, uint16_t fg, uint16_t bg);
  static void drawFooterDitherAndPill(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> &d,
                                       int16_t y, int16_t h, const char *timeDateLine, uint16_t fg, uint16_t bg);

private:
  WatchyGSR &app;
  uint8_t visibleRowCount(int16_t bodyH) const;
  void drawScrollbar(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> &d,
                     int16_t x, int16_t y, int16_t w, int16_t h,
                     uint8_t scrollOffset, uint8_t totalRows, uint8_t visibleRows);
};

void gsr_ui_invalidate_footer();

#endif
