# This file was created by MkExe ver. %VER% [%DATE% %TIME%]

!ifndef NAME
!error "NAME is not defined !"
!endif

!ifndef MKFILE
!error "MKFILE is not defined !"
!endif

CC=cl
%EXE%CFLAGS=/MT /W3 /O2 /EHsc /GR
%DLL%CFLAGS=/MT /W3 /O2 /EHsc /GR /LD

CF_C=/c
CF_E=/Fe

RCC=rc

LINK=link
LFLAGS = -D"" -ap -Tpe -x -Gn -v


%EXE%TARGET=$(NAME).exe
%DLL%TARGET=$(NAME).dll
ARC=$(NAME)_src
RC=$(NAME).rc
RES=$(NAME).res
%DLL%DEF=$(NAME).def
%EXE%LIBS=cMain.lib %LIB%
%DLL%LIBS=dMain.lib %LIB%
#cMain.lib udk.lib 
LLIBS=%LLIB%


OBJS=%OBJ%
#	$(NAME).obj


all: $(TARGET) $(OBJS) %ARC% $(ARC).rar

%EXE%$(TARGET) : $(OBJS) $(LLIBS) $(MKFILE) $(RC)
%DLL%$(TARGET) : $(OBJS) $(LLIBS) $(MKFILE) $(RC) $(DEF)
	vinc $(RC)
	$(RCC) $(RC)
	$(CC) $(CFLAGS) $(CF_E)$(NAME) $(OBJS) $(LIBS) $(LLIBS) $(RES)
#	@upx $(NAME).exe

.cpp.obj:
	$(CC) $(CFLAGS) $(CF_C) $<

.c.obj:
	$(CC) $(CFLAGS) $(CF_C) $<

$(ARC).rar : $(OBJS) $(MKFILE)
	-1 @rar u $(ARC) *.h *.cpp *.c *.rc *.mak *.def
# cp $(ARC).rar \Doc\h

.rc.res:
	$(RCC) $<

clean:
	del $(NAME).exe
	del $(OBJS)
	del $(RES)

# ---------------------------------------------------------------------------
#$(NAME).exe: $(OBJS)
#	vinc $(RC)
#	$(RCC) $(RC)
#	$(LINK) @&&!
#	$(LFLAGS) -L$(LIBPATH) +
#	c0x32.obj $(OBJS), +
#	$(NAME).exe,, +
#	$(LIBS) $(LLIBS) import32.lib cw32mt.lib, +
#	, +
#	$(RES)
#!
#%PACK%	@upx $(NAME).exe
# ---------------------------------------------------------------------------
