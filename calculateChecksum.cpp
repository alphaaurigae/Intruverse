#include <openssl/sha.h>
#include <openssl/evp.h>

void calculateChecksum(const char* data, size_t length, unsigned char* checksum) {
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    EVP_DigestInit(mdctx, EVP_sha256());
    EVP_DigestUpdate(mdctx, data, length);
    EVP_DigestFinal(mdctx, checksum, NULL);
    EVP_MD_CTX_free(mdctx);
}