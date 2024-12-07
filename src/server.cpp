#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <openssl/sha.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <mutex>
#include <algorithm>
#include <thread>
#include <csignal>
#include <atomic>

using boost::asio::ip::tcp;
boost::asio::io_context io_context;
std::string filePath;
std::vector<std::shared_ptr<tcp::socket>> clients;
std::mutex clientsMutex;

extern void calculateChecksum(const char* data, size_t length, unsigned char* checksum);

std::atomic<bool> shutting_down(false);

void handleShutdown(int signal) {
    std::cout << "Received shutdown signal: " << signal << std::endl;
    shutting_down = true;
    io_context.stop();

    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        for (auto& client : clients) {
            if (client->is_open()) {
                client->close();
            }
        }
    }
}

void sendFileContent(std::shared_ptr<tcp::socket> client) {
    if (shutting_down) return;

    std::cout << "sendFileContent: Starting the process of sending file content." << std::endl;

    // Open the file once, and keep it open for the entire operation
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "sendFileContent: Failed to open file." << std::endl;
        return;
    }
    std::cout << "sendFileContent: File opened successfully." << std::endl;

    char buffer[1024];

    std::function<void()> sendNextChunk = [&]() {
        // Check if there is more data to read from the file
        if (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
            size_t bytesRead = file.gcount();
            std::cout << "sendFileContent: Read " << bytesRead << " bytes from file." << std::endl;

            // Prepare the checksum for the chunk
            unsigned char checksum[SHA256_DIGEST_LENGTH];
            std::cout << "sendFileContent: Calculating checksum for the current chunk." << std::endl;
            calculateChecksum(buffer, bytesRead, checksum);
            std::cout << "sendFileContent: Checksum calculated." << std::endl;

            std::vector<boost::asio::const_buffer> writeBuffers = {
                boost::asio::buffer(checksum, SHA256_DIGEST_LENGTH),
                boost::asio::buffer(buffer, bytesRead)
            };

            // Send the checksum and data to the client asynchronously
            boost::asio::async_write(*client, writeBuffers, [&, sendNextChunk](boost::system::error_code ec, std::size_t) {
                if (ec) {
                    std::cerr << "sendFileContent: Write failed: " << ec.message() << std::endl;
                    client->close();
                } else {
                    std::cout << "sendFileContent: Write succeeded." << std::endl;
                    sendNextChunk();
                }
            });
        } else {
            // No more data to send, close the file and socket
            file.close();
            std::cout << "sendFileContent: File closed after all chunks were sent." << std::endl;
            client->close();
        }
    };

    // Start sending the file content by calling sendNextChunk
    sendNextChunk();
}

void onNewConnection(tcp::acceptor& acceptor) {
    std::shared_ptr<tcp::socket> client = std::make_shared<tcp::socket>(io_context);
    acceptor.async_accept(*client, [client, &acceptor](boost::system::error_code ec) {
        if (!ec && !shutting_down) {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients.push_back(client);

            // Start sending the file content to the newly connected client
            sendFileContent(client);

            // Continue accepting new clients unless shutting down
            if (!shutting_down) {
                onNewConnection(acceptor);
            }
        } else {
            std::cerr << "Error accepting connection: " << ec.message() << std::endl;
        }
    });
}

// Function to handle file changes and notify clients
void onFileChange() {
    if (shutting_down) return;

    std::lock_guard<std::mutex> lock(clientsMutex);
    for (auto& client : clients) {
        sendFileContent(client);  // Immediately send file content to all clients
    }
}

// Prune closed client sockets
void pruneClosedClients() {
    if (shutting_down) return;

    std::lock_guard<std::mutex> lock(clientsMutex);
    clients.erase(
        std::remove_if(clients.begin(), clients.end(), [](const std::shared_ptr<tcp::socket>& client) {
            return !client->is_open();
        }),
        clients.end());
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file-path>" << std::endl;
        return 1;
    }

    filePath = argv[1];

    try {
        std::signal(SIGINT, handleShutdown);
        std::signal(SIGTERM, handleShutdown);

        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));

        onNewConnection(acceptor);
        
        // Simulate a file change after X seconds
        std::this_thread::sleep_for(std::chrono::seconds(5));
        onFileChange();

        std::this_thread::sleep_for(std::chrono::seconds(10));
        pruneClosedClients();

        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}