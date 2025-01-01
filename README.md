# Intruverse 
> Continuous connection datatransfer between server and client(s) ... 

- server read and provide file content to clients.
- clients receive file content and print to console while update on changes.


## Deps
```
- libuv1-dev - https://github.com/libuv/libuv
- libboost-all-dev
```
## Build

- Build OS: "Ubuntu 24.04"

... cmake ...
(g++/clang - Cmakelists "option(USE_CLANG" ON/OFF - default clang)

- ```./build_cmake.sh``` - build
- ```./clean_cmake.sh``` - tidy build && dirs


## Client

> Connect, receive, verify, clear for new data

```
$ bin/client 127.0.0.1 12345
Attempting to connect to server...
Connected to the server.
```
```
(stream starts and file content shows, updated periodically (int main server.cpp))```
```

## Server

> Accept connect, checksum, push data, wait for data update, repeat.

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

## Use

` ./server <file_path>` (port hardcoded sever.cpp int main)

` ./client 127.0.0.1 12345` 


### Todo
- e.g "streaming" && timed read as options
- sec .. tidy, format.
