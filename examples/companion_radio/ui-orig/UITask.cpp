#include "UITask.h"
#include <Arduino.h>
#include <helpers/TxtDataHelpers.h>
#include "../MyMesh.h"

#define AUTO_OFF_MILLIS     15000   // 15 seconds
#define BOOT_SCREEN_MILLIS   5000   // 5 seconds

#ifdef PIN_STATUS_LED
#define LED_ON_MILLIS     20
#define LED_ON_MSG_MILLIS 200
#define LED_CYCLE_MILLIS  4000
#endif

#ifndef USER_BTN_PRESSED
#define USER_BTN_PRESSED LOW
#endif

// 'meshcore', 128x64px
static const uint8_t meshcore_logo [] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xaa, 0xa8, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x55, 0x50, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xaa, 0xa0, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x55, 0x40, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xaa, 0x80, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x55, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2a, 0xaa, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x54, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xaa, 0xa9, 0x40, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x55, 0x52, 0x80, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x0f, 0x00, 0xf1, 0xff, 0xe1, 0xff, 0x8f, 0x07, 0x07, 0xfc, 0x3f, 0xe0, 0x7f, 0xf0, 0x7f, 0xf8, 
    0x0f, 0x01, 0xf1, 0xff, 0xe7, 0xff, 0x8f, 0x07, 0x0f, 0xfc, 0x7f, 0xf8, 0x7f, 0xf8, 0x7f, 0xf8, 
    0x0f, 0x81, 0xf1, 0xff, 0xe7, 0xff, 0x8f, 0x0f, 0x1f, 0xfc, 0xff, 0xf8, 0x7f, 0xfc, 0x7f, 0xf8, 
    0x0f, 0x83, 0xe1, 0xff, 0xcf, 0xff, 0x0e, 0x0f, 0x3f, 0xf9, 0xff, 0xf8, 0x7f, 0xfc, 0x7f, 0xf0, 
    0x1f, 0x87, 0xe3, 0xc0, 0x0f, 0x00, 0x1e, 0x0f, 0x3c, 0x01, 0xe0, 0x78, 0xf0, 0x38, 0xf0, 0x00, 
    0x1f, 0x87, 0xe3, 0xff, 0x8f, 0xfe, 0x1f, 0xff, 0x3c, 0x01, 0xe0, 0x78, 0xf0, 0x78, 0xff, 0xe0, 
    0x1f, 0xcf, 0xe3, 0xff, 0x0f, 0xff, 0x1f, 0xff, 0x3c, 0x01, 0xe0, 0x78, 0xf0, 0x78, 0xff, 0xc0, 
    0x1f, 0xdf, 0xe3, 0xff, 0x07, 0xff, 0x1f, 0xfe, 0x38, 0x01, 0xc0, 0x78, 0xff, 0xf8, 0xff, 0xc0, 
    0x1d, 0xdf, 0xe3, 0x80, 0x00, 0x07, 0x1c, 0x1e, 0x78, 0x01, 0xc0, 0x70, 0xff, 0xf0, 0xe0, 0x00, 
    0x1d, 0xfd, 0xc7, 0x80, 0x00, 0x07, 0x3c, 0x1e, 0x78, 0x03, 0xc0, 0xf1, 0xff, 0xe1, 0xe0, 0x00, 
    0x1d, 0xf9, 0xc7, 0xff, 0x9f, 0xff, 0x3c, 0x1e, 0x7f, 0xf3, 0xff, 0xf1, 0xe1, 0xe1, 0xff, 0xe0, 
    0x1d, 0xfb, 0xc7, 0xff, 0x9f, 0xfe, 0x3c, 0x1e, 0x7f, 0xf3, 0xff, 0xe1, 0xe1, 0xf1, 0xff, 0xe0, 
    0x18, 0xf3, 0xc7, 0xff, 0x9f, 0xfc, 0x3c, 0x1c, 0x3f, 0xf1, 0xff, 0xc1, 0xe0, 0xf1, 0xff, 0xe0, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x29, 0x55, 0x50, 0x00, 
    0x00, 0x00, 0x0f, 0xe0, 0x7e, 0x07, 0xf0, 0x7f, 0x0c, 0x60, 0x7e, 0x03, 0x92, 0xea, 0xa0, 0x00, 
    0x00, 0x00, 0x0f, 0xe0, 0xff, 0x07, 0xf0, 0xfe, 0x0c, 0x60, 0xfe, 0x03, 0xa5, 0xd5, 0x40, 0x00, 
    0x00, 0x00, 0x0c, 0x60, 0xc6, 0x06, 0x30, 0x18, 0x0c, 0x60, 0xc0, 0x07, 0xca, 0xea, 0x80, 0x00, 
    0x00, 0x00, 0x0f, 0xe1, 0xc6, 0x07, 0xf0, 0x38, 0x1c, 0x61, 0xde, 0x06, 0xd5, 0xd5, 0x00, 0x00, 
    0x00, 0x00, 0x1f, 0xc1, 0x86, 0x0f, 0xe0, 0x30, 0x18, 0x61, 0x86, 0x0c, 0xeb, 0xaa, 0x00, 0x00, 
    0x00, 0x00, 0x18, 0x01, 0xfe, 0x0c, 0xe0, 0x30, 0x1f, 0xc1, 0xfc, 0x1f, 0xf5, 0xfc, 0x00, 0x00, 
    0x00, 0x00, 0x18, 0x01, 0xfc, 0x0c, 0x60, 0x30, 0x0f, 0xc1, 0xfc, 0x18, 0xeb, 0xf8, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x55, 0x50, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xaa, 0xa0, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x55, 0x40, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xaa, 0x80, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

void UITask::begin(DisplayDriver* display, SensorManager* sensors, NodePrefs* node_prefs) {
  _display = display;
  _sensors = sensors;
  _auto_off = millis() + AUTO_OFF_MILLIS;
  clearMsgPreview();
  _node_prefs = node_prefs;
  if (_display != NULL) {
    _display->turnOn();
  }

  // strip off dash and commit hash by changing dash to null terminator
  // e.g: v1.2.3-abcdef -> v1.2.3
  char *version = strdup(FIRMWARE_VERSION);
  char *dash = strchr(version, '-');
  if (dash) {
    *dash = 0;
  }

  // v1.2.3 (1 Jan 2025)
  sprintf(_version_info, "%s (%s)", version, FIRMWARE_BUILD_DATE);

#ifdef PIN_BUZZER
  buzzer.begin();
  buzzer.quiet(_node_prefs->buzzer_quiet);
  buzzer.startup();
#endif

  // Initialize digital button if available
#ifdef PIN_USER_BTN
  _userButton = new Button(PIN_USER_BTN, USER_BTN_PRESSED);
  _userButton->begin();
  
  // Set up digital button callbacks
  _userButton->onShortPress([this]() { handleButtonShortPress(); });
  _userButton->onDoublePress([this]() { handleButtonDoublePress(); });
  _userButton->onTriplePress([this]() { handleButtonTriplePress(); });
  _userButton->onQuadruplePress([this]() { handleButtonQuadruplePress(); });
  _userButton->onLongPress([this]() { handleButtonLongPress(); });
  _userButton->onAnyPress([this]() { handleButtonAnyPress(); });
#endif

  // Initialize analog button if available
#ifdef PIN_USER_BTN_ANA
  _userButtonAnalog = new Button(PIN_USER_BTN_ANA, USER_BTN_PRESSED, true, 20);
  _userButtonAnalog->begin();
  
  // Set up analog button callbacks
  _userButtonAnalog->onShortPress([this]() { handleButtonShortPress(); });
  _userButtonAnalog->onDoublePress([this]() { handleButtonDoublePress(); });
  _userButtonAnalog->onTriplePress([this]() { handleButtonTriplePress(); });
  _userButtonAnalog->onQuadruplePress([this]() { handleButtonQuadruplePress(); });
  _userButtonAnalog->onLongPress([this]() { handleButtonLongPress(); });
  _userButtonAnalog->onAnyPress([this]() { handleButtonAnyPress(); });
#endif
  ui_started_at = millis();
}

void UITask::notify(UIEventType t) {
#if defined(PIN_BUZZER)
switch(t){
  case UIEventType::contactMessage:
    // gemini's pick
    buzzer.play("MsgRcv3:d=4,o=6,b=200:32e,32g,32b,16c7");
    break;
  case UIEventType::channelMessage:
    buzzer.play("kerplop:d=16,o=6,b=120:32g#,32c#");
    break;
  case UIEventType::ack:
    buzzer.play("ack:d=32,o=8,b=120:c");
    break;
  case UIEventType::roomMessage:
  case UIEventType::newContactMessage:
  case UIEventType::none:
  default:
    break;
}
#endif
//  Serial.print("DBG:  Alert user -> ");
//  Serial.println((int) t);
}

void UITask::msgRead(int msgcount) {
  _msgcount = msgcount;
  if (msgcount == 0) {
    clearMsgPreview();
  }
}

void UITask::clearMsgPreview() {
  _origin[0] = 0;
  _msg[0] = 0;
  _need_refresh = true;
}

void UITask::newMsg(uint8_t path_len, const char* from_name, const char* text, int msgcount) {
  _msgcount = msgcount;

  if (path_len == 0xFF) {
    sprintf(_origin, "(F) %s", from_name);
  } else {
    sprintf(_origin, "(%d) %s", (uint32_t) path_len, from_name);
  }
  StrHelper::strncpy(_msg, text, sizeof(_msg));

  if (_display != NULL) {
    if (!_display->isOn() && !hasConnection()) {
      _display->turnOn();
    }
    if (_display->isOn()) {
    _auto_off = millis() + AUTO_OFF_MILLIS;  // extend the auto-off timer
    _need_refresh = true;
    }
  }
}

// LiPo discharge curve: per-cell open-circuit voltage (mV) -> remaining capacity (%).
// Approximates a standard 1S LiPo at a low discharge rate; linearly interpolated
// between points. The non-linear S-shape (steep at the top, a long flat plateau
// through the mid-range, steep drop near empty) tracks real cell behaviour far
// better than a straight (v - min) / (max - min) line.
static const struct { uint16_t milliVolts; uint8_t percent; } lipo_discharge_curve[] = {
  { 4200, 100 }, { 4150, 95 }, { 4100, 90 }, { 4050, 85 }, { 4000, 80 },
  { 3950, 74 }, { 3900, 68 }, { 3850, 60 }, { 3800, 50 }, { 3750, 38 },
  { 3700, 28 }, { 3650, 18 }, { 3600, 10 }, { 3500,  5 }, { 3300,  0 },
};

// Maps a battery voltage to a state-of-charge percentage using the curve above.
// The number of cells in series is derived from BATT_MAX_MILLIVOLTS, so the same
// per-cell curve works for 1S (4.2 V) and 2S (8.4 V) packs. Result clamped to [0, 100].
static int lipoPercentFromMilliVolts(uint16_t batteryMilliVolts) {
#ifndef BATT_MIN_MILLIVOLTS
  #define BATT_MIN_MILLIVOLTS 3000
#endif
#ifndef BATT_MAX_MILLIVOLTS
  #define BATT_MAX_MILLIVOLTS 4200
#endif
  const int numCells = (BATT_MAX_MILLIVOLTS + 2099) / 4200;   // round(4.2 V per cell)
  const uint16_t cellMilliVolts = batteryMilliVolts / numCells;

  const int n = sizeof(lipo_discharge_curve) / sizeof(lipo_discharge_curve[0]);
  if (cellMilliVolts >= lipo_discharge_curve[0].milliVolts) return 100;
  if (cellMilliVolts <= lipo_discharge_curve[n - 1].milliVolts) return 0;

  for (int i = 0; i < n - 1; i++) {
    const uint16_t vHi = lipo_discharge_curve[i].milliVolts;
    const uint16_t vLo = lipo_discharge_curve[i + 1].milliVolts;
    if (cellMilliVolts <= vHi && cellMilliVolts >= vLo) {
      const int pHi = lipo_discharge_curve[i].percent;
      const int pLo = lipo_discharge_curve[i + 1].percent;
      return pLo + (int)(cellMilliVolts - vLo) * (pHi - pLo) / (int)(vHi - vLo);
    }
  }
  return 0;
}

void UITask::renderBatteryIndicator(uint16_t batteryMilliVolts) {
  int batteryPercentage = lipoPercentFromMilliVolts(batteryMilliVolts);

  // battery icon
  int iconWidth = 24;
  int iconHeight = 12;
  int iconX = _display->width() - iconWidth - 5; // Position the icon near the top-right corner
  int iconY = 0;
  _display->setColor(DisplayDriver::GREEN);

  // battery outline
  _display->drawRect(iconX, iconY, iconWidth, iconHeight);

  // battery "cap"
  _display->fillRect(iconX + iconWidth, iconY + (iconHeight / 4), 3, iconHeight / 2);

  // fill the battery based on the percentage
  int fillWidth = (batteryPercentage * (iconWidth - 4)) / 100;
  _display->fillRect(iconX + 2, iconY + 2, fillWidth, iconHeight - 4);
}

void UITask::renderCurrScreen() {
  if (_display == NULL) return;  // assert() ??

  char tmp[80];
  if (_alert[0]) {
    _display->setTextSize(1.4);
    uint16_t textWidth = _display->getTextWidth(_alert);
    _display->setCursor((_display->width() - textWidth) / 2, 22);
    _display->setColor(DisplayDriver::GREEN);
    _display->print(_alert);
    _alert[0] = 0;
    _need_refresh = true;
    return;
  } else if (_origin[0] && _msg[0]) { // message preview
    // render message preview
    _display->setCursor(0, 0);
    _display->setTextSize(1);
    _display->setColor(DisplayDriver::GREEN);
    _display->print(_node_prefs->node_name);

    _display->setCursor(0, 12);
    _display->setColor(DisplayDriver::YELLOW);
    _display->print(_origin);
    _display->setCursor(0, 24);
    _display->setColor(DisplayDriver::LIGHT);
    _display->print(_msg);

    _display->setCursor(_display->width() - 28, 9);
    _display->setTextSize(2);
    _display->setColor(DisplayDriver::ORANGE);
    sprintf(tmp, "%d", _msgcount);
    _display->print(tmp);
    _display->setColor(DisplayDriver::YELLOW); // last color will be kept on T114
  } else if ((millis() - ui_started_at) < BOOT_SCREEN_MILLIS) { // boot screen
    // meshcore logo (128x64, centered on the display)
    int logoX = (_display->width() - 128) / 2;
    int logoY = (_display->height() - 64) / 2;
    _display->setColor(DisplayDriver::BLUE);
    _display->drawXbm(logoX, logoY, meshcore_logo, 128, 64);

    // meshcore website (overlaid in the bottom empty band of the logo)
    const char* website = "https://meshcore.pt";
    _display->setColor(DisplayDriver::LIGHT);
    _display->setTextSize(1);
    uint16_t websiteWidth = _display->getTextWidth(website);
    _display->setCursor((_display->width() - websiteWidth) / 2, logoY + 44);
    _display->print(website);

    // version info
    uint16_t textWidth = _display->getTextWidth(_version_info);
    _display->setCursor((_display->width() - textWidth) / 2, logoY + 54);
    _display->print(_version_info);
  } else {  // home screen
    // node name
    _display->setCursor(0, 0);
    _display->setTextSize(1);
    _display->setColor(DisplayDriver::GREEN);
    _display->print(_node_prefs->node_name);

    // battery voltage
    renderBatteryIndicator(_board->getBattMilliVolts());

    // freq / sf
    _display->setCursor(0, 20);
    _display->setColor(DisplayDriver::YELLOW);
    sprintf(tmp, "FREQ: %06.3f SF%d", _node_prefs->freq, _node_prefs->sf);
    _display->print(tmp);

    // bw / cr
    _display->setCursor(0, 30);
    sprintf(tmp, "BW: %03.2f CR: %d", _node_prefs->bw, _node_prefs->cr);
    _display->print(tmp);

    // BT pin
    if (!_connected && the_mesh.getBLEPin() != 0) {
      _display->setColor(DisplayDriver::RED);
      _display->setTextSize(2);
      _display->setCursor(0, 43);
      sprintf(tmp, "Pin:%d", the_mesh.getBLEPin());
      _display->print(tmp);
      _display->setColor(DisplayDriver::GREEN);
    } else {
      _display->setColor(DisplayDriver::LIGHT); 
    }
  }
  _need_refresh = false;
}

void UITask::userLedHandler() {
#ifdef PIN_STATUS_LED
  static int state = 0;
  static int next_change = 0;
  static int last_increment = 0;

  int cur_time = millis();
  if (cur_time > next_change) {
    if (state == 0) {
      state = 1;
      if (_msgcount > 0) {
        last_increment = LED_ON_MSG_MILLIS;
      } else {
        last_increment = LED_ON_MILLIS;
      }
      next_change = cur_time + last_increment;
    } else {
      state = 0;
      next_change = cur_time + LED_CYCLE_MILLIS - last_increment;
    }
    digitalWrite(PIN_STATUS_LED, state == LED_STATE_ON);
  }
#endif
}

/* 
  hardware-agnostic pre-shutdown activity should be done here 
*/
void UITask::shutdown(bool restart){

  #ifdef PIN_BUZZER
  /* note: we have a choice here -
     we can do a blocking buzzer.loop() with non-deterministic consequences
     or we can set a flag and delay the shutdown for a couple of seconds
     while a non-blocking buzzer.loop() plays out in UITask::loop()
  */
  buzzer.shutdown();
  uint32_t buzzer_timer = millis(); // fail-safe shutdown
  while (buzzer.isPlaying() && (millis() - 2500) < buzzer_timer)
    buzzer.loop();

  #endif // PIN_BUZZER

  if (restart) {
    _board->reboot();
  } else {
    radio_driver.powerOff();
#if ENV_INCLUDE_GPS == 1
    // power down GPS before SYSTEMOFF; some boards' powerOff() only pulls EN low
    if (_sensors) {
      _sensors->setSettingValue("gps", "0");
    }
#endif
    _board->powerOff();
  }
}

void UITask::loop() {
  #ifdef PIN_USER_BTN
    if (_userButton) {
      _userButton->update();
    }
  #endif
  #ifdef PIN_USER_BTN_ANA
    if (_userButtonAnalog) {
      _userButtonAnalog->update();
    }
  #endif
  userLedHandler();

#ifdef PIN_BUZZER
  if (buzzer.isPlaying())  buzzer.loop();
#endif

  if (_display != NULL && _display->isOn()) {
    static bool _firstBoot = true;
    if(_firstBoot && (millis() - ui_started_at) >= BOOT_SCREEN_MILLIS) {
      _need_refresh = true;
      _firstBoot = false;
    }
    if (millis() >= _next_refresh && _need_refresh) {
      _display->startFrame();
      renderCurrScreen();
      _display->endFrame();

      _next_refresh = millis() + 1000;   // refresh every second
    }
#ifdef KEEP_DISPLAY_ON_USB
    // Opt-in: refresh the auto-off deadline while externally powered, so the
    // timer counts from the moment external power is removed. Off by default
    // because OLED panels burn in quickly; only enable for LCD targets or
    // where the display is replaceable.
    if (board.isExternalPowered()) {
      _auto_off = millis() + AUTO_OFF_MILLIS;
    }
#endif
    if (millis() > _auto_off) {
      _display->turnOff();
    }
  }
}

void UITask::handleButtonAnyPress() {
  MESH_DEBUG_PRINTLN("UITask: any press triggered");
  // called on any button press before other events, to wake up the display quickly
  // do not refresh the display here, as it may block the button handler
  if (_display != NULL) {
    _displayWasOn = _display->isOn();  // Track display state before any action
    if (!_displayWasOn) {
      _display->turnOn();
    }
    _auto_off = millis() + AUTO_OFF_MILLIS;   // extend auto-off timer
  }
}

void UITask::handleButtonShortPress() {
  MESH_DEBUG_PRINTLN("UITask: short press triggered");
  if (_display != NULL) {
    // Only clear message preview if display was already on before button press
    if (_displayWasOn) {
      // If display was on and showing message preview, clear it
      if (_origin[0] && _msg[0]) {
        clearMsgPreview();
      } else {
        // Otherwise, refresh the display
        _need_refresh = true;
      }
    } else {
      _need_refresh = true; // display just turned on, so we need to refresh
    }
    // Note: Display turn-on and auto-off timer extension are handled by handleButtonAnyPress
  }
}

void UITask::handleButtonDoublePress() {
  MESH_DEBUG_PRINTLN("UITask: double press triggered, sending advert");
  // ADVERT
  #ifdef PIN_BUZZER
      notify(UIEventType::ack);
  #endif
  if (the_mesh.advert()) {
    MESH_DEBUG_PRINTLN("Advert sent!");
    sprintf(_alert, "Advert sent!");
  } else {
    MESH_DEBUG_PRINTLN("Advert failed!");
    sprintf(_alert, "Advert failed..");
  }
  _need_refresh = true;
}

void UITask::handleButtonTriplePress() {
  MESH_DEBUG_PRINTLN("UITask: triple press triggered");
  // Toggle buzzer quiet mode
  #ifdef PIN_BUZZER
    if (buzzer.isQuiet()) {
      buzzer.quiet(false);
      notify(UIEventType::ack);
      sprintf(_alert, "Buzzer: ON");
    } else {
      buzzer.quiet(true);
      sprintf(_alert, "Buzzer: OFF");
    }
    _node_prefs->buzzer_quiet = buzzer.isQuiet();
    the_mesh.savePrefs();
    _need_refresh = true;
  #endif
}

void UITask::handleButtonQuadruplePress() {
  MESH_DEBUG_PRINTLN("UITask: quad press triggered");
  if (_sensors != NULL) {
    // toggle GPS onn/off
    int num = _sensors->getNumSettings();
    for (int i = 0; i < num; i++) {
      if (strcmp(_sensors->getSettingName(i), "gps") == 0) {
        if (strcmp(_sensors->getSettingValue(i), "1") == 0) {
          _sensors->setSettingValue("gps", "0");
          notify(UIEventType::ack);
          sprintf(_alert, "GPS: Disabled");
        } else {
          _sensors->setSettingValue("gps", "1");
          notify(UIEventType::ack);
          sprintf(_alert, "GPS: Enabled");
        }
        break;
      }
    }
  }
  _need_refresh = true;
}

void UITask::handleButtonLongPress() {
  MESH_DEBUG_PRINTLN("UITask: long press triggered");
  if (millis() - ui_started_at < 8000) {   // long press in first 8 seconds since startup -> CLI/rescue
    the_mesh.enterCLIRescue();
  } else {
    shutdown();
  }
}