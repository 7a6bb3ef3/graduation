#!/bin/bash

git pull https://github.com/nynicg/graduation

g++ main.cpp -o diyusi `pkg-config --libs --cflags opencv`

chmod 755 update.sh
chmod 755 diyusi