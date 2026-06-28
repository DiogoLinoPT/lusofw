#include "LusoDefaults.h"

#include <MeshCore.h>
#include <string.h>

void LusoDefaults::applyDefaults(NodePrefs& prefs) {
  prefs.advert_interval = 0;                  // direct adverts are legacy
  prefs.advert_loc_policy = ADVERT_LOC_PREFS; // use coordinates from prefs
  prefs.airtime_factor = 1.0f;                // reverted to 50% due to internacionalization issues
  prefs.direct_tx_delay_factor = 0.3f;        // was 0.2
  prefs.flood_advert_base = 0.308f;           // magic number
  prefs.flood_advert_interval = 23;  // defaults to 23h on lusofw, when >0 enabled our custom advert handling
  prefs.interference_threshold = 0; // disable RSSI based listen-before-talk
  prefs.cad_enabled = 1;            // enable hardware CAD listen-before-talk (set cad off to disable)
  prefs.loop_detect = LOOP_DETECT_MINIMAL; // default to minimal loop detection
  prefs.path_hash_mode = 1;                // default to 2 bytes
  prefs.rx_delay_base = 0.0f;              // turn off by default, was 10.0;
  prefs.tx_delay_factor = 0.5f;            // was 0.25f

#if defined(USE_SX1262) || defined(USE_SX1268)
#ifdef SX126X_RX_BOOSTED_GAIN
      prefs.rx_boosted_gain = SX126X_RX_BOOSTED_GAIN;
#else
      prefs.rx_boosted_gain = 1; // enabled by default;
#endif
#endif
}

void LusoDefaults::readVersion(FILESYSTEM* fs, char* buf, size_t bufLen) {
  if (!buf || bufLen == 0) {
    return;
  }

  buf[0] = 0;
  if (!fs) {
    return;
  }

#if defined(NRF52_PLATFORM) || defined(STM32_PLATFORM)
  File f = fs->open("/lusofw", FILE_O_READ);
#elif defined(RP2040_PLATFORM)
  File f = fs->open("/lusofw", "r");
#else
  File f = fs->open("/lusofw");
#endif

  if (!f) {
    return;
  }

  size_t n = f.readBytes(buf, bufLen - 1);
  buf[n] = 0;
  f.close();

  while (n > 0 && (buf[n - 1] == '\r' || buf[n - 1] == '\n' || buf[n - 1] == ' ' || buf[n - 1] == '\t')) {
    buf[--n] = 0;
  }

  MESH_DEBUG_PRINTLN("LusoDefaults: read stored version '%s'", buf);
}

void LusoDefaults::writeVersion(FILESYSTEM* fs, const char* version) {
  if (!fs || !version) {
    return;
  }

#if defined(NRF52_PLATFORM) || defined(STM32_PLATFORM)
  fs->remove("/lusofw");
  File f = fs->open("/lusofw", FILE_O_WRITE);
#elif defined(RP2040_PLATFORM)
  File f = fs->open("/lusofw", "w");
#else
  File f = fs->open("/lusofw", "w", true);
#endif

  if (f) {
    f.print(version);
    f.close();
    MESH_DEBUG_PRINTLN("LusoDefaults: wrote version '%s'", version);
  }
}
