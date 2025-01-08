@echo off

if M%BCLIB% == M goto skipBC
cd bc
make -DNAME=Makefile -DMKFILE=Makefile -fMakefile install
cd ..
:skipBC
