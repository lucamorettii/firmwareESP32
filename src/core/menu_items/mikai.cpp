#include "Mikai.h"
#include "core/display.h"
#include "core/utils.h"

void Mikai::optionsMenu() {
    std::vector<Option> options;

    options.push_back(
        {"Read",
         []() {
             Serial.println("Funzione 1 eseguita");
             // qui il tuo codice reale
         },
         false}
    );

    options.push_back(
        {"Info",
         []() {
             Serial.println("Funzione 1 eseguita");
             // qui il tuo codice reale
         },
         false}
    );

    options.push_back(
        {"Add credit",
         []() {
             Serial.println("Funzione 1 eseguita");
             // qui il tuo codice reale
         },
         false}
    );

    options.push_back(
        {"Reset mykey",
         []() {
             Serial.println("Funzione 1 eseguita");
             // qui il tuo codice reale
         },
         false}
    );

    options.push_back(
        {"Import vendor",
         []() {
             Serial.println("Funzione 2 eseguita");
             // qui il tuo codice reale
         },
         false}
    );

    options.push_back(
        {"Export vendor",
         []() {
             Serial.println("Funzione 2 eseguita");
             // qui il tuo codice reale
         },
         false}
    );

    options.push_back(
        {"Write",
         []() {
             Serial.println("Funzione 2 eseguita");
             // qui il tuo codice reale
         },
         false}
    );

    // Chiama loopOptions come per il menu principale, ma con tipo SUB
    loopOptions(options, MENU_TYPE_SUBMENU, "Mikai Menu");
}

void Mikai::drawIcon(float scale) {
    clearIconArea();

    int w = scale * 60; // larghezza banconota
    int h = scale * 36; // altezza banconota
    int lineW = scale * 4;
    int padding = scale * 6;

    // Cornice rettangolo banconota
    tft.drawRect(iconCenterX - w / 2, iconCenterY - h / 2, w, h, bruceConfig.priColor);

    // Linee interne per dettaglio banconota
    tft.drawRect(
        iconCenterX - w / 2 + padding,
        iconCenterY - h / 2 + padding,
        w - 2 * padding,
        h - 2 * padding,
        bruceConfig.priColor
    );

    // Simbolo Euro "€"
    int euroRadius = h / 4;

    // Cerchio centrale
    tft.drawCircle(iconCenterX, iconCenterY, euroRadius, bruceConfig.priColor);

    // Linee centrali del €
    tft.drawLine(
        iconCenterX - euroRadius,
        iconCenterY - euroRadius / 2,
        iconCenterX + euroRadius,
        iconCenterY - euroRadius / 2,
        bruceConfig.priColor
    );

    tft.drawLine(
        iconCenterX - euroRadius,
        iconCenterY + euroRadius / 2,
        iconCenterX + euroRadius,
        iconCenterY + euroRadius / 2,
        bruceConfig.priColor
    );

    // Curva del €
    for (int i = -euroRadius; i <= euroRadius; i++) {
        float angle = map(i, -euroRadius, euroRadius, -1.5, 1.5);
        int x = iconCenterX + i;
        int y = iconCenterY - int(sin(angle) * euroRadius);
        tft.drawPixel(x, y, bruceConfig.priColor);
    }
}
