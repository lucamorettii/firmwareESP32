# Piano: Liberare Memoria + Nuove Funzioni Tessere (MIFARE)

## Obiettivi
1. Rimuovere moduli non necessari tramite flag di compilazione
2. Aggiungere a Tessere: Key Map, Confronta Dump, nuovo gestore ybb

---

## FASE 1 — Flag di Compilazione per Moduli da Rimuovere

### Moduli da sacrificare (selezionati)
| Modulo | Guardia da aggiungere | Stima risparmio |
|--------|----------------------|----------------|
| **JS Interpreter** (`src/modules/bjs_interpreter/`, 45 file) | `#ifndef DISABLE_INTERPRETER` | ~150-250 KB |
| **RF/Sub-GHz** (`src/modules/rf/`, 24 file) | `#ifndef LITE_VERSION` | ~80-120 KB |
| **IR** (`src/modules/ir/`, 11 file) | `#ifndef LITE_VERSION` | ~60-100 KB |
| **GPS** (`src/modules/gps/`, 6 file) | `#ifndef LITE_VERSION` | ~30-50 KB |
| **pwnagotchi** (`src/modules/pwnagotchi/`, 10 file) | `#ifndef LITE_VERSION` | ~40-60 KB |
| **NRF24** (`src/modules/NRF24/`, 6 file) | `#ifndef LITE_VERSION` | ~30-50 KB |
| **FM Radio** | Già escluso da `LITE_VERSION` | — |
| **Ethernet** | Già escluso da `LITE_VERSION` | — |
| **LoRa** | Già escluso da `LITE_VERSION` | — |
| **TOTALE RECUPERABILE** | | **~400-600 KB** |

### Moduli da tenere
- WiFi (deauth, evil portal, sniffer, ecc.)
- BLE (spam, apple, ninebot)
- reverseShell
- Mikai (SRIX4K)
- Tessere (MIFARE) + nuove funzioni

### File da modificare

**`platformio.ini`:**
- `[env]` → `build_flags`: aggiungere opzioni per `DISABLE_INTERPRETER` e `LITE_VERSION` esteso
- `[env_light]` → estendere `lib_deps` per escludere: `IRremoteESP8266`, `SmartRC-CC1101-Driver-Lib`, `RF24`, `TinyGPSPlus`, `mquickjs`

**`include/globals.h`:**
- Wrappare `#include "interpreter_interface.h"` e relativi oggetti con `#ifndef DISABLE_INTERPRETER`

**`src/core/main_menu.cpp/h`:**
- Aggiungere guardie `#ifndef LITE_VERSION` per voci menu: RF, IR, GPS, NRF24, pwnagotchi

**`src/core/settings.cpp/h`:**
- Opzionale: toggle runtime per esclusione moduli (richiede reboot)

### Template guardia per file modulo
```cpp
// Nei file .h e .cpp di ogni modulo da escludere
#ifndef LITE_VERSION
// ... codice modulo ...
#endif
```

---

## FASE 2 — Nuove Funzioni Tessere (MIFARE)

### 2.1 — Key Map (difficoltà: MEDIA, ~4 ore)

**Cosa fa**: Come l'app MCT (MIFARE Classic Tool) Android.
1. Chiede "Avvicina il tag..." → `attesaTag()`
2. Legge il dump completo usando chiavi da `/rfid/tag/chiavi.txt` → `leggiDump()`
3. Mostra griglia settori: **verde** = chiave trovata e dati letti, **rosso** = non autenticato
4. Navigazione: selezioni un settore → mostra hex dump dei suoi blocchi (4 blocchi per settore su MIFARE 1K, 16 su settori estesi 4K)
5. Opzione "Salva dump su SD" → `salvaDump()`

**Layout schermata griglia:**
```
┌──────────────────────────┐
│  Key Map  (UID: A1B2..)  │
│                          │
│  ┌──┬──┬──┬──┬──┬──┬──┐  │
│  │S0│S1│S2│S3│S4│S5│S6│  │  ← colonne di settori
│  │██│██│██│░░│██│██│░░│  │  ██=verde(letto) ░░=rosso(non auth)
│  ├──┼──┼──┼──┼──┼──┼──┤  │
│  │S7│S8│S9│SA│SB│SC│SD│  │
│  │░░│██│██│██│░░│██│░░│  │
│  ├──┴──┴──┴──┴──┴──┴──┤  │
│  │   > Seleziona settore│  │
│  │   Salva dump su SD   │  │
│  └──────────────────────┘  │
│  ↑ Prev  ↓ Next  Sel↩ Esc↩│
└──────────────────────────┘
```

**Layout hex dump (alla MCT):**
```
┌──────────────────────────┐
│  Settore 5  (Key A: OK)  │
│                          │
│  Blk 20: AA BB CC DD ... │
│  Blk 21: 11 22 33 44 ... │
│  Blk 22: FF EE DD CC ... │
│  Blk 23: 00 00 00 00 ... │  ← trailer (chiavi visibili)
│                          │
│  ┌──────────────────────┐│
│  │  > Torna alla griglia ││
│  └──────────────────────┘│
│  ↑ Prev  ↓ Next  Sel↩ Esc↩│
└──────────────────────────┘
```

**Implementazione:**
```
TessereMenu.cpp → nuova funzione keyMap()
1. attesaTag() → rileva tag
2. leggiDump(settoriLetti) → usa leggiDumpClassic() con chiavi da SD
3. Render griglia:
   - Matrice di rettangoli colorati per settori
   - Sotto ogni rettangolo: numero settore (S0-S15 o S0-S39)
   - Legenda colori
4. Navigazione: seleziona settore con SelPress → mostra hex dump
5. Hex dump view:
   - Per ogni blocco del settore: "Bxx: " + 16 byte in HEX
   - Se è trailer: mostra anche Key A e Key B
6. Opzione "Salva" → salvaDump()
```

**Note**: Riutilizza `DumpMifare`, `leggiDump()` (versione ottimizzata, vedi 2.1.5), `caricaChiavi()`, `salvaDump()`.

---

### 2.1.5 — Ottimizzazione ricerca chiavi (difficoltà: BASSA, ~2 ore)

**Perché**: usata da Key Map (2.1) e dal menu Leggi esistente. `leggiDumpClassic()` è oggi molto lento.

**Problemi attuali in `leggiDumpClassic()` (`TessereLogica.cpp:387`):**

| # | Problema | Codice | Effetto |
|---|----------|--------|---------|
| 1 | Re-selezione lenta | `readPassiveTargetID(..., 1000)` — timeout 1s dopo ogni auth fallita | ~50-100ms sprecati per tentativo |
| 2 | Key B prova tutto | Loop completo su tutte le chiavi anche dopo Key A trovata | 2× i tentativi necessari |
| 3 | Nessuna cache | Ogni settore ricomincia da FF/00, anche se la chiave era già stata trovata prima | Ripe stessa chiave più volte |
| 4 | Nessun ordinamento | Chiavi provate in ordine file — quelle più probabili potrebbero essere in fondo | Più tentativi del necessario |

**Ottimizzazioni:**

**A) Timeout adattivo** (`TessereLogica.cpp:406-407`)
```
Modifica: readPassiveTargetID(..., 1000) → readPassiveTargetID(..., 100)
Motivo: dopo auth fallita, se il tag è ancora presente, 100ms bastano per ri-selezionarlo
Risparmio: ~10× più veloce sui fallimenti
```

**B) Key B intelligente** (`TessereLogica.cpp:410-421`)
```
Dopo Key A trovata, Key B prova solo:
  1. Key A stessa (a volte Key A == Key B)
  2. Complemento di Key A (0xFF - ogni byte)
  3. FF FF FF FF FF FF
  4. 00 00 00 00 00 00
  5. Solo le chiavi "forti" dal file (es. note come Key B, non tutto il file)
```

**C) Cache chiavi per settore successivo**
```
- Inserire un piccolo buffer (max 5 chiavi) delle ultime chiavi trovate
- Per ogni nuovo settore, provare PRIMA le chiavi della cache
- Le chiavi nel buffer si riordinano: più successi → più priorità
```

**D) Ordinamento statistico chiavi all'avvio**
```
- All'avvio di Tessere (o prima della lettura):
  - Leggere chiavi.txt
  - Mettere in testa: FF*6, 00*6, A0B0C0D0E0F0, D3F7D3F7D3F7, ecc.
  - Mettere in fondo: chiavi specifiche per UID di altri tag (formato UID,CHIAVE)
```

**Risultato atteso:**

| Scenario | Oggi | Ottimizzato | Guadagno |
|----------|------|-------------|----------|
| 100 chiavi, tag 1K, nessuna nota | ~60-80s | ~8-12s | ~7× |
| 100 chiavi, tag 1K, tutte note | ~3-5s | ~1-2s | ~3× |
| 50 chiavi, tag 1K, 50% note | ~20-40s | ~3-5s | ~7× |
| 500 chiavi, tag 1K (key heavy) | ~5-8 min | ~30-60s | ~8× |

**File da modificare:**
- `src/itsFree/tessere/TessereLogica.cpp` → `leggiDumpClassic()`, nuova funzione `caricaChiaviOrdinate()`

---

### 2.2 — Dump Comparison (difficoltà: BASSA, ~3 ore)

**Cosa fa**: Carica 2 dump `.bin` da SD, confronta byte per byte, mostra differenze per settore/blocco.

**Implementazione:**
```
TessereMenu.cpp → nuova funzione confrontaDump()
1. Lista file .bin da SD → "Seleziona dump 1"
2. Lista file .bin da SD → "Seleziona dump 2"
3. Carica entrambi in due DumpMifare locali statici
4. Confronto byte-per-byte con output:
   - "Dump IDENTICI" se nessuna differenza
   - Altrimenti: "Settore X: blocco Y differisce (AA:BB:CC:DD vs EE:FF:GG:HH)"
5. Opzione "Dettaglio" → hex dump affiancato
```

**Note**: La funzione `caricaDump()` esiste già. Serve solo UI di selezione e confronto.

---

*Nota: attacchi crittografici (Nested, Hardnested) rimossi dal piano — l'autenticazione si basa solo su bruteforce con chiavi da file.*

---

## FASE 3 — Modifiche al Menu Tessere

### Struttura menu aggiornata
```
Tessere (menu principale)
├── Gestori (esistente)
│   ├── Sto&Bene
│   │   ├── Info
│   │   └── Imposta Credito
│   ├── AquaGold
│   │   ├── Info
│   │   └── Imposta Credito
│   ├── TiWash
│   │   ├── Info
│   │   └── Imposta Credito
│   └── ybb (NUOVO)
│       ├── Info
│       └── Imposta Credito
├── Calcola Chiavi (da NON toccare il codice esistente — diventa submenu)
│   ├── Microel (esistente — calcola chiavi in automatico da UID)
│   └── Comestero (NUOVO — solo placeholder, NON implementare ancora)
├── Key Map            ← NUOVO
└── Confronta Dump     ← NUOVO
```

---

## Struttura nuovi file (solo nuove UI / logica)

```
src/itsFree/tessere/
├── TessereLogica.h/cpp         (esistente — ottimizzare caricaChiavi + leggiDumpClassic)
├── TessereMenu.h/cpp           (esistente — aggiungere keyMap, confrontaDump, ybb)
├── microel/                    (esistente)
├── gestori/                    (esistente)
│   └── YbbLogica.h/cpp         ← NUOVO (logica ybb)
└── test/                       (esistente)
```

---

## Rischi e Mitigazioni

| Rischio | Impatto | Mitigazione |
|---------|---------|-------------|
| Ottimizzazioni chiavi rompono lettura esistente | Alto | Testare su tag con chiavi note prima e dopo |
| DumpComparison non trova differenze su dump corrotto | Basso | Validare checksum/firma MFDR prima del confronto |
| ybb ha struttura blocco diversa dalle attese | Medio | Aggiungere test con dump reale prima del rilascio |
| Regressions da flag compilazione | Medio | Testare build full vs light per ogni cambio |
