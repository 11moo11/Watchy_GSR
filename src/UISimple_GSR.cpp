#include "UISimple_GSR.h"
#include "Watchy_GSR.h"

static UIFooterState g_footerState;

void gsr_ui_invalidate_footer() { g_footerState.dirty = true; }

static int16_t align8(int16_t v) { return (int16_t)((v / 8) * 8); }

GSRUIModule::GSRUIModule(WatchyGSR &appRef) : app(appRef) {}

uint8_t GSRUIModule::visibleRowCount(int16_t bodyH) const {
  if (bodyH < GSRUIConfig::ROW_H) return 1;
  return (uint8_t)(bodyH / GSRUIConfig::ROW_H);
}

void GSRUIModule::drawDitherSidebar(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> &d,
                                    int16_t x, int16_t y, int16_t w, int16_t h) {
  static const uint8_t bayer4[4][4] = {
      {0, 8, 2, 10}, {12, 4, 14, 6}, {3, 11, 1, 9}, {15, 7, 13, 5}};
  for (int16_t row = 0; row < h; row++) {
    for (int16_t col = 0; col < w; col++) {
      int16_t gx = col + x;
      int16_t gy = row + y;
      uint8_t t = (uint8_t)((col * 16) / (w > 1 ? w - 1 : 1));
      uint8_t th = bayer4[gy & 3][gx & 3];
      bool blk = (th + t) < 12;
      d.drawPixel(gx, gy, blk ? GxEPD_BLACK : GxEPD_WHITE);
    }
  }
}

static void printWithTracking(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> &d, const char *text, int16_t tracking) {
  if (tracking <= 0) {
    d.print(text);
    return;
  }
  while (*text) {
    int16_t y = d.getCursorY();
    d.print(*text);
    d.setCursor(d.getCursorX() + tracking, y);
    text++;
  }
}

void GSRUIModule::drawListItem(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> &d,
                               int16_t x, int16_t y, int16_t w, int16_t h,
                               const char *text, ListItemVisual vis) {
  const int16_t dashW = 6;
  if (vis == ListItemVisual::Selected) {
    d.fillRect(x, y, w, h, GxEPD_BLACK);
    d.setFont(GSR_IBMMono_Menu);
    d.setTextColor(GxEPD_WHITE);
    d.setCursor((int16_t)(x + dashW + 2), (int16_t)(y + h - 4));
    printWithTracking(d, text, GSRUIConfig::MENU_TRACKING);
  } else {
    d.fillRect(x, y, w, h, GxEPD_WHITE);
    d.drawFastHLine(x + 1, y + h / 2, dashW - 1, GxEPD_BLACK);
    d.setFont(GSR_IBMMono_Menu);
    d.setTextColor(GxEPD_BLACK);
    d.setCursor((int16_t)(x + dashW + 2), (int16_t)(y + h - 4));
    printWithTracking(d, text, GSRUIConfig::MENU_TRACKING);
  }
}

void GSRUIModule::drawHeader(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> &d,
                             bool showBack, const char *title, uint16_t fg, uint16_t bg) {
  d.fillRect(0, 0, GSRUIConfig::SCREEN, GSRUIConfig::HEADER_H, bg);
  d.drawFastHLine(0, (int16_t)(GSRUIConfig::HEADER_H - 1), GSRUIConfig::SCREEN, GxEPD_BLACK);
  const int box = 22;
  if (showBack) {
    d.drawRect(2, 3, box, (int16_t)(GSRUIConfig::HEADER_H - 6), GxEPD_BLACK);
    d.setFont(GSR_IBMMono_Menu);
    d.setTextColor(fg);
    d.setCursor(8, 24);
    d.print("<");
  }
  d.setFont(GSR_IBMMono_Small);
  d.setTextColor(fg);
  d.setCursor(showBack ? (int16_t)(box + 8) : 4, 24);
  d.print(title);
}

void GSRUIModule::drawFooterDitherAndPill(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> &d,
                                          int16_t y, int16_t h, const char *timeDateLine, uint16_t fg, uint16_t) {
  for (int16_t row = 0; row < h; row++) {
    for (int16_t col = 0; col < GSRUIConfig::SCREEN; col++) {
      bool stip = ((col + row) & 1) == 0;
      d.drawPixel(col, y + row, stip ? GxEPD_BLACK : GxEPD_WHITE);
    }
  }
  d.setFont(GSR_IBMMono_Small);
  int16_t tx, ty;
  uint16_t tw, th;
  d.getTextBounds(timeDateLine, 0, 0, &tx, &ty, &tw, &th);
  int16_t px = (GSRUIConfig::SCREEN - (int16_t)tw - 16) / 2;
  int16_t py = (int16_t)(y + (h - (int16_t)th) / 2 + (int16_t)th - 2);
  d.fillRoundRect((int16_t)(px - 6), (int16_t)(py - (int16_t)th - 4), (int16_t)(tw + 12), (int16_t)(th + 8), 4, GxEPD_WHITE);
  d.drawRoundRect((int16_t)(px - 6), (int16_t)(py - (int16_t)th - 4), (int16_t)(tw + 12), (int16_t)(th + 8), 4, GxEPD_BLACK);
  d.setTextColor(fg);
  d.setCursor(px, py);
  d.print(timeDateLine);
}

void GSRUIModule::drawScrollbar(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> &d,
                                int16_t x, int16_t y, int16_t w, int16_t h,
                                uint8_t scrollOffset, uint8_t totalRows, uint8_t visibleRows) {
  if (totalRows <= visibleRows) return;
  for (int16_t row = 0; row < h; row++) {
    for (int16_t col = 0; col < w; col++) {
      bool noise = ((col * 3 + row * 5) & 3) != 0;
      d.drawPixel(x + col, y + row, noise ? GxEPD_BLACK : GxEPD_WHITE);
    }
  }
  uint8_t maxS = (uint8_t)(totalRows - visibleRows);
  float frac = maxS ? (float)scrollOffset / (float)maxS : 0.f;
  int16_t thumbH = (int16_t)golow(gobig((int)(h * visibleRows / totalRows), 6), (int)h);
  int16_t thumbY = y + (int16_t)((h - thumbH) * frac);
  d.fillRect(x, thumbY, w, thumbH, GxEPD_BLACK);
}

void GSRUIModule::drawMenuBodyRegion(const SimpleMenuNav &nav, const char *const *labels, uint8_t count) {
  auto &d = WatchyGSR::display;
  uint8_t vis = visibleRowCount(GSRUIConfig::BODY_H);
  bool isSubMenu = (nav.screen != SimpleUIState::MainMenu);
  int16_t sidebarWidth = isSubMenu ? GSRUIConfig::SIDEBAR_W : 0;
  int16_t listX = GSRUIConfig::BODY_PAD_X + sidebarWidth + 1;
  int16_t listW = (int16_t)(GSRUIConfig::SCREEN - listX - GSRUIConfig::SCROLLBAR_W - 2);
  int16_t bodyY = GSRUIConfig::BODY_Y;
  d.fillRect(listX, bodyY, listW, GSRUIConfig::BODY_H, GxEPD_WHITE);
  for (uint8_t r = 0; r < vis; r++) {
    uint8_t idx = (uint8_t)(nav.scrollOffset + r);
    if (idx >= count) break;
    int16_t ry = (int16_t)(bodyY + r * GSRUIConfig::ROW_H);
    ListItemVisual visu = (idx == nav.selectedIndex) ? ListItemVisual::Selected : ListItemVisual::Normal;
    drawListItem(d, listX, ry, listW, GSRUIConfig::ROW_H, labels[idx], visu);
  }
  int16_t sx = (int16_t)(GSRUIConfig::SCREEN - GSRUIConfig::SCROLLBAR_W - 1);
  drawScrollbar(d, sx, bodyY, GSRUIConfig::SCROLLBAR_W, GSRUIConfig::BODY_H, nav.scrollOffset, count, vis);
}

void GSRUIModule::drawFullMenuShell(const SimpleMenuNav &nav, const char *breadcrumb,
                                    const char *const *labels, uint8_t count) {
  auto &d = WatchyGSR::display;
  uint16_t bg = app.BackColor();
  uint16_t fg = app.ForeColor();
  d.fillScreen(bg);
  bool back = (nav.screen != SimpleUIState::MainMenu);
  drawHeader(d, back, breadcrumb, fg, bg);
  if (back) {
    drawDitherSidebar(d, GSRUIConfig::BODY_PAD_X, GSRUIConfig::BODY_Y, GSRUIConfig::SIDEBAR_W, GSRUIConfig::BODY_H);
  }
  drawMenuBodyRegion(nav, labels, count);
  drawFooterBar(true);
}

void GSRUIModule::invalidateFooter() { g_footerState.dirty = true; }

void GSRUIModule::drawFooterBar(bool forceRedraw) {
  if (!forceRedraw && !g_footerState.dirty) return;
  bool alarm = false;
  String line = app.MakeTime(WatchTime.Local.Hour, WatchTime.Local.Minute, alarm);
  line += ", ";
  char datebuf[24];
  snprintf(datebuf, sizeof(datebuf), "%02u/%02u/%04u", (unsigned)WatchTime.Local.Month, (unsigned)WatchTime.Local.Day,
           (unsigned)(WatchTime.Local.Year + 1900 + WatchyGSR::SRTC.getLocalYearOffset()));
  line += datebuf;
  auto &d = WatchyGSR::display;
  drawFooterDitherAndPill(d, (int16_t)(GSRUIConfig::SCREEN - GSRUIConfig::FOOTER_H), GSRUIConfig::FOOTER_H, line.c_str(),
                          GxEPD_BLACK, GxEPD_WHITE);
  g_footerState.dirty = false;
  g_footerState.lastMinute = WatchTime.Local.Minute;
  g_footerState.lastHour = WatchTime.Local.Hour;
  g_footerState.lastDay = WatchTime.Local.Day;
  g_footerState.lastMonth = WatchTime.Local.Month;
  g_footerState.lastYear = (uint16_t)(WatchTime.Local.Year + 1900);
}

void GSRUIModule::refreshMenuBodyPartial(const SimpleMenuNav &nav, const char *const *labels, uint8_t count) {
  gsr_ui_invalidate_footer(); // Force footer to redraw to extend the e-paper partial update bounding box to the bottom
  int16_t y = align8(GSRUIConfig::BODY_Y);
  int16_t yEnd = (int16_t)(GSRUIConfig::BODY_Y + GSRUIConfig::BODY_H);
  int16_t h = (int16_t)(align8((int16_t)(yEnd + 7)) - y);
  auto &d = WatchyGSR::display;
  
  d.fillRect(0, y, GSRUIConfig::SCREEN, h, GxEPD_WHITE);
  bool back = (nav.screen != SimpleUIState::MainMenu);
  if (back) {
    drawDitherSidebar(d, GSRUIConfig::BODY_PAD_X, GSRUIConfig::BODY_Y, GSRUIConfig::SIDEBAR_W, GSRUIConfig::BODY_H);
  }
  drawMenuBodyRegion(nav, labels, count);
}
