#!/bin/bash

echo "Installing required dependencies for DebForge..."

# Update package list
sudo apt-get update

# Install required packages
sudo apt-get install -y \
    build-essential \
    cmake \
    libreadline-dev

echo "✅ All dependencies installed successfully!"
echo "You can now build the project with:"
echo "  mkdir -p build && cd build"
echo "  cmake .."
echo "  make"
echo "  sudo make install"
