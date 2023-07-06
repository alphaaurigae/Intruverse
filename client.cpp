#include <iostream>
#include <cstring>
#include <uv.h>
#include <openssl/sha.h>
#include <openssl/evp.h>

const int BUFFER_SIZE = 1024;

void clearConsole() {
    std::cout << "\033[2J\033[1;1H";
}

extern void calculateChecksum(const char* data, size_t length, unsigned char* checksum);

void onRead(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
    if (nread < 0) {
        if (nread != UV_EOF) {
            std::cerr << "Read error: " << uv_strerror(nread) << std::endl;
        }
        uv_close((uv_handle_t*)stream, nullptr);
        free(buf->base);
    } else if (nread > 0) {
        if (nread < SHA256_DIGEST_LENGTH) {
            std::cerr << "Received incomplete data" << std::endl;
            return;
        }

        unsigned char receivedChecksum[SHA256_DIGEST_LENGTH];
        unsigned char calculatedChecksum[SHA256_DIGEST_LENGTH];
        memcpy(receivedChecksum, buf->base, SHA256_DIGEST_LENGTH);
        const char* data = buf->base + SHA256_DIGEST_LENGTH;
        const ssize_t dataLength = nread - SHA256_DIGEST_LENGTH;

        // Verify the checksum
        calculateChecksum(data, dataLength, calculatedChecksum);
        if (memcmp(receivedChecksum, calculatedChecksum, SHA256_DIGEST_LENGTH) != 0) {
            std::cerr << "Checksum mismatch. Data may be corrupted or tampered." << std::endl;
            return;
        }

        clearConsole(); // Clear the console before writing new data
        std::cout.write(data, dataLength);
        std::cout.flush();
        std::cout << std::endl; // Add a newline after each update
    }
}

void onConnect(uv_connect_t* req, int status) {
    if (status < 0) {
        std::cerr << "Connection error: " << uv_strerror(status) << std::endl;
        uv_close((uv_handle_t*)req->handle, nullptr);
        free(req);
    } else {
        uv_read_start(req->handle, [](uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
            buf->base = (char*)malloc(suggested_size);
            buf->len = suggested_size;
        }, onRead);
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: ./client <server_ip> <server_port>" << std::endl;
        return 1;
    }

    const std::string serverIP = argv[1];
    const int serverPort = std::stoi(argv[2
]);

    uv_loop_t* loop = uv_default_loop();

    uv_tcp_t client;
    uv_tcp_init(loop, &client);

    sockaddr_in serverAddr{};
    uv_ip4_addr(serverIP.c_str(), serverPort, &serverAddr);

    uv_connect_t* connectReq = (uv_connect_t*)malloc(sizeof(uv_connect_t));
    uv_tcp_connect(connectReq, &client, (const sockaddr*)&serverAddr, onConnect);

    uv_run(loop, UV_RUN_DEFAULT);

    uv_loop_close(loop);
    uv_loop_delete(loop);
    free(connectReq);

    return 0;
}