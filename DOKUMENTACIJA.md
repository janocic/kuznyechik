# Kuznyechik - Implementacija

Implementacija Kuznyechik blok sifre prema GOST R 34.12-2015 standardu (RFC 7801).

## Opis algoritma

Kuznyechik je ruska blok sifra sa sljedecim karakteristikama:

- Velicina bloka: 128 bita (16 bajtova)
- Velicina kljuca: 256 bita (32 bajta)
- Broj rundi: 10
- Struktura: Supstitucijsko-permutacijska mreza (SPN)

## Struktura projekta

```
kuznyechik-simple/
├── kuznyechik.hpp    - Kompletna implementacija
├── test.cpp          - Test program
├── compile.bat       - Build za Windows
└── compile.sh        - Build za Linux/Mac
```

## Kompajliranje i pokretanje

### Windows
```
compile.bat
```

### Linux/Mac
```
chmod +x compile.sh
./compile.sh
```

### Rucno
```
g++ -std=c++11 -O2 -Wall -o test test.cpp
./test
```

## Implementacija

### Osnovne komponente

**S-box transformacija**
Nelinearna supstitucija koja zamjenjuje svaki bajt prema S-box tablici iz RFC 7801.

**L transformacija**
Linearna transformacija koja se sastoji od 16 primjena R transformacije.

**R transformacija**
Linearni shift sa Galois Field GF(2^8) mnozenjem. Koristi 16 linearnih koeficijenata.

**Galois Field aritmetika**
Mnozenje u polju GF(2^8) sa ireducibilnim polinomom x^8 + x^7 + x^6 + x + 1.

**LSX transformacija**
Kombinacija XOR sa round kljucem, S-box transformacije i L transformacije.

**Key schedule**
Generiranje 10 round kljuceva iz master kljuca koristeci Feistel mrezu sa 32 iteracije.

### Enkripcija

1. 9 rundi LSX transformacije (XOR + S + L)
2. Finalni XOR sa 10. round kljucem

### Dekripcija

1. XOR sa 10. round kljucem
2. 9 inverznih rundi (L^-1 + S^-1 + XOR)

## Koristenje

```cpp
#include "kuznyechik.hpp"

Key master_key = { ... };  // 32 bajta
Block plaintext = { ... }; // 16 bajtova

Kuznyechik cipher(master_key);

Block ciphertext;
cipher.encrypt(plaintext, ciphertext);

Block decrypted;
cipher.decrypt(ciphertext, decrypted);
```

## RFC 7801 kompatibilnost

Implementacija prolazi sve RFC 7801 test vektore:

- Master key: 88 99 AA BB CC DD EE FF...
- Plaintext: 11 22 33 44 55 66 77 00...
- Expected: 7F 67 9D 90 BE BC 24 30...

## Tehnicke specifikacije

- Standard: C++11
- Vanjske zavisnosti: Nema
- Optimizacije: -O2
- Velicina koda: ~200 linija (hpp) + ~150 linija (test)

## Napomene

Sve kriptografske konstante (S-box, linearni koeficijenti) preuzete su direktno
iz RFC 7801 specifikacije i stoga su identicne u svim implementacijama koje
slijede standard.

Implementacija je napravljena u obrazovne svrhe.
