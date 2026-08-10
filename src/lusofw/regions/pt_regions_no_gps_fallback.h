#pragma once

#include <Arduino.h>

struct RegionFallback {
    const char* prefix;
    const char** regions;
    int num_regions;
};

// Define region string arrays
const char* fallback_AV[] = {"#pt-aveiro", "#pt-porto", "#pt-viseu", "#pt-coimbra", "#pt-iata-opo", "#pt-anepc-norte", "#pt-anepc-centro"};
const char* fallback_AC[] = {"#pt-ilha-de-santa-maria", "#pt-ilha-de-sao-miguel", "#pt-ilha-terceira", "#pt-ilha-da-graciosa", "#pt-ilha-de-sao-jorge", "#pt-ilha-do-pico", "#pt-ilha-do-faial", "#pt-ilha-das-flores", "#pt-ilha-do-corvo", "#pt-acores", "#pt-iata-pdl", "#pt-anepc-ra-acores"};
const char* fallback_BE[] = {"#pt-beja", "#pt-setubal", "#pt-evora", "#pt-faro", "#pt-iata-byj", "#pt-anepc-alentejo"};
const char* fallback_BR[] = {"#pt-braga", "#pt-viana-do-castelo", "#pt-vila-real", "#pt-porto", "#pt-iata-opo", "#pt-anepc-norte"};
const char* fallback_BA[] = {"#pt-braganca", "#pt-vila-real", "#pt-viseu", "#pt-guarda", "#pt-iata-opo", "#pt-anepc-norte"};
const char* fallback_CB[] = {"#pt-castelo-branco", "#pt-guarda", "#pt-coimbra", "#pt-leiria", "#pt-santarem", "#pt-portalegre", "#pt-iata-lis", "#pt-anepc-centro"};
const char* fallback_CO[] = {"#pt-coimbra", "#pt-aveiro", "#pt-viseu", "#pt-guarda", "#pt-castelo-branco", "#pt-leiria", "#pt-iata-opo", "#pt-anepc-centro"};
const char* fallback_FA[] = {"#pt-faro", "#pt-beja", "#pt-iata-fao", "#pt-anepc-algarve"};
const char* fallback_GU[] = {"#pt-guarda", "#pt-braganca", "#pt-viseu", "#pt-coimbra", "#pt-castelo-branco", "#pt-iata-opo", "#pt-anepc-centro", "#pt-anepc-norte"};
const char* fallback_LE[] = {"#pt-leiria", "#pt-coimbra", "#pt-castelo-branco", "#pt-santarem", "#pt-lisboa", "#pt-iata-lis", "#pt-anepc-centro", "#pt-anepc-oeste-e-vale-do-tejo"};
const char* fallback_LI[] = {"#pt-lisboa", "#pt-leiria", "#pt-santarem", "#pt-setubal", "#pt-iata-lis", "#pt-anepc-grande-lisboa", "#pt-anepc-oeste-e-vale-do-tejo"};
const char* fallback_MA[] = {"#pt-ilha-da-madeira", "#pt-ilha-de-porto-santo", "#pt-madeira", "#pt-iata-fnc", "#pt-anepc-ra-madeira"};
const char* fallback_PA[] = {"#pt-portalegre", "#pt-castelo-branco", "#pt-santarem", "#pt-evora", "#pt-iata-byj", "#pt-anepc-alentejo"};
const char* fallback_PO[] = {"#pt-porto", "#pt-braga", "#pt-vila-real", "#pt-viseu", "#pt-aveiro", "#pt-iata-opo", "#pt-anepc-norte"};
const char* fallback_SA[] = {"#pt-santarem", "#pt-leiria", "#pt-castelo-branco", "#pt-portalegre", "#pt-evora", "#pt-setubal", "#pt-lisboa", "#pt-iata-lis", "#pt-anepc-oeste-e-vale-do-tejo", "#pt-anepc-centro", "#pt-anepc-alentejo"};
const char* fallback_SE[] = {"#pt-setubal", "#pt-lisboa", "#pt-santarem", "#pt-evora", "#pt-beja", "#pt-iata-lis", "#pt-anepc-peninsula-de-setubal", "#pt-anepc-alentejo"};
const char* fallback_VC[] = {"#pt-viana-do-castelo", "#pt-braga", "#pt-iata-opo", "#pt-anepc-norte"};
const char* fallback_VR[] = {"#pt-vila-real", "#pt-braga", "#pt-porto", "#pt-viseu", "#pt-braganca", "#pt-iata-opo", "#pt-anepc-norte"};
const char* fallback_VI[] = {"#pt-viseu", "#pt-porto", "#pt-vila-real", "#pt-braganca", "#pt-guarda", "#pt-coimbra", "#pt-aveiro", "#pt-iata-opo", "#pt-anepc-centro", "#pt-anepc-norte"};
const char* fallback_EV[] = {"#pt-evora", "#pt-portalegre", "#pt-santarem", "#pt-setubal", "#pt-beja", "#pt-iata-byj", "#pt-anepc-alentejo"};

const RegionFallback FALLBACK_REGIONS[] = {
    {"AV", fallback_AV, 7},
    {"AC", fallback_AC, 12},
    {"BE", fallback_BE, 6},
    {"BR", fallback_BR, 6},
    {"BA", fallback_BA, 6},
    {"CB", fallback_CB, 8},
    {"CO", fallback_CO, 8},
    {"FA", fallback_FA, 4},
    {"GU", fallback_GU, 8},
    {"LE", fallback_LE, 8},
    {"LI", fallback_LI, 7},
    {"MA", fallback_MA, 5},
    {"PA", fallback_PA, 6},
    {"PO", fallback_PO, 7},
    {"SA", fallback_SA, 11},
    {"SE", fallback_SE, 8},
    {"VC", fallback_VC, 4},
    {"VR", fallback_VR, 7},
    {"VI", fallback_VI, 10},
    {"EV", fallback_EV, 7}
};
const int NUM_FALLBACK_REGIONS = 20;
