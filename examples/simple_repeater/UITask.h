#pragma once

#include <helpers/ui/DisplayDriver.h>
#include <helpers/CommonCLI.h>

class UITask {
  DisplayDriver* _display;
  unsigned long _next_read, _next_refresh, _auto_off, _next_batt_chck;
  uint16_t _cached_batt_mv;
  int _prevBtnState;
  NodePrefs* _node_prefs;
  char _version_info[32];

  void renderCurrScreen();
  void renderBatteryPercent();
public:
  UITask(DisplayDriver& display) : _display(&display) { _next_read = _next_refresh = _next_batt_chck = 0; _cached_batt_mv = 0; }
  void begin(NodePrefs* node_prefs, const char* build_date, const char* firmware_version);

  void loop();
};