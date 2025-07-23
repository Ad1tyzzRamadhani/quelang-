#!/bin/bash
echo "🔧 Building QueLang compiler..."
g++ -std=c++17 main.cpp -o quelang
chmod +x quelang 

if [ $? -eq 0 ]; then
    echo "✅ Build succeeded: ./quelang"
    echo " "
    echo "=== quelang compiler ==="
    echo "use :"
    echo " ./quelang input.q output.s"
    echo "for debug :"
    echo " ./quelang --debug input.q output.s"
    echo " "
    echo "qc0.7 --Alpha"
else
    echo "EROR !"
    echo "❌ Build failed!"
    echo "Check https://github.com/Ad1tyzzRamadhani/quelang-"
    echo "For Latest Fix Info"
    exit 1
fi
