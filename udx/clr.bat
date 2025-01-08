@echo off

if M%BCLIB% == M goto skipBC
cd bc
make -DNAME=Makefile -DMKFILE=Makefile -fMakefile clean
cd ..
:skipBC

if M%GCCLIB% == M goto skipGCC
cd gcc
make -DNAME=Makefile -DMKFILE=Makefile -fMakefile clean
cd ..
:skipGCC

if M%VCLIB% == M goto skipVC
nmake /f Makefile.vc NAME=Makefile MKFILE=Makefile.vc clean
nmake /f MakefileD.vc NAME=MakefileD MKFILE=MakefileD.vc clean
:skipVC
