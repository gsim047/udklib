# This file was created by MkExe ver. %VER% [%DATE% %TIME%]

NAME=%NAME%
MKFILE=$(NAME).gmk

CC=g++
RCC=windres
CPPFLAGS=-s -I ~/include -std=c++11
CFLAGS=$(CPPFLAGS)
RC=$(NAME).rc
CFG=$(NAME).cfg

UNAME_M = $(shell uname -m)
UNAME_O = $(shell uname -o)

ifeq ($(UNAME_O),Cygwin)
TARGET=$(NAME).exe
RES=$(NAME)_res.o
LIBS=-L ~/lib %LIB% -lversion
RCCCMD=$(RCC) -o $(RES) $(RC)
else
TARGET=$(NAME)
RES=$(NAME).res
LIBS=-L ~/lib %LIB%
ifeq ($(UNAME_M),x86_64)
RCCCMD=objcopy -I binary -O elf64-x86-64 -B i386 $(RC) $(RES)
else
RCCCMD=objcopy -I binary -O elf32-i386 -B i386 $(RC) $(RES)
endif
endif

ARC=$(NAME)_src
LLIBS=%LLIB%

MM=$(NAME).d

OBJS=%OBJ%
#	$(NAME).o


all: $(TARGET) $(OBJS) %ARC% $(ARC).rar

$(TARGET) : $(OBJS) $(RES) $(LLIBS) $(MKFILE)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(RES) $(LIBS) $(LLIBS)

#include $(MM)

%.o : %.cpp
	$(CC) $(CPPFLAGS) -c -o $@ $<

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<


$(RES) : $(RC) $(OBJS) $(MKFILE)
	vinc $(RC)
	$(RCCCMD)

$(ARC).rar : $(OBJS) $(MKFILE)
	-1 @rar u $(ARC) *.h *.cpp *.c *.rc *.mak *.def *.gmk
# cp $(ARC).rar \Doc\h

clean:
	-rm $(TARGET)
	-rm $(OBJS)
	-rm $(RES)


install: all copy

copy: all
	cp -v -u $(TARGET) ~/bin
	cp -v -u $(CFG) ~/bin


#$(MM): *.cpp *.h $(MKFILE)
#	$(CC) $(CFLAGS) -MM *.cpp *.h > $(MM)
