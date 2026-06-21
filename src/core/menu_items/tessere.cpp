/**
 * @file Tessere.cpp
 */

#include "Tessere.h"
#include "core/display.h"
#include "itsFree/tessere/TessereLogica.h"
#include "itsFree/tessere/TessereMenu.h"

void Tessere::optionsMenu() {
    // Inizializza il PN532 all'ingresso del menu: se fallisce non mostrare le opzioni
    if (!inizializzaNfc()) return;

    std::vector<Option> opzioni = {
        {"Gestori",      []() { menuPrincipaleGestori(); }, false},
        {"Genera Chiavi", []() { generaChiavi(); },        false},
    };

    loopOptions(opzioni, MENU_TYPE_SUBMENU, "Tessere");
}

void Tessere::drawIcon(float scala) {
    clearIconArea();

    // ── Corpo della carta ─────────────────────────────────────────────────────
    int w = (int)(scala * 80);      // larghezza proporzionale alla scala
    int h = (int)(scala * 50);      // altezza proporzionale alla scala
    int left = iconCenterX - w / 2; // bordo sinistro centrato
    int top = iconCenterY - h / 2;  // bordo superiore centrato

    tft.drawRect(left, top, w, h, bruceConfig.priColor);

    // ── Chip EMV ──────────────────────────────────────────────────────────────
    int chipW = w / 4;                   // larghezza chip = 1/4 della carta
    int chipH = h / 2;                   // altezza chip = 1/2 della carta
    int chipX = left + w / 8;            // margine sinistro
    int chipY = iconCenterY - chipH / 2; // centrato verticalmente

    tft.drawRect(chipX, chipY, chipW, chipH, bruceConfig.priColor);
}
