#ifndef CALCULATE_CHECKSUM_H
#define CALCULATE_CHECKSUM_H

#include <openssl/sha.h>
#include <openssl/evp.h>
#include <iostream>

inline void calculateChecksum(const char* data, size_t length, unsigned char* checksum) {
    // Allocate
    EVP_MD_CTX* digestContext = EVP_MD_CTX_new();
    if (!digestContext) {
        std::cerr << "Failed to create EVP_MD_CTX." << std::endl;
        return;
    }

    // Initialize
    if (EVP_DigestInit(digestContext, EVP_sha256()) != 1) {
        std::cerr << "Failed to initialize SHA-256 digest." << std::endl;
        EVP_MD_CTX_free(digestContext);
        return;
    }

    // Update
    if (EVP_DigestUpdate(digestContext, data, length) != 1) {
        std::cerr << "Failed to update SHA-256 digest." << std::endl;
        EVP_MD_CTX_free(digestContext);
        return;
    }

    // Finalize
    if (EVP_DigestFinal(digestContext, checksum, nullptr) != 1) {
        std::cerr << "Failed to finalize SHA-256 digest." << std::endl;
        EVP_MD_CTX_free(digestContext);
        return;
    }

    EVP_MD_CTX_free(digestContext);
}

#endif