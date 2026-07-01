#pragma once

#include <helpers/CommonCLI.h>

// Applies and tracks lusofw-specific NodePrefs defaults.
//
// On every firmware version change (including the very first boot),
// reset() overwrites the lusofw-managed preference fields, then the
// caller persists them and stamps the new version. User-defined values
// for these fields are intentionally reset on version change.
class LusoDefaults {
 public:
  // Resets all lusofw-managed NodePrefs fields to their factory defaults.
  // Overwrites any user-defined values, so only call on firmware version change.
  static void applyDefaults(NodePrefs& prefs);

  // Reads the previously-recorded firmware version string from the filesystem.
  // Empty string if none recorded yet (first boot).
  static void readVersion(FILESYSTEM* fs, char* buf, size_t bufLen);

  // Records the given firmware version string to the filesystem.
  static void writeVersion(FILESYSTEM* fs, const char* version);
};
