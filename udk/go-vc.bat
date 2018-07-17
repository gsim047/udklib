@echo off

if M%BCLIB% == M goto skipBC
make -DNAME=Makefile -DMKFILE=Makefile -fMakefile.bc install
:skipBC

if M%GCCLIB% == M goto skipGCC
cd gcc
make -DNAME=Makefile -DMKFILE=Makefile -fMakefile install
cd ..
:skipGCC

if M%VCLIB% == M goto skipVC
nmake /f Makefile.vc NAME=Makefile MKFILE=Makefile.vc install
nmake /f MakefileD.vc NAME=MakefileD MKFILE=MakefileD.vc install
:skipVC
