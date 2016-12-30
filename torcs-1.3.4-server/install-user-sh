#!/bin/bash
# Use this script to install in a user folder instead of a system folder,
# feel free to modify it
set -o xtrace
./configure --prefix=$(pwd)/../torcs-1.3.4-server-bin
make
make install
make datainstall
