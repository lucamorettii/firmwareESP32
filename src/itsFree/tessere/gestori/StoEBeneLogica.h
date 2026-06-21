#pragma once

#include <Arduino.h>
#include "../TessereLogica.h"

#define STOEBENE_SETTORE_1 12
#define STOEBENE_SETTORE_2 13
#define STOEBENE_BLOCCO_CREDITO_1 50
#define STOEBENE_BLOCCO_CREDITO_2 54

void iniettaChiaviStoEBene(DumpMifare &dump);
uint16_t leggiCreditoStoEBene(const DumpMifare &dump);
void impostaCreditoStoEBene(DumpMifare &dump, uint16_t creditoCentesimi);
