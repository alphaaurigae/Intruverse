#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <atomic>
#include <functional>
#include <cstring>
#include <openssl/sha.h>

using boost::asio::ip::tcp;

constexpr int BUFFER_SIZE = 1024;
constexpr int RECONNECT_INTERVAL_MS = 1000;
std::atomic<bool> is_connected{false};

void calculateChecksum(const char* data, size_t length, unsigned char* checksum);

class Client {
public:
    Client(boost::asio::io_context& io_context, const std::string& server_ip, int server_port)
        : io_context_(io_context),
          socket_(io_context),
          timer_(io_context),
          server_endpoint_(boost::asio::ip::make_address(server_ip), server_port) {
        connectToServer();
    }

private:
    void connectToServer() {
        postLogMessage("Attempting to connect to server...\n");
        socket_.async_connect(server_endpoint_,
            [this](const boost::system::error_code& ec) {
                if (ec) {
                    postLogMessage("Connection failed: " + ec.message() + "\n");
                    scheduleReconnect();
                } else {
                    postLogMessage("Connected to the server.\n");
                    is_connected = true;
                    startReading();
                }
            });
    }

void startReading() {
    socket_.async_read_some(boost::asio::buffer(buffer_),
        [this](const boost::system::error_code& ec, std::size_t length) {
            if (ec) {
                if (ec == boost::asio::error::eof) {
                    // EOF: The connection was cleanly closed by the server
                    // postLogMessage("Connection closed by server.\n");
                    is_connected = false;
                    socket_.close();
                    scheduleReconnect();
                } else {
                    postLogMessage("Connection lost: " + ec.message() + "\n");
                    is_connected = false;
                    socket_.close();
                    scheduleReconnect();
                }
            } else {
                processReceivedData(buffer_.data(), length);
                startReading();
            }
        });
}

    void processReceivedData(const char* data, std::size_t length) {
        received_data_.append(data, length);

        while (received_data_.size() >= SHA256_DIGEST_LENGTH) {
            const size_t dataLength = received_data_.size() - SHA256_DIGEST_LENGTH;
            const char* actualData = received_data_.data() + SHA256_DIGEST_LENGTH;

            if (dataLength < 1) {
                break; // Wait for more data.
            }

            unsigned char receivedChecksum[SHA256_DIGEST_LENGTH];
            unsigned char calculatedChecksum[SHA256_DIGEST_LENGTH];

            std::memcpy(receivedChecksum, received_data_.data(), SHA256_DIGEST_LENGTH);
            calculateChecksum(actualData, dataLength, calculatedChecksum);

            if (std::memcmp(receivedChecksum, calculatedChecksum, SHA256_DIGEST_LENGTH) != 0) {
                postLogMessage("ALERT: Checksum mismatch!\n");
                received_data_.clear();
                return;
            }

            // Process the valid data
            clearConsole();
            std::cout.write(actualData, dataLength);
            std::cout.flush();
            std::cout << std::endl;

            // Remove processed data and checksum from the buffer
            received_data_ = received_data_.substr(SHA256_DIGEST_LENGTH + dataLength);
        }
    }

    void scheduleReconnect() {
        // postLogMessage("Scheduling reconnect...\n");
        timer_.expires_after(std::chrono::milliseconds(RECONNECT_INTERVAL_MS));
        timer_.async_wait([this](const boost::system::error_code& ec) {
            if (!ec) {
                connectToServer();
            }
        });
    }

    void postLogMessage(const std::string& message) {
        std::cout << message;
    }

    void clearConsole() {
        std::cout << "\033[2J\033[1;1H";
    }

    boost::asio::io_context& io_context_;
    tcp::socket socket_;
    boost::asio::steady_timer timer_;
    tcp::endpoint server_endpoint_;
    std::array<char, BUFFER_SIZE> buffer_;
    std::string received_data_;
};

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: ./client <server_ip> <server_port>\n";
        return 1;
    }

    std::string serverIP = argv[1];
    int serverPort = 0;

    try {
        serverPort = std::stoi(argv[2]);
        if (serverPort < 1 || serverPort > 65535) {
            throw std::out_of_range("Invalid port range.");
        }
    } catch (...) {
        std::cerr << "Invalid port number. Port must be between 1 and 65535.\n";
        return 1;
    }

    boost::asio::io_context io_context;

    Client client(io_context, serverIP, serverPort);

    io_context.run();

    return 0;
}