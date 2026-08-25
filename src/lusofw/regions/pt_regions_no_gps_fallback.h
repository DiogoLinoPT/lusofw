#pragma once

#include <Arduino.h>

struct RegionFallback {
    const char* prefix;
    const char** regions;
    int num_regions;
};

// Define region string arrays (Apenas Distrito alvo + ANEPC NUTS II + ANEPC CIM + IATA)
const char* fallback_AV[] = {"#pt-aveiro", "#pt-iata-opo", "#pt-anepc-centro", "#pt-anepc-regiao-de-aveiro"};
const char* fallback_AC[] = {"#pt-acores", "#pt-iata-pdl", "#pt-anepc-acores"};
const char* fallback_BE[] = {"#pt-beja", "#pt-iata-byj", "#pt-anepc-alentejo", "#pt-anepc-baixo-alentejo"};
const char* fallback_BR[] = {"#pt-braga", "#pt-iata-opo", "#pt-anepc-norte", "#pt-anepc-cavado", "#pt-anepc-ave"};
const char* fallback_BA[] = {"#pt-braganca", "#pt-iata-opo", "#pt-anepc-norte", "#pt-anepc-terras-tras-os-montes"};
const char* fallback_CB[] = {"#pt-castelo-branco", "#pt-iata-lis", "#pt-anepc-centro", "#pt-anepc-beira-baixa"};
const char* fallback_CO[] = {"#pt-coimbra", "#pt-iata-opo", "#pt-anepc-centro", "#pt-anepc-regiao-de-coimbra"};
const char* fallback_FA[] = {"#pt-faro", "#pt-iata-fao", "#pt-anepc-algarve"};
const char* fallback_GU[] = {"#pt-guarda", "#pt-iata-opo", "#pt-anepc-centro", "#pt-anepc-beiras-e-serra-estrela"};
const char* fallback_LE[] = {"#pt-leiria", "#pt-iata-lis", "#pt-anepc-centro", "#pt-anepc-lisboa-vale-tejo", "#pt-anepc-regiao-de-leiria", "#pt-anepc-oeste"};
const char* fallback_LI[] = {"#pt-lisboa", "#pt-iata-lis", "#pt-anepc-lisboa-vale-tejo", "#pt-anepc-grande-lisboa", "#pt-anepc-oeste"};
const char* fallback_MA[] = {"#pt-madeira", "#pt-iata-fnc", "#pt-anepc-madeira"};
const char* fallback_PA[] = {"#pt-portalegre", "#pt-iata-byj", "#pt-anepc-alentejo", "#pt-anepc-alto-alentejo"};
const char* fallback_PO[] = {"#pt-porto", "#pt-iata-opo", "#pt-anepc-norte", "#pt-anepc-porto", "#pt-anepc-tamega-e-sousa"};
const char* fallback_SA[] = {"#pt-santarem", "#pt-iata-lis", "#pt-anepc-lisboa-vale-tejo", "#pt-anepc-centro", "#pt-anepc-medio-tejo", "#pt-anepc-leziria-do-tejo"};
const char* fallback_SE[] = {"#pt-setubal", "#pt-iata-lis", "#pt-anepc-lisboa-vale-tejo", "#pt-anepc-alentejo", "#pt-anepc-peninsula-de-setubal", "#pt-anepc-alentejo-litoral"};
const char* fallback_VC[] = {"#pt-viana-do-castelo", "#pt-iata-opo", "#pt-anepc-norte", "#pt-anepc-alto-minho"};
const char* fallback_VR[] = {"#pt-vila-real", "#pt-iata-opo", "#pt-anepc-norte", "#pt-anepc-douro"};
const char* fallback_VI[] = {"#pt-viseu", "#pt-iata-opo", "#pt-anepc-centro", "#pt-anepc-norte", "#pt-anepc-viseu-dao-lafoes", "#pt-anepc-douro"};
const char* fallback_EV[] = {"#pt-evora", "#pt-iata-byj", "#pt-anepc-alentejo", "#pt-anepc-alentejo-central"};

const RegionFallback FALLBACK_REGIONS[] = {
    {"AV", fallback_AV, 4},
    {"AC", fallback_AC, 3},
    {"BE", fallback_BE, 4},
    {"BR", fallback_BR, 5},
    {"BA", fallback_BA, 4},
    {"CB", fallback_CB, 4},
    {"CO", fallback_CO, 4},
    {"FA", fallback_FA, 3},
    {"GU", fallback_GU, 4},
    {"LE", fallback_LE, 6},
    {"LI", fallback_LI, 5},
    {"MA", fallback_MA, 3},
    {"PA", fallback_PA, 4},
    {"PO", fallback_PO, 5},
    {"SA", fallback_SA, 6},
    {"SE", fallback_SE, 6},
    {"VC", fallback_VC, 4},
    {"VR", fallback_VR, 4},
    {"VI", fallback_VI, 6},
    {"EV", fallback_EV, 4}
};
const int NUM_FALLBACK_REGIONS = 20;
