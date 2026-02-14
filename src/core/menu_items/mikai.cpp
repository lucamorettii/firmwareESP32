#include "mikai.h"
#include "display.h"
#include "utils.h"

void mikai::optionsMenu() {
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
    loopOptions(options, MENU_TYPE_SUB, "Mikai Menu");
}
