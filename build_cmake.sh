#!/bin/bash


BOLD='\033[1m'
BRIGHT_WHITE='\033[1;37m'
RESET='\033[0m' 

BUILD_DIR="build"
BIN_DIR="bin"
BIN_NAME_1="server"
BIN_NAME_2="client"


configure() {
    echo -e "${BOLD}${BRIGHT_WHITE}Create build directories and config cmake${RESET}"
    mkdir -p ${BUILD_DIR}
    cmake -S . -B ${BUILD_DIR} -DCMAKE_VERBOSE_MAKEFILE=ON
}

build() {
    echo -e "${BOLD}${BRIGHT_WHITE}Building project...${RESET}"
    cmake --build ${BUILD_DIR} --target all -- -j$(nproc) --debug
}


./clean_cmake.sh

configure
build

echo ""
echo -e "${BOLD}${BRIGHT_WHITE}bin's should be in $BIN_DIR e.g ${BIN_DIR}/${BIN_NAME_1} && ${BIN_DIR}/${BIN_NAME_2}${RESET}"

echo -e "${BOLD}${BRIGHT_WHITE}E.g:${RESET}"
echo ""
echo -e "${BOLD}${BRIGHT_WHITE}${BIN_DIR}/${BIN_NAME_1}${RESET}"
echo -e "${BOLD}${BRIGHT_WHITE}1. Run the server${RESET}"
echo ""
echo -e "${BOLD}${BRIGHT_WHITE}${BIN_DIR}/${BIN_NAME_2}${RESET}"
echo -e "${BOLD}${BRIGHT_WHITE}2. Run the client to connect${RESET}"
echo ""
echo -e "${BOLD}${BRIGHT_WHITE}3. Edit the input file and safe changes e.g (input/testfile & monitor client window ...${RESET}"
