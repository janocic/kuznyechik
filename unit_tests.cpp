#include "kuznyechik.h"
#include <iostream>
#include <iomanip>
#include <cstring>

int tests_passed = 0;
int tests_failed = 0;

void print_block(const char* label, const Block& block) {
    std::cout << label << ": ";
    for (size_t i = 0; i < block.size(); i++) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)block[i];
    }
    std::cout << std::dec << std::endl;
}

bool blocks_equal(const Block& a, const Block& b) {
    for (size_t i = 0; i < BLOCK_SIZE; i++) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

void test(const char* name, bool passed) {
    if (passed) {
        std::cout << "[PASS] " << name << std::endl;
        tests_passed++;
    } else {
        std::cout << "[FAIL] " << name << std::endl;
        tests_failed++;
    }
}

void test_gf_multiply() {
    std::cout << "\n=== GF Multiply ===" << std::endl;
    test("Multiply by 0", gf_multiply(0x57, 0x00) == 0x00);
    test("Multiply by 1", gf_multiply(0x57, 0x01) == 0x57);
    test("Commutativity", gf_multiply(0x53, 0xCA) == gf_multiply(0xCA, 0x53));
}

void test_sbox() {
    std::cout << "\n=== S-box ===" << std::endl;
    
    bool inverse_ok = true;
    for (int i = 0; i < 256; i++) {
        if (INV_SBOX[SBOX[i]] != i) {
            inverse_ok = false;
            break;
        }
    }
    test("S-box inverse property", inverse_ok);
    
    Block block = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                   0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    Block original = block;
    apply_s(block);
    apply_s_inv(block);
    test("apply_s and apply_s_inv", blocks_equal(block, original));
}

void test_r_transform() {
    std::cout << "\n=== R Transform ===" << std::endl;
    
    Block block = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                   0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00};
    Block original = block;
    
    apply_r(block);
    apply_r_inv(block);
    test("R inverse property", blocks_equal(block, original));
    
    block = original;
    for (int i = 0; i < 5; i++) apply_r(block);
    for (int i = 0; i < 5; i++) apply_r_inv(block);
    test("Multiple R applications", blocks_equal(block, original));
}

void test_l_transform() {
    std::cout << "\n=== L Transform ===" << std::endl;
    
    Block block = {0x64, 0xA5, 0x94, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    Block original = block;
    
    apply_l(block);
    apply_l_inv(block);
    test("L inverse property", blocks_equal(block, original));
    
    Block b1 = {0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67, 0x89,
                0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};
    Block b2 = b1;
    
    apply_l(b1);
    for (int i = 0; i < 16; i++) apply_r(b2);
    test("L equals 16x R", blocks_equal(b1, b2));
}

void test_lsx_transform() {
    std::cout << "\n=== LSX Transform ===" << std::endl;
    
    Block data = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x00,
                  0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88};
    Block key = {0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
                 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};
    Block original = data;
    
    Block manual = original;
    for (size_t i = 0; i < BLOCK_SIZE; i++) manual[i] ^= key[i];
    apply_s(manual);
    apply_l(manual);
    
    apply_lsx(data, key);
    test("LSX equals XOR+S+L", blocks_equal(data, manual));
}

void test_key_schedule() {
    std::cout << "\n=== Key Schedule ===" << std::endl;
    
    Key master_key = {
        0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF
    };
    
    RoundKeys round_keys;
    generate_round_keys(master_key, round_keys);
    
    Block k0, k1;
    std::memcpy(k0.data(), master_key.data(), BLOCK_SIZE);
    std::memcpy(k1.data(), master_key.data() + BLOCK_SIZE, BLOCK_SIZE);
    
    test("Round key 0 = first half", blocks_equal(round_keys[0], k0));
    test("Round key 1 = second half", blocks_equal(round_keys[1], k1));
    
    bool all_unique = true;
    for (size_t i = 0; i < NUM_ROUNDS && all_unique; i++) {
        for (size_t j = i + 1; j < NUM_ROUNDS && all_unique; j++) {
            if (blocks_equal(round_keys[i], round_keys[j])) all_unique = false;
        }
    }
    test("All round keys unique", all_unique);
}

void test_encryption() {
    std::cout << "\n=== Encryption ===" << std::endl;
    
    Key master_key = {
        0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF
    };
    
    Block plaintext = {
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x00,
        0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88
    };
    
    Block expected = {
        0x7F, 0x67, 0x9D, 0x90, 0xBE, 0xBC, 0x24, 0x30,
        0x5A, 0x46, 0x8D, 0x42, 0xB9, 0xD4, 0xED, 0xCD
    };
    
    Kuznyechik cipher(master_key);
    Block ciphertext;
    cipher.encrypt(plaintext, ciphertext);
    
    print_block("  Plaintext", plaintext);
    print_block("  Expected ", expected);
    print_block("  Got      ", ciphertext);
    
    test("RFC 7801 test vector", blocks_equal(ciphertext, expected));
}

void test_decryption() {
    std::cout << "\n=== Decryption ===" << std::endl;
    
    Key master_key = {
        0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF
    };
    
    Block ciphertext = {
        0x7F, 0x67, 0x9D, 0x90, 0xBE, 0xBC, 0x24, 0x30,
        0x5A, 0x46, 0x8D, 0x42, 0xB9, 0xD4, 0xED, 0xCD
    };
    
    Block expected = {
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x00,
        0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88
    };
    
    Kuznyechik cipher(master_key);
    Block plaintext;
    cipher.decrypt(ciphertext, plaintext);
    
    test("RFC 7801 decryption", blocks_equal(plaintext, expected));
}

void test_roundtrip() {
    std::cout << "\n=== Roundtrip ===" << std::endl;
    
    Key key = {
        0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF
    };
    
    Block original = {0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE, 0xBA, 0xBE,
                      0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    
    Kuznyechik cipher(key);
    Block encrypted, decrypted;
    
    cipher.encrypt(original, encrypted);
    cipher.decrypt(encrypted, decrypted);
    test("Encrypt-decrypt roundtrip", blocks_equal(original, decrypted));
    
    Block zeros = {};
    cipher.encrypt(zeros, encrypted);
    cipher.decrypt(encrypted, decrypted);
    test("Zero block roundtrip", blocks_equal(zeros, decrypted));
    
    Block data = original;
    for (int i = 0; i < 10; i++) {
        cipher.encrypt(data, encrypted);
        cipher.decrypt(encrypted, data);
    }
    test("10 consecutive roundtrips", blocks_equal(original, data));
}

void test_avalanche() {
    std::cout << "\n=== Avalanche Effect ===" << std::endl;
    
    Key key = {};
    Kuznyechik cipher(key);
    
    Block p1 = {};
    Block p2 = {};
    p2[15] = 0x01;
    
    Block c1, c2;
    cipher.encrypt(p1, c1);
    cipher.encrypt(p2, c2);
    
    int diff = 0;
    for (size_t i = 0; i < BLOCK_SIZE; i++) {
        if (c1[i] != c2[i]) diff++;
    }
    
    std::cout << "  1-bit change -> " << diff << " bytes differ" << std::endl;
    test("Avalanche (>=8 bytes differ)", diff >= 8);
}

int main() {
    std::cout << "=== KUZNYECHIK UNIT TESTS ===" << std::endl;
    
    test_gf_multiply();
    test_sbox();
    test_r_transform();
    test_l_transform();
    test_lsx_transform();
    test_key_schedule();
    test_encryption();
    test_decryption();
    test_roundtrip();
    test_avalanche();
    
    std::cout << "\n=== SUMMARY ===" << std::endl;
    std::cout << "Passed: " << tests_passed << std::endl;
    std::cout << "Failed: " << tests_failed << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\n*** ALL TESTS PASSED ***" << std::endl;
        return 0;
    } else {
        std::cout << "\n*** SOME TESTS FAILED ***" << std::endl;
        return 1;
    }
}
