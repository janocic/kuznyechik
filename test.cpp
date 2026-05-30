#include "kuznyechik.h"
#include <iostream>
#include <iomanip>
#include <cstring>

void print_hex(const char* label, const Block& data) {
    std::cout << label << ": ";
    for (size_t i = 0; i < data.size(); i++) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)data[i];
    }
    std::cout << std::dec << std::endl;
}

bool blocks_equal(const Block& a, const Block& b) {
    for (size_t i = 0; i < BLOCK_SIZE; i++) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

int main() {
    std::cout << "=== KUZNYECHIK TEST ===" << std::endl;
    
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
    Block ciphertext, decrypted;
    
    cipher.encrypt(plaintext, ciphertext);
    print_hex("Plaintext ", plaintext);
    print_hex("Encrypted ", ciphertext);
    print_hex("Expected  ", expected);
    
    if (blocks_equal(ciphertext, expected)) {
        std::cout << "[PASS] Encryption" << std::endl;
    } else {
        std::cout << "[FAIL] Encryption" << std::endl;
        return 1;
    }
    
    cipher.decrypt(ciphertext, decrypted);
    print_hex("Decrypted ", decrypted);
    
    if (blocks_equal(decrypted, plaintext)) {
        std::cout << "[PASS] Decryption" << std::endl;
    } else {
        std::cout << "[FAIL] Decryption" << std::endl;
        return 1;
    }
    
    std::cout << "=== ALL TESTS PASSED ===" << std::endl;
    return 0;
}
