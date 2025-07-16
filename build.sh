#!/bin/bash

# Build script for QueLang
# Requires: g++, C++17

echo "🔧 Building QueLang compiler..."
g++ -std=c++17 main.cpp -o quelang
g++ -std=c++17 dash.cpp -o dash
chmod +x quelang 
chmod +x dash

if [ $? -eq 0 ]; then
    echo "✅ Build succeeded: ./quelang"
    ./dash
else
    echo "❌ Build failed!"
    exit 1
fi
