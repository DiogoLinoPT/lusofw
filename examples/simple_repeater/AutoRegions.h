#pragma once

#include "MyMesh.h"

class AutoRegions {
public:
    static void checkRegionAutoAssign(MyMesh* mesh);
    static bool isNodeInEurope();
private:
    static bool in_europe_flag;
    static void inject_hierarchy(MyMesh* mesh, bool create_eu, bool create_pt);
    static uint16_t get_parent_for_region(MyMesh* mesh, const char* name);
    static void enable_region_path(MyMesh* mesh, const char* name);
    static void apply_dynamic_region(MyMesh* mesh, const char* reg_name, uint16_t parent_id);
    static void remove_outdated_region(MyMesh* mesh, const char* reg_name);
};
