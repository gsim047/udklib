# /bin/sh

if [[ !(-e udk.d) ]]
then
	g++ -I ~/include -std=c++11 -MM *cpp > udk.d
fi

make -f Makefile.gmk install
