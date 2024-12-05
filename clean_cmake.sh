#!/bin/bash

# Define directories
BUILD_DIR=build
BIN_DIR=bin

# Clean build and bin directories
clean() {
    echo "Cleaning build and bin directories..."
    rm -rf ${BUILD_DIR} ${BIN_DIR}
}

# Main function
clean