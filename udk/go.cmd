@echo off
g++ -I ~/include -std=c++11 -MM *cpp > udk.d
make -f Makefile.gmk install
