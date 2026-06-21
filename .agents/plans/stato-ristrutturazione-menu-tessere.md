# Stato Ristrutturazione Menu Tessere

## Struttura Finale del Menu

```
Tessere (launcher)
├── Gestori
│   ├── Sto&Bene
│   │   ├── Info        → mostra UID, SAK, ATQA, tipo, credito, gestore, dump SD
│   │   └── Imposta Credito → menu importi predefiniti → scrittura fisica
│   ├── aquaGold
│   │   ├── Info        → stessa struttura
│   │   └── Imposta Credito → menu importi predefiniti → scrittura fisica
│   └── TiWash
│       ├── Info        → solo dati base (NFC, nessuna lettura settori)
│       └── Imposta Credito → menu importi, ma solo placeholder
└── Genera Chiavi       → KDF Microel da UID manuale (invariata)
```

## File Modificati

### `src/core/menu_items/tessere.cpp`
- Menu ridotto a 2 voci: `"Gestori"` e `"Genera Chiavi"`
- Rimosse: Info, Read, Write, Microel, Config (non più necessarie)

### `src/itsFree/tessere/TessereMenu.h`
Aggiunte dichiarazioni:
- `menuPrincipaleGestori()`, `menuStoEBene()`, `menuAquaGold()`, `menuTiWash()`
- `mostraInfoStoEBene()`, `impostaCreditoStoEBene()`
- `mostraInfoAquaGold()`, `impostaCreditoAquaGold()`
- `mostraInfoTiWash()`, `impostaCreditoTiWash()`
- `generaChiavi()` (estratta da `menuMicroel()`)

### `src/itsFree/tessere/TessereMenu.cpp`
- `menuPrincipaleGestori()` → menu con Sto&Bene, aquaGold, TiWash
- `menuStoEBene/menuAquaGold/menuTiWash()` → ciascuno con sottomenu Info / Imposta Credito
- `mostraInfoStoEBene()` → inietta chiavi → `leggiDumpConChiavi()` → mostra credito
- `impostaCreditoStoEBene()` → inietta chiavi → legge → menu importi → `impostaCreditoStoEBene()` → `scriviDump()`
- `mostraInfoAquaGold()` → stesso pattern con chiavi aquaGold
- `impostaCreditoAquaGold()` → stesso pattern
- `mostraInfoTiWash()` → solo `attesaTag()` + info base (placeholder)
- `impostaCreditoTiWash()` → menu importi, solo messaggio (placeholder)
- `generaChiavi()` → identica logica KDF Microel, standalone

## Nuovi File

### `src/itsFree/tessere/gestori/StoEBeneLogica.h`
```cpp
#define STOEBENE_SETTORE_1 12
#define STOEBENE_SETTORE_2 13
#define STOEBENE_BLOCCO_CREDITO_1 50  // settore 12, blocco 2
#define STOEBENE_BLOCCO_CREDITO_2 54  // settore 13, blocco 2

void iniettaChiaviStoEBene(DumpMifare &dump);
uint16_t leggiCreditoStoEBene(const DumpMifare &dump);
void impostaCreditoStoEBene(DumpMifare &dump, uint16_t creditoCentesimi);
```

### `src/itsFree/tessere/gestori/StoEBeneLogica.cpp`
- **Chiavi**: Key A = `76 B1 11 C6 71 11`, Key B = `60 A1 02 D0 62 10`
- `iniettaChiaviStoEBene()` → imposta chiavi per settori 12 e 13
- `leggiCreditoStoEBene()` → legge uint16 big-endian da byte [1][2] del blocco 50
- `impostaCreditoStoEBene()` → modifica **solo blocco 2** (byte 0=00, 1-2=credito big-endian, 3=00, 8=0A) di settori 12 e 13. Non tocca blocco 0, 1, né trailer.

### `src/itsFree/tessere/gestori/AquaGoldLogica.h`
```cpp
#define AQUAGOLD_SETTORE 3
#define AQUAGOLD_BLOCCO_CREDITO 12  // settore 3, blocco 0

void iniettaChiaviAquaGold(DumpMifare &dump);
uint32_t leggiValoreAquaGold(const DumpMifare &dump);
void impostaCreditoAquaGold(DumpMifare &dump, uint16_t creditoCentesimi);
```

### `src/itsFree/tessere/gestori/AquaGoldLogica.cpp`
- **Chiavi**: Key A = `42 75 52 73 41 31` ("BuRsA1"), Key B = `FF FF FF FF FF FF`
- `iniettaChiaviAquaGold()` → imposta chiave per settore 3
- `leggiValoreAquaGold()` → legge uint32 little-endian da byte [0..3] del blocco 12. Valore in millesimi (centesimi × 10)
- `impostaCreditoAquaGold()` → costruisce blocco 16 byte: valore LE 4B + complemento NOT 4B + valore 4B + costante `0CF30CF3`. Modifica solo blocco 12.

## Note Logica Crediti

### Sto&Bene (settori 12 e 13 identici)
- Blocco 0 (abs 48/52): dati fissi del gestore (NON modificati)
- Blocco 1 (abs 49/53): dati fissi del gestore (NON modificati)
- Blocco 2 (abs 50/54): byte 0=00, byte 1-2=credito in centesimi **big-endian** (es. 15.00€ = 0x05DC), byte 3=00, byte 8=0A, resto 00
- Blocco 3 trailer (abs 51/55): `76B111C67111 FF078069 60A102D06210` (NON modificato)

### AquaGold (settore 3)
- Blocco 0 (abs 12): byte 0-3=credito×1000 in **little-endian** (es. 15.00€ → 15000 = 0x00003A98), byte 4-7=NOT di byte 0-3, byte 8-11=byte 0-3, byte 12-15=0x0CF30CF3
- Blocco 1 (abs 13): tutto zero (NON modificato)
- Blocco 2 (abs 14): dati fissi (NON modificato)
- Blocco 3 trailer (abs 15): `427552734131 FF078069 FFFFFFFFFFFF` (NON modificato)

## Pattern di Lettura/Scrittura Usato

```
iniettaChiaviX(dump_globale)    → imposta chiavi nei settori target
leggiDumpConChiavi(settoriLetti) → legge SOLO con chiavi già nel dump

impostaCreditoX(dump_globale, c) → modifica dati in RAM
scriviDump(dump_globale, settoriScritti) → scrive solo blocchi con bloccLetto=true
```

## Cose Ancora da Fare

- [ ] **TiWash**: implementare logica specifica (settore/blocchi/formato credito)
- [ ] **AquaGold Info**: testare lettura credito (valore/10 per centesimi)
- [ ] **Sto&Bene Info test**: testare su tag reale
- [ ] Salvare dump dopo imposta credito? (opzionale, per backup)
- [ ] Aggiungere opzione "Associa gestore" dopo scrittura credito?

## Build

- Ambiente: `LAUNCHER_CYD-2432S028`
- Build: OK (0 errori, Flash ~69.5%)
- Ultimo upload: 21/06/2026
