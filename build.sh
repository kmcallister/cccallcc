#!/bin/sh -ex

compile() {
    g++ -Wall --std=c++0x -o $1 $1.cpp
}

compile simple
compile backtrack
