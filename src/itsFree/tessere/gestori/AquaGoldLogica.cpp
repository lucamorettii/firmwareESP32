#include "AquaGoldLogica.h"
#include <cstring>

static const uint8_t KEY_A_AQUA[] = {0x42, 0x75, 0x52, 0x73, 0x41, 0x31};
static const uint8_t KEY_B_AQUA[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void iniettaChiaviAquaGold(DumpMifare &dump) {
    int s = AQUAGOLD_SETTORE;
    if (!dump.chiaveATrovata[s]) {
        memcpy(dump.chiaveA[s], KEY_A_AQUA, 6);
        dump.chiaveATrovata[s] = true;
    }
    if (!dump.chiaveBTrovata[s]) {
        memcpy(dump.chiaveB[s], KEY_B_AQUA, 6);
        dump.chiaveBTrovata[s] = true;
    }
}

uint32_t leggiValoreAquaGold(const DumpMifare &dump) {
    if (!dump.bloccLetto[AQUAGOLD_BLOCCO_CREDITO]) return 0;
    uint32_t valore = 0;
    for (int i = 0; i < 4; i++) {
        valore |= ((uint32_t)dump.dati[AQUAGOLD_BLOCCO_CREDITO][i]) << (i * 8);
    }
    return valore;
}

void impostaCreditoAquaGold(DumpMifare &dump, uint16_t creditoCentesimi) {
    uint32_t valore = (uint32_t)creditoCentesimi * 10;

    uint8_t blocco0[16];
    for (int i = 0; i < 4; i++) {
        blocco0[i] = (valore >> (i * 8)) & 0xFF;
    }
    for (int i = 0; i < 4; i++) {
        blocco0[4 + i] = ~blocco0[i] & 0xFF;
    }
    for (int i = 0; i < 4; i++) {
        blocco0[8 + i] = blocco0[i];
    }
    blocco0[12] = 0x0C;
    blocco0[13] = 0xF3;
    blocco0[14] = 0x0C;
    blocco0[15] = 0xF3;

    // Modifica solo il blocco credito
    memcpy(dump.dati[AQUAGOLD_BLOCCO_CREDITO], blocco0, 16);
    dump.bloccLetto[AQUAGOLD_BLOCCO_CREDITO] = true;
}
