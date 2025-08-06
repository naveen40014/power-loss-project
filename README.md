# power-loss-project

MODE 1 – Quick-and-Easy (no battery)
Works if your SIM800A board is clearly marked “5 V IN” (has its own regulator).

Parts
DC-DC buck converter (e.g. LM2596) set to 5 V / ≥2 A

SIM800A “5 V IN” module

ESP32 dev board

Relay module (5 V coil)

470 µF electrolytic capacitor (on SIM800A VCC)

Wiring Steps
#	Action
1	Buck IN + / IN - → printer supply (e.g. 24 V). Adjust pot until OUT = 5.0 V.
2	Buck OUT + → ESP32 5 V/Vin pin.
3	Buck OUT + → Relay VCC pin.
4	Buck OUT + → SIM800A 5 V IN pin.
5	Buck OUT - → common GND for ESP32, SIM800A, Relay.
6	Add the 470 µF cap across SIM800A 5 V & GND pins.
7	Finish logic wiring (SIM TX/RX ↔ ESP32 17/16, Relay OUT → GPIO 14).

✔ Done. When mains fails the whole board turns off, so SMS only works while power is present.

MODE 2 – Recommended Battery-Backup (SMS even when mains dies)
Works with any SIM800A (5 V or bare 4 V) and gives ≈10 s runtime to send SMS.

Parts
Same 5 V buck converter (≥2 A)

TP4056 Li-ion charger module

18650 Li-ion cell (2 000–3 000 mAh)

Two Schottky diodes (SS34 or 1N5819)

MT3608 boost set to 4.0 V (only if your SIM800A needs 4 V)

470 µF cap on ESP32 Vin, 1000 µF cap on SIM800A VCC

Wiring Steps
#	Action
A Main 5 V Path	
1	Buck OUT + → Diode-1 anode.
2	Diode-1 cathode → create the 5 V rail (label it).
3	Buck OUT - → common GND.
B Battery & Charger	
4	Solder 18650 to TP4056 B+ / B-.
5	TP4056 OUT + → Diode-2 anode.
6	Diode-2 cathode → same 5 V rail.
7	TP4056 OUT - → common GND.
C Feeding the Boards	
8	5 V rail → ESP32 Vin.
9	5 V rail → Relay VCC.
10	If SIM800A is “5 V IN”, connect its VCC to 5 V rail directly. (skip steps 11-12)
11	(Bare 4 V module only) 5 V rail → MT3608 IN+ (GND to GND).
12	MT3608 OUT 4.0 V → SIM800A VCC.
D Stabilise	
13	470 µF cap across ESP32 Vin & GND.
14	1000 µF cap across SIM800A VCC & GND (close to the module).
E Logic Connections	
15	SIM TX → ESP32 GPIO 16, SIM RX → ESP32 GPIO 17.
16	Relay OUT (NO pin) → ESP32 GPIO 14, add 10 k pull-up to 3 .3 V.

How it works

Mains present → 5 V rail via Buck & Diode-1 (≈ 4.7 V after drop). Battery charges through TP4056.

Mains lost → Buck collapses, Diode-1 reverse-biases, Diode-2 now forward-biases. Battery (≈ 3.6 V after drop) powers the 5 V rail → ESP32 keeps running.

Boost (if used) still gives 4 V @ 2 A to SIM800A, long enough to send SMS.

What to double-check
Item	Target
Buck OUT	5.0 – 5.2 V
MT3608 OUT	4.0 – 4.1 V (if used)
Battery after diodes	≥ 3.3 V under load
SIM800A signal	AT ⇒ OK, AT+CPIN? ⇒ READY, AT+CREG? ⇒ 0,1 or 0,5

Follow the steps in order and your system will power up correctly and stay alive just long enough to deliver its SMS alert whenever your printer’s mains fails.







Ask ChatGPT
