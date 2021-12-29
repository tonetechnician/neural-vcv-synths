#!/bin/bash

echo "Initializing environment"

LIBTORCH_VER="1.10.1"

if [[ "$OSTYPE" == "darwin"* ]] ; then
  echo "Set up darwin"
  echo "Downloading libtorch shared libraries"
  wget https://download.pytorch.org/libtorch/cpu/libtorch-macos-$LIBTORCH_VER.zip -P ./tmp
  unzip ./tmp/libtorch-macos-$LIBTORCH_VER.zip -d ./Nash-Suite/dep/libtorch
elif [[ "$OSTYPE" == "linux-gnu"* ]] ; then
  echo "Set up linux"
  echo "Downloading libtorch shared librariess with cxx11 ABI"
  wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-$LIBTORCH_VER%2Bcpu.zip -P ./tmp
  unzip ./tmp/libtorch-cxx11-abi-shared-with-deps-$LIBTORCH_VER+cpu.zip -d ./Nash-Suite/dep
fi