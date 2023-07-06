#include <iostream>
#include <fstream>
#include <uv.h>
#include <vector>
#include <openssl/sha.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>

uv_loop_t* loop;
uv_tcp_t server;
std::string filePath;
std::vector<uv_tcp_t*> clients;

extern void calculateChecksum(const char* data, size_t length, unsigned char* checksum);


void sendFileContent(uv_stream_t* client) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return;
    }

    char buffer[1024];
    while (!file.eof()) {
        file.read(buffer, sizeof(buffer));
        ssize_t bytesRead = file.gcount();
        if (bytesRead > 0) {
            unsigned char checksum[SHA256_DIGEST_LENGTH];
            calculateChecksum(buffer, bytesRead, checksum);
            uv_buf_t checksumBuf = uv_buf_init((char*)checksum, SHA256_DIGEST_LENGTH);
            uv_buf_t dataBuf = uv_buf_init(buffer, bytesRead);

            uv_write_t* req = (uv_write_t*)malloc(sizeof(uv_write_t));
            req->data = client;

            uv_buf_t writeBufs[] = { checksumBuf, dataBuf };
            uv_write(req, client, writeBufs, 2, [](uv_write_t* req, int status) {
                uv_stream_t* client = (uv_stream_t*)req->data;
                if (status < 0) {
                    // Write error handling
                }
                free(req);
                if (!uv_is_closing((uv_handle_t*)client)) {
                    sendFileContent(client);
                }
            });

            uv_sleep(100);
        }
    }

    file.close();
}

void onNewConnection(uv_stream_t* server, int status) {
    if (status < 0) {
        return;
    }

    uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, client);

    if (uv_accept(server, (uv_stream_t*)client) == 0) {
        clients.push_back(client);
        sendFileContent((uv_stream_t*)client);
    } else {
        uv_close((uv_handle_t*)client, nullptr);
        free(client);
    }
}

void onFileChange(uv_fs_event_t* handle, const char* filename, int events, int status) {
    if (status < 0) {
        return;
    }

    if (events & UV_CHANGE) {
        for (uv_tcp_t* client : clients) {
            sendFileContent((uv_stream_t*)client);
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        return 1;
    }

    filePath = argv[1];

    loop = uv_default_loop();

    uv_tcp_init(loop, &server);
    sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", 8000, &addr);
    uv_tcp_bind(&server, (const sockaddr*)&addr, 0);
    int res = uv_listen((uv_stream_t*)&server, 128, onNewConnection);
    if (res < 0) {
        return 1;
    }

    uv_fs_event_t fileEvent;
    uv_fs_event_init(loop, &fileEvent);
    uv_fs_event_start(&fileEvent, onFileChange, filePath.c_str(), 0);

    uv_run(loop, UV_RUN_DEFAULT);

    uv_loop_close(loop);
    uv_loop_delete(loop);

    return 0;
}