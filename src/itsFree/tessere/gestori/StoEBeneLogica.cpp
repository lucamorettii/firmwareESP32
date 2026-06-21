#include "StoEBeneLogica.h"
#include <cstring>

static const uint8_t KEY_A_STO[] = {0x76, 0xB1, 0x11, 0xC6, 0x71, 0x11};
static const uint8_t KEY_B_STO[] = {0x60, 0xA1, 0x02, 0xD0, 0x62, 0x10};

void iniettaChiaviStoEBene(DumpMifare &dump) {
    for (int s = STOEBENE_SETTORE_1; s <= STOEBENE_SETTORE_2; s++) {
        if (!dump.chiaveATrovata[s]) {
            memcpy(dump.chiaveA[s], KEY_A_STO, 6);
            dump.chiaveATrovata[s] = true;
        }
        if (!dump.chiaveBTrovata[s]) {
            memcpy(dump.chiaveB[s], KEY_B_STO, 6);
            dump.chiaveBTrovata[s] = true;
        }
    }
}

uint16_t leggiCreditoStoEBene(const DumpMifare &dump) {
    if (!dump.bloccLetto[STOEBENE_BLOCCO_CREDITO_1]) return 0;
    return ((uint16_t)dump.dati[STOEBENE_BLOCCO_CREDITO_1][1] << 8) |
            dump.dati[STOEBENE_BLOCCO_CREDITO_1][2];
}

void impostaCreditoStoEBene(DumpMifare &dump, uint16_t creditoCentesimi) {
    uint8_t blocco2[16] = {0};
    blocco2[0] = 0x00;
    blocco2[1] = (creditoCentesimi >> 8) & 0xFF;
    blocco2[2] = creditoCentesimi & 0xFF;
    blocco2[3] = 0x00;
    blocco2[8] = 0x0A;

    // Modifica solo il blocco 2 (credito) di entrambi i settori
    memcpy(dump.dati[STOEBENE_BLOCCO_CREDITO_1], blocco2, 16);
    dump.bloccLetto[STOEBENE_BLOCCO_CREDITO_1] = true;

    memcpy(dump.dati[STOEBENE_BLOCCO_CREDITO_2], blocco2, 16);
    dump.bloccLetto[STOEBENE_BLOCCO_CREDITO_2] = true;
}
