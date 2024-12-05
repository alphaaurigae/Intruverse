# Intruverse 
> Continuous connection datatransfer between server and client(s) ... 

- server read and provide file content to clients.
- clients receive file content and print to console while update on changes.

 
## Client
```
 $ bin/client 127.0.0.1 12345
Attempting to connect to server...
Connected to the server.
```
(stream starts and file content shows, updated on the fly)

### Techniques

Establishing a TCP connection to the server.

Reading data from the server asynchronously.

Verifying checksum to ensure data integrity.

Clearing the console before writing new data.

## Server

```
$ bin/server input/testfile 
sendFileContent: Starting the process of sending file content.
sendFileContent: File opened successfully.
sendFileContent: Read 22 bytes from file.
sendFileContent: Calculating checksum for the current chunk.
sendFileContent: Checksum calculated.
sendFileContent: Write succeeded.
sendFileContent: File closed after all chunks were sent.
sendFileContent: Starting the process of sending file content.
sendFileContent: File opened successfully.
```

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

Test OS `ubuntu 24.04`

cmake ...
`./build_cmake.sh`
`./clean_cmake.sh`