#!/bin/sh

g++ -I ~/include -std=c++11 -MM *cpp > cMain.d
make -f Makefile.gmk install
