#if defined(ENABLE_AUTO_REGIONS)

#include "AutoRegions.h"

#include <Arduino.h>
#include <helpers/TxtDataHelpers.h>

// Define the distance buffer for each region type (0, 5, 10, 20, 35, 50)
#define REGION_DISTRICTS_BUFFER 0 // km
#define REGION_NUTS2_BUFFER     0 // km
#define REGION_CIMS_BUFFER      0 // km 

// Enable or disable specific region types
#define ENABLE_REGION_DISTRICTS
#define ENABLE_REGION_NUTS2
// #define ENABLE_REGION_CIMS
// #define ENABLE_REGION_IATA

// Version of the region geometry engine. Change to force cache recalculation on next boot.
#define REGION_ENGINE_VERSION 0.50f // v0.50

#define STR_HELPER(x)               #x
#define STR(x)                      STR_HELPER(x)

#define HEADER_DISTRICTS_FILE(b)    lusofw/regions/pt_districts_##b##km.h
#define HEADER_NUTS2_FILE(b)        lusofw/regions/pt_anepc_nuts2_##b##km.h
#define HEADER_CIMS_FILE(b)         lusofw/regions/pt_anepc_cims_##b##km.h

#define HEADER_DISTRICTS(b)         STR(HEADER_DISTRICTS_FILE(b))
#define HEADER_NUTS2(b)             STR(HEADER_NUTS2_FILE(b))
#define HEADER_CIMS(b)              STR(HEADER_CIMS_FILE(b))

#ifdef ENABLE_REGION_DISTRICTS
  #include HEADER_DISTRICTS(REGION_DISTRICTS_BUFFER)
#endif

#ifdef ENABLE_REGION_NUTS2
  #include HEADER_NUTS2(REGION_NUTS2_BUFFER)
#endif

#ifdef ENABLE_REGION_CIMS
  #include HEADER_CIMS(REGION_CIMS_BUFFER)
#endif

#include "lusofw/regions/pt_regions_no_gps_fallback.h"
#include "lusofw/regions/eu_region_0km.h"

bool AutoRegions::in_europe_flag = false;

bool AutoRegions::isNodeInEurope() {
    return in_europe_flag;
}
// Classic Ray-Casting algorithm (Fast and allocation-free)
static bool isPointInPolygon(float lat, float lon, const GeoPoint* poly, int num_points) {
    bool inside = false;
    for (int i = 0, j = num_points - 1; i < num_points; j = i++) {
        // Check if the horizontal ray intersects the segment between nodes i and j
        if (((poly[i].lon > lon) != (poly[j].lon > lon)) &&
            (lat < (poly[j].lat - poly[i].lat) * (lon - poly[i].lon) / (poly[j].lon - poly[i].lon) + poly[i].lat)) {
            inside = !inside;
        }
    }
    return inside;
}

void AutoRegions::inject_hierarchy(RegionMap& region_map, bool create_eu, bool create_pt) {
    if (create_eu) {
        auto r_europa = region_map.findByName("#eu");
        if (!r_europa) {
            r_europa = region_map.putRegion("#eu", region_map.getWildcard().id);
            if (r_europa) r_europa->flags |= REGION_AUTO_ASSIGN;
        }
        if (r_europa) r_europa->flags |= REGION_DENY_FLOOD;
    }

    if (create_pt) {
        auto r_europa = region_map.findByName("#eu");
        auto r_portugal = region_map.findByName("#pt");
        if (!r_portugal) {
            r_portugal = region_map.putRegion("#pt", r_europa ? r_europa->id : region_map.getWildcard().id);
            if (r_portugal) r_portugal->flags |= REGION_AUTO_ASSIGN;
        } else if (r_portugal->parent != (r_europa ? r_europa->id : region_map.getWildcard().id)) {
            r_portugal->parent = r_europa ? r_europa->id : region_map.getWildcard().id;
        }
        if (r_portugal) r_portugal->flags |= REGION_DENY_FLOOD;
    }
}

uint16_t AutoRegions::get_parent_for_region(RegionMap& region_map, const char* name) {
    if (strcmp(name, "#eu") == 0) {
        return region_map.getWildcard().id;
    }
    if (strcmp(name, "#pt") == 0) {
        auto p = region_map.findByName("#eu");
        return p ? p->id : region_map.getWildcard().id;
    }
    auto p = region_map.findByName("#pt");
    return p ? p->id : region_map.getWildcard().id;
}

void AutoRegions::enable_region_path(RegionMap& region_map, const char* name) {
    auto r = region_map.findByName(name);
    while (r) {
        r->flags &= ~REGION_DENY_FLOOD; // Enable flood for this region and all its parents
        if (r->parent != region_map.getWildcard().id) {
            r = region_map.findById(r->parent);
        } else {
            break;
        }
    }
}

void AutoRegions::apply_dynamic_region(RegionMap& region_map, const char* reg_name, uint16_t parent_id) {
    auto dynamic_region = region_map.findByName(reg_name);
    if (!dynamic_region) {
        dynamic_region = region_map.putRegion(reg_name, parent_id);
        if (dynamic_region) dynamic_region->flags |= REGION_AUTO_ASSIGN;
    } else if (dynamic_region->parent != parent_id) {
        dynamic_region->parent = parent_id;
    }
    enable_region_path(region_map, reg_name);
}

void AutoRegions::remove_outdated_region(RegionMap& region_map, const char* reg_name) {
    auto r = region_map.findByName(reg_name);
    if (r && (r->flags & REGION_AUTO_ASSIGN)) {
        region_map.removeRegion(*r);
    }
}

void AutoRegions::checkRegionAutoAssign(RegionMap& region_map, NodePrefs& prefs, SensorManager& sensors, FILESYSTEM* fs) {
    static bool state_loaded = false;
    static float last_checked_lat = -999.0f;
    static float last_checked_lon = -999.0f;
    static char last_checked_name[sizeof(prefs.node_name)] = {0};

    static bool force_initial_check = false;
    static float original_airtime_factor = -1.0f;

    if (!state_loaded) {
        state_loaded = true;
        #if defined(RP2040_PLATFORM)
        File f = fs->open("/lusofw_regions_state.txt", "r");
        #else
        File f = fs->open("/lusofw_regions_state.txt");
        #endif
        if (f) {
            float version = f.parseFloat();
            if (abs(version - REGION_ENGINE_VERSION) < 0.001f) {
                last_checked_lat = f.parseFloat();
                last_checked_lon = f.parseFloat();
                f.readStringUntil('\n'); // consume delimiter
                String name = f.readStringUntil('\n');
                name.trim();
                StrHelper::strncpy(last_checked_name, name.c_str(), sizeof(last_checked_name));
            } else {
                force_initial_check = true;
            }
            f.close();
        } else {
            force_initial_check = true;
        }
    }

    float current_lat = 0.0f;
    float current_lon = 0.0f;

    if (prefs.advert_loc_policy == ADVERT_LOC_PREFS) {
        current_lat = prefs.node_lat;
        current_lon = prefs.node_lon;
    } else if (prefs.advert_loc_policy == ADVERT_LOC_SHARE) {
        current_lat = sensors.node_lat;
        current_lon = sensors.node_lon;
    }

    bool name_changed = (strcmp(prefs.node_name, last_checked_name) != 0);
    bool state_needs_saving = false;

    // If the user's manual coordinates (_prefs) are 0.0, they explicitly cleared them.
    // Unlike a physical GPS losing lock, a manual 0.0 is an explicit command to drop location.
    // We discard the last known location and force a fallback evaluation immediately.
    if (prefs.advert_loc_policy == ADVERT_LOC_PREFS && current_lat == 0.0f && current_lon == 0.0f && (last_checked_lat != 0.0f || last_checked_lon != 0.0f)) {
        last_checked_lat = 0.0f;
        last_checked_lon = 0.0f;
        force_initial_check = true;
        state_needs_saving = true;
    }

    bool force_check = force_initial_check || (prefs.advert_loc_policy == ADVERT_LOC_NONE && (last_checked_lat != 0.0f || last_checked_lon != 0.0f));

    float eval_lat = (current_lat == 0.0f) ? last_checked_lat : current_lat;
    float eval_lon = (current_lon == 0.0f) ? last_checked_lon : current_lon;

    bool has_gps = (eval_lat != 0.0f || eval_lon != 0.0f);

    float diff_lat = eval_lat > last_checked_lat ? eval_lat - last_checked_lat : last_checked_lat - eval_lat;
    float diff_lon = eval_lon > last_checked_lon ? eval_lon - last_checked_lon : last_checked_lon - eval_lon;
    bool gps_changed = (diff_lat > 0.01f || diff_lon > 0.01f);

    if (state_loaded && !force_check && !name_changed && !gps_changed) {
        return; // No movement, no name change -> do nothing
    }

    force_initial_check = false;

    const char* valid_regions[16];
    int num_valid = 0;
    bool is_in_portugal = false;
    bool is_in_europe = false;

    auto add_valid_region = [&](const char* name) {
        if (num_valid < 16) {
            valid_regions[num_valid++] = name;
        }
    };

    auto is_region_valid = [&](const char* name) {
        for (int i = 0; i < num_valid; i++) {
            if (strcmp(valid_regions[i], name) == 0) return true;
        }
        return false;
    };

    struct IataHub {
        const char* name;
        float lat;
        float lon;
    };

    const IataHub iata_hubs[] = {
        {"#pt-iata-opo", 41.2481f, -8.6814f},
        {"#pt-iata-lis", 38.7742f, -9.1342f},
        {"#pt-iata-byj", 38.0792f, -7.9308f},
        {"#pt-iata-fao", 37.0144f, -7.9659f},
        {"#pt-iata-pdl", 37.7412f, -25.6978f},
        {"#pt-iata-ter", 38.7619f, -27.0908f},
        {"#pt-iata-fnc", 32.6970f, -16.7744f}
    };

    if (has_gps) {
        auto evaluate_polygon_array = [&](const RegionPolygon* polys, int count) {
            for (int i = 0; i < count; i++) {
                if (isPointInPolygon(eval_lat, eval_lon, polys[i].points, polys[i].num_points)) {
                    is_in_portugal = true;
                    add_valid_region(polys[i].name);
                }
            }
        };

        #ifdef ENABLE_REGION_DISTRICTS
        evaluate_polygon_array(PORTUGAL_DISTRICTS, NUM_PORTUGAL_DISTRICTS);
        #endif

        #ifdef ENABLE_REGION_NUTS2
        evaluate_polygon_array(PORTUGAL_ANEPC_NUTS2, NUM_PORTUGAL_ANEPC_NUTS2);
        #endif

        #ifdef ENABLE_REGION_CIMS
        evaluate_polygon_array(PORTUGAL_ANEPC_CIMS, NUM_PORTUGAL_ANEPC_CIMS);
        #endif

        #ifdef ENABLE_REGION_IATA
        if (is_in_portugal) {
            const char* closest_iata = nullptr;
            float min_dist_sq = 999999.0f;

            for (int i = 0; i < 7; i++) {
                float dx = eval_lat - iata_hubs[i].lat;
                float dy = (eval_lon - iata_hubs[i].lon) * 0.76f;
                float dist_sq = dx * dx + dy * dy;
                if (dist_sq < min_dist_sq) {
                    min_dist_sq = dist_sq;
                    closest_iata = iata_hubs[i].name;
                }
            }
            if (closest_iata) {
                add_valid_region(closest_iata);
            }
        }
        #endif

        if (is_in_portugal) {
            is_in_europe = true;
            add_valid_region("#pt");
        } else {
            if (isPointInPolygon(eval_lat, eval_lon, REGION_EUROPE.points, REGION_EUROPE.num_points)) {
                is_in_europe = true;
            }
        }

        if (is_in_europe) {
            add_valid_region("#eu");
        }
    } else {
        if (prefs.node_name[0] != '\0' && prefs.node_name[1] != '\0' && prefs.node_name[2] == '.') {
            char prefix[3];
            prefix[0] = toupper(prefs.node_name[0]);
            prefix[1] = toupper(prefs.node_name[1]);
            prefix[2] = '\0';

            for (int i = 0; i < NUM_FALLBACK_REGIONS; i++) {
                if (strcmp(FALLBACK_REGIONS[i].prefix, prefix) == 0) {
                    is_in_portugal = true;
                    is_in_europe = true;
                    add_valid_region("#pt");
                    add_valid_region("#eu");

                    for (int j = 0; j < FALLBACK_REGIONS[i].num_regions; j++) {
                        const char* reg_name = FALLBACK_REGIONS[i].regions[j];
#ifndef ENABLE_REGION_IATA
                        if (strstr(reg_name, "iata") != nullptr) continue;
#endif
#ifndef ENABLE_REGION_NUTS2
                        if (strcmp(reg_name, "#pt-anepc-norte") == 0 ||
                            strcmp(reg_name, "#pt-anepc-algarve") == 0 ||
                            strcmp(reg_name, "#pt-anepc-centro") == 0 ||
                            strcmp(reg_name, "#pt-anepc-grande-lisboa") == 0 ||
                            strcmp(reg_name, "#pt-anepc-peninsula-de-setubal") == 0 ||
                            strcmp(reg_name, "#pt-anepc-alentejo") == 0 ||
                            strcmp(reg_name, "#pt-anepc-oeste-e-vale-do-tejo") == 0 ||
                            strcmp(reg_name, "#pt-anepc-ra-acores") == 0 ||
                            strcmp(reg_name, "#pt-anepc-ra-madeira") == 0) {
                            continue;
                        }
#endif
                        add_valid_region(reg_name);
                    }
                    break;
                }
            }
        }
    }

    // Now remove any region that is NOT in valid_regions
    #ifdef ENABLE_REGION_DISTRICTS
    for (int i = 0; i < NUM_PORTUGAL_DISTRICTS; i++) {
        if (!is_region_valid(PORTUGAL_DISTRICTS[i].name)) remove_outdated_region(region_map, PORTUGAL_DISTRICTS[i].name);
    }
    #endif

    #ifdef ENABLE_REGION_NUTS2
    for (int i = 0; i < NUM_PORTUGAL_ANEPC_NUTS2; i++) {
        if (!is_region_valid(PORTUGAL_ANEPC_NUTS2[i].name)) remove_outdated_region(region_map, PORTUGAL_ANEPC_NUTS2[i].name);
    }
    #endif

    #ifdef ENABLE_REGION_CIMS
    for (int i = 0; i < NUM_PORTUGAL_ANEPC_CIMS; i++) {
        if (!is_region_valid(PORTUGAL_ANEPC_CIMS[i].name)) remove_outdated_region(region_map, PORTUGAL_ANEPC_CIMS[i].name);
    }
    #endif

    #ifdef ENABLE_REGION_IATA
    for (int i = 0; i < 7; i++) {
        if (!is_region_valid(iata_hubs[i].name)) remove_outdated_region(region_map, iata_hubs[i].name);
    }
    #endif

    for (int i = 0; i < NUM_FALLBACK_REGIONS; i++) {
        for (int j = 0; j < FALLBACK_REGIONS[i].num_regions; j++) {
            if (!is_region_valid(FALLBACK_REGIONS[i].regions[j])) {
                remove_outdated_region(region_map, FALLBACK_REGIONS[i].regions[j]);
            }
        }
    }

    if (!is_region_valid("#pt")) remove_outdated_region(region_map, "#pt");
    if (!is_region_valid("#eu")) remove_outdated_region(region_map, "#eu");

    in_europe_flag = is_in_europe;

    // Enforce European regulations (max 10% duty cycle / min 9.0 airtime factor)
    // whenever the node is physically located within the European geography.
    // We only overwrite the active setting in RAM if the user's setting is currently illegal.
    // NOTE: We DO NOT call savePrefs() here because flash writes block interrupts,
    // which causes a boot crash (WDT/Hard Fault) on nRF52/RAK4631.
    if (in_europe_flag) {
        if (prefs.airtime_factor < 9.0f) {
            // Capture the original permissive duty cycle before restricting it
            if (original_airtime_factor < 0.0f) {
                original_airtime_factor = prefs.airtime_factor;
            }
            prefs.airtime_factor = 9.0f;
        }
    } else {
        // Restore the original permissive duty cycle if we leave Europe
        if (original_airtime_factor >= 0.0f) {
            prefs.airtime_factor = original_airtime_factor;
            original_airtime_factor = -1.0f;
        }
    }

    // Now apply all valid regions
    if (num_valid > 0) {
        inject_hierarchy(region_map, is_in_europe, is_in_portugal);
        for (int i = 0; i < num_valid; i++) {
            apply_dynamic_region(region_map, valid_regions[i], get_parent_for_region(region_map, valid_regions[i]));
            // MESH_DEBUG_PRINTLN("Auto-Region Assign: %s", valid_regions[i]);
        }
    }

    // Update state
    if (force_check || gps_changed || name_changed) {
        if (has_gps) {
            last_checked_lat = eval_lat;
            last_checked_lon = eval_lon;
        }
        StrHelper::strncpy(last_checked_name, prefs.node_name, sizeof(last_checked_name));
        state_needs_saving = true;
    }

    if (state_needs_saving) {
        region_map.save(fs); // Persist assigned regions to NVS

        if (fs->exists("/lusofw_regions_state.txt")) {
            fs->remove("/lusofw_regions_state.txt");
        }
        #if defined(NRF52_PLATFORM) || defined(STM32_PLATFORM)
        File f = fs->open("/lusofw_regions_state.txt", FILE_O_WRITE);
        #elif defined(RP2040_PLATFORM)
        File f = fs->open("/lusofw_regions_state.txt", "w");
        #else
        File f = fs->open("/lusofw_regions_state.txt", "w", true);
        #endif
        if (f) {
            f.println(REGION_ENGINE_VERSION, 2);
            f.println(last_checked_lat, 6);
            f.println(last_checked_lon, 6);
            f.println(last_checked_name);
            f.close();
        }
    }
}

#endif // ENABLE_AUTO_REGIONS
