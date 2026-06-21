# Piano: Ristrutturazione Menu Tessere

## Obiettivo

Quando l'utente seleziona "Tessere" dal launcher di Bruce, deve apparire un menu con solo due opzioni:
- **Gestori** → sottomenu con i gestori specifici
- **Genera Chiavi** → funzione esistente (estrazione da `menuMicroel()`)

Ogni gestore avrà un proprio sottomenu con "Info" e "Imposta Credito", ciascuno con la propria logica specifica per calcolo/impostazione del credito.

---

## Struttura del Menu

```
Tessere
├── Gestori
│   ├── Sto&Bene
│   │   ├── Info
│   │   └── Imposta Credito
│   ├── aquaGold
│   │   ├── Info
│   │   └── Imposta Credito
│   └── TiWash
│       ├── Info
│       └── Imposta Credito
└── Genera Chiavi
```

## Modifiche ai File

### 1. `src/core/menu_items/tessere.cpp`

- **Modificare** `Tessere::optionsMenu()`:
  - Svuotare il `std::vector<Option>` esistente
  - Aggiungere solo due opzioni:
    - `"Gestori"` → chiama `menuPrincipaleGestori()`
    - `"Genera Chiavi"` → chiama `generaChiavi()` (nuova funzione)

### 2. `src/itsFree/tessere/TessereMenu.h`

- **Dichiarare** le nuove funzioni:
  - `void menuPrincipaleGestori();`
  - `void menuStoEBene();`
  - `void menuAquaGold();`
  - `void menuTiWash();`
  - `void mostraInfoStoEBene();`
  - `void impostaCreditoStoEBene();`
  - `void mostraInfoAquaGold();`
  - `void impostaCreditoAquaGold();`
  - `void mostraInfoTiWash();`
  - `void impostaCreditoTiWash();`
  - `void generaChiavi();` (estratta da `menuMicroel()`)

### 3. `src/itsFree/tessere/TessereMenu.cpp`

- **Implementare** `menuPrincipaleGestori()`:
  - `std::vector<Option>` con le voci: "Sto&Bene", "aquaGold", "TiWash"
  - Ogni voce chiama la rispettiva funzione sottomenu (`menuStoEBene()`, ecc.)

- **Implementare** `menuStoEBene()`, `menuAquaGold()`, `menuTiWash()`:
  - Ciascuna crea un `std::vector<Option>` con "Info" e "Imposta Credito"
  - Le lambda chiamano le funzioni di logica specifiche (`mostraInfoStoEBene()`, ecc.)

- **Implementare** `generaChiavi()`:
  - Estrarre la logica di generazione chiavi da `menuMicroel()` (linee 362-423 in `TessereMenu.cpp`)
  - Mantenere input UID, calcolo Key A/B, opzione salvataggio SD

- **Rimuovere** (o mantenere come non utilizzate):
  - `menuMicroel()` — non più chiamata direttamente
  - `menuGestori()` — non più chiamata direttamente (opzione "Configurazione Gestori" eliminata)

### 4. Nuovi file per logica gestori (da valutare)

Opzione A — file separati per chiarezza:
- `src/itsFree/tessere/gestori/StoEBeneLogica.h/.cpp`
- `src/itsFree/tessere/gestori/AquaGoldLogica.h/.cpp`
- `src/itsFree/tessere/gestori/TiWashLogica.h/.cpp`

Opzione B — integrazione in `TessereLogica.h/.cpp` se la logica è minimale.

**Decisione:** Da valutare in fase di implementazione.

---

## Funzioni di Logica per Gestore (Placeholder iniziali)

Ogni gestore avrà:
- `mostraInfoX()` — mostra UID, SAK, ATQA, tipo, credito corrente, info gestore, presenza dump SD
- `impostaCreditoX()` — legge la tessera, mostra credito attuale, chiede nuovo importo, applica logica di scrittura

La logica specifica per calcolo/impostazione credito sarà implementata successivamente con le regole di ciascun gestore.

---

## Note

- Nessuna modifica a `MifareKeysManager` (non coinvolto)
- Nessuna modifica a `TessereLogica.h/.cpp` (API già sufficienti)
- Il PN532 viene inizializzato all'ingresso del menu Tessere (già implementato in `optionsMenu()`)
- Le funzioni `mostraMessaggio()`, `mostraInfo()`, `attesaTag()`, `leggiDump()`, `scriviDump()` restano disponibili da `TessereLogica.h`
