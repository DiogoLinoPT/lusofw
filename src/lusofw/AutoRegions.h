#pragma once

#include <helpers/CommonCLI.h>

// Region geometry primitives. Consumed by the region-data headers in regions/.
struct GeoPoint {
  float lat;
  float lon;
};

struct RegionPolygon {
  const char* name;
  const GeoPoint* points;
  int num_points;
};

// Automatic geographical region assignment.
//
// This module is intentionally decoupled from any specific application class:
// callers pass the RegionMap, NodePrefs, SensorManager and filesystem it should
// operate on. It depends only on MeshCore library types, so it can be compiled
// into any firmware environment without pulling in an app-specific header.
class AutoRegions {
public:
  static void checkRegionAutoAssign(RegionMap& region_map, NodePrefs& prefs, SensorManager& sensors, FILESYSTEM* fs);
  static bool isNodeInEurope();

private:
  static bool in_europe_flag;
  static bool inject_hierarchy(RegionMap& region_map, bool create_eu, bool create_pt);
  static uint16_t get_parent_for_region(RegionMap& region_map, const char* name);
  static bool enable_region_path(RegionMap& region_map, const char* name);
  static bool apply_dynamic_region(RegionMap& region_map, const char* reg_name, uint16_t parent_id);
  static bool remove_outdated_region(RegionMap& region_map, const char* reg_name);
};
