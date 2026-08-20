#pragma once

#ifdef ENABLE_REGION_IATA

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

#endif
