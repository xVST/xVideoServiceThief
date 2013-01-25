#!/bin/bash

# change to script directory
cd `dirname "$0"`

# precompile
g++ -c main.cpp -o main.o
g++ -c ../pakunpak.cpp -o pakunpak.o

# generate application
g++ main.o pakunpak.o -o pakunpak

# remove precompiled files
rm main.o
rm pakunpak.o