# Kuznyechik (GOST R 34.12-2015)

🇭🇷 **Hrvatski**

Ovaj repozitorij sadrži C++ implementaciju simetričnog blok algoritma šifriranja **Kuznyechik** (GOST R 34.12-2015), razvijenu u sklopu seminarskog rada. Algoritam podržava šifriranje 128-bitnih blokova podataka koristeći 256-bitne ključeve.

**Značajke:**
- Cjelovita implementacija algoritma u C++11 (S-kutije, L-transformacije, aritmetika u Galoisovim poljima).
- Uključeni i provjereni službeni **RFC 7801** testni vektori za potpunu ispravnost.
- Skup jediničnih (unit) i općenitih testova koji jamče stabilan rad i analiziraju Avalanche efekt (efekt lavine).

**Prevođenje i pokretanje:**
Za kompilaciju i pokretanje testova u Windows okruženju, koristite priloženu `.bat` skriptu:
```cmd
.\compile.bat
.\unit_tests.exe
.\test.exe
```

---

🇬🇧 **English**

This repository contains a C++ implementation of the **Kuznyechik** (GOST R 34.12-2015) symmetric block cipher, developed as part of a university seminar project. The algorithm supports 128-bit data blocks and uses 256-bit keys.

**Features:**
- Complete C++11 implementation of the algorithm components (S-boxes, L-transformations, Galois field arithmetic).
- Verified against official **RFC 7801** test vectors to ensure correctness.
- Comprehensive unit and general tests ensuring algorithm stability and analyzing the Avalanche effect.

**Compilation and execution:**
To compile and run the test suite on Windows, use the provided `.bat` script:
```cmd
.\compile.bat
.\unit_tests.exe
.\test.exe
```
