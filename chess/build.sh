#!/usr/bin/env bash
set +x

DIR=`dirname "$0"`

if which g++ > /dev/null; then
    echo "Building..."
    /usr/bin/g++ -fdiagnostics-color=always -g $DIR/engine/base_engine.cpp -o $DIR/engine/base_engine -std=c++17 -O3 -march=native
    echo
    echo "Success!"
    echo
    echo "Run '$DIR/engine/base_engine' and type (w/ ENTER after each line):"
    echo "uci"
    echo "ucinewgame"
    echo "isready"
    echo "position startpos moves e2e4"
    echo "go"
    echo "quit"
else
    echo "You need g++ to build the engine"
    exit 1
fi
