#pragma once

#include <Arduino.h>
#include "../TessereLogica.h"

#define AQUAGOLD_SETTORE 3
#define AQUAGOLD_BLOCCO_CREDITO 12

void iniettaChiaviAquaGold(DumpMifare &dump);
uint32_t leggiValoreAquaGold(const DumpMifare &dump);
void impostaCreditoAquaGold(DumpMifare &dump, uint16_t creditoCentesimi);
