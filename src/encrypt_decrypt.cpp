#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <iostream>
#include <string>

RSA* loadPrivateKey(const std::string& privateKeyFile) {
    FILE* file = fopen(privateKeyFile.c_str(), "rb");
    if (!file) {
        std::cerr << "Unable to open private key file." << std::endl;
        return nullptr;
    }
    RSA* rsa = PEM_read_RSAPrivateKey(file, nullptr, nullptr, nullptr);
    fclose(file);
    return rsa;
}

RSA* loadPublicKey(const std::string& publicKeyFile) {
    FILE* file = fopen(publicKeyFile.c_str(), "rb");
    if (!file) {
        std::cerr << "Unable to open public key file." << std::endl;
        return nullptr;
    }
    RSA* rsa = PEM_read_RSA_PUBKEY(file, nullptr, nullptr, nullptr);
    fclose(file);
    return rsa;
}

std::string rsaEncrypt(RSA* rsa, const std::string& data) {
    std::vector<unsigned char> encrypted(RSA_size(rsa));
    int len = RSA_public_encrypt(data.size(), (unsigned char*)data.c_str(), encrypted.data(), rsa, RSA_PKCS1_OAEP_PADDING);
    if (len == -1) {
        std::cerr << "RSA encryption failed" << std::endl;
        return "";
    }
    return std::string(encrypted.begin(), encrypted.begin() + len);
}

std::string rsaDecrypt(RSA* rsa, const std::string& encryptedData) {
    std::vector<unsigned char> decrypted(RSA_size(rsa));
    int len = RSA_private_decrypt(encryptedData.size(), (unsigned char*)encryptedData.c_str(), decrypted.data(), rsa, RSA_PKCS1_OAEP_PADDING);
    if (len == -1) {
        std::cerr << "RSA decryption failed" << std::endl;
        return "";
    }
    return std::string(decrypted.begin(), decrypted.begin() + len);
}