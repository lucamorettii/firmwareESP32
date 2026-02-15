#include "Mikai.h"
#include "core/display.h"
#include "pn532_srix.h"

void Mikai::optionsMenu() {
    std::vector<Option> options;

    options.push_back({"Read", []() { ReadUID(); }, false});

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

void ReadUID() {
    // Crea istanza del lettore PN532
#if defined(PN532_IRQ) && defined(PN532_RF_REST)
    Arduino_PN532_SRIX nfc(PN532_IRQ, PN532_RF_REST);
#else
    Arduino_PN532_SRIX nfc;
#endif

    displayRedStripe("Init PN532...", TFT_WHITE, bruceConfig.bgColor);

    // Inizializza
    if (!nfc.init()) {
        displayError("PN532 init failed!");
        delay(2000);
        return;
    }

    displayRedStripe("Place SRIX tag...", TFT_WHITE, bruceConfig.bgColor);

    // Inizializza SRIX
    if (!nfc.SRIX_init()) {
        displayError("SRIX init failed");
        delay(2000);
        return;
    }

    // Cerca tag
    if (!nfc.SRIX_initiate_select()) {
        displayError("No tag found");
        delay(2000);
        return;
    }

    displaySuccess("Tag found!");

    // Leggi UID
    uint8_t uid[8];
    if (!nfc.SRIX_get_uid(uid)) {
        displayError("UID read failed");
        delay(2000);
        return;
    }

    // Mostra UID
    String uidStr = "UID: ";
    for (uint8_t i = 0; i < 8; i++) {
        if (uid[i] < 0x10) uidStr += "0";
        uidStr += String(uid[i], HEX);
        if (i < 7) uidStr += ":";
    }
    uidStr.toUpperCase();
    displayInfo(uidStr.c_str());
    Serial.println(uidStr);
    delay(2000);

    displaySuccess("Done!");
    delay(2000);
}
