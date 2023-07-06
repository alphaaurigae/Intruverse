# Intruverse 
> Was bored. . micro project for fun ... server unit for continuous connection datatransfer between server and client(s) (hey i can see a file and changes on it applied in realtime on a remote box - thats leet :)

This program is a mock-up test for a client-server continuous connection to transfer data without interruption.

... reconnection ability missing, nothing encrypted, no multithreading, no security tests, no menu, no warning or error handling, no unit tests (lol), no optimization nor any "beautiful" code lol

## Client
### Libraries

`iostream`: Input/output stream handling.

`cstring`: String manipulation and memory operations.

`uv.h`: Libuv library for asynchronous event-driven programming.

`openssl/sha.h`: OpenSSL library for SHA checksum calculation.

`openssl/evp.h`: OpenSSL library for encryption and decryption.

### Techniques

Establishing a TCP connection to the server.

Reading data from the server asynchronously.

Verifying checksum to ensure data integrity.

Clearing the console before writing new data.

## Server
### Libraries

`iostream`: Input/output stream handling.
    
`fstream`: File stream handling.
    
`uv.h`: Libuv library for asynchronous event-driven programming.
    
`vector`: Dynamic array data structure.
    
`openssl/sha.h`: OpenSSL library for SHA checksum calculation.
    
`openssl/ssl.h`, openssl/err.h, openssl/evp.h: OpenSSL library for encryption and decryption.

### Techniques

Setting up a TCP server.

Accepting and handling multiple client connections.
    
Sending file content to connected clients.

Calculating and sending checksum for data integrity.

Monitoring file changes and updating clients.

## USE

` ./server <file_path>`

` ./client 127.0.0.1 8000`

## BUILD

`g++ client.cpp calculateChecksum.cpp -o client -luv -lssl -lcrypto`

`g++ server.cpp calculateChecksum.cpp -o server -luv -lssl -lcrypto`

