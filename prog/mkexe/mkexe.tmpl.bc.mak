# This file was created by MkExe ver. %VER% [%DATE% %TIME%]
.autodepend

!ifndef NAME
!error "NAME is not defined !"
!endif

!ifndef MKFILE
!error "MKFILE is not defined !"
!endif

LIBPATH = $(BCB)\lib;$(BCLIB)

CC = bcc32
%EXE%CFLAGS = -tWM -5
%DLL%CFLAGS = -tWD -tWM -5
#-vG
CF_C = -c
CF_E = -e

LINK = ilink32
%EXE%LFLAGS = -D"" -ap -Tpe -x -Gn -v
%DLL%LFLAGS = -D"" -aa -Tpd -x -Gn -v

RCC = brcc32
ARC = $(NAME)_src
RC = $(NAME).rc
RES = $(NAME).res
%EXE%OBJ0 = c0x32.obj
%DLL%OBJ0 = c0d32.obj
%EXE%TARGET = $(NAME).exe
%DLL%TARGET = $(NAME).dll
%EXE%LIBS = cMain.lib %LIB%
%DLL%LIBS = dMain.lib %LIB%
#cMain.lib udk.lib 
#cg32.lib
LLIBS = %LLIB%

OBJS = %OBJ%
#	$(NAME).obj


%EXE%%ARC%all: $(TARGET) $(OBJS) $(ARC).rar
%EXE%%NOARC%all: $(TARGET) $(OBJS)
%DLL%%ARC%all: $(TARGET) $(OBJS) $(NAME).lib $(NAME).def $(ARC).rar
%DLL%%NOARC%all: $(TARGET) $(OBJS) $(NAME).lib $(NAME).def

#$(NAME).exe : $(OBJS) $(NAME).res $(LLIBS) $(MKFILE)
#	$(CC) $(CFLAGS) $(CF_E)$(NAME) $(OBJS)  $(LIBS) $(LLIBS)
#	@upx $(NAME).exe

.cpp.obj:
	$(CC) $(CFLAGS) $(CF_C) $<

#.c.obj:
#	$(CC) $(CFLAGS) $(CF_C) $<

$(ARC).rar : $(OBJS) $(MKFILE)
	-1 @rar u $(ARC) *.h *.cpp *.c *.rc $(MKFILE)

.rc.res:
	$(RCC) -fo$@ $<

# ---------------------------------------------------------------------------
$(TARGET): $(OBJS) $(NAME).res $(LLIBS) $(MKFILE)
	vinc $(RC)
	$(RCC) -fo$(RES) $(RC)
	$(LINK) @&&!
	$(LFLAGS) -L$(LIBPATH) +
	$(OBJ0) $(OBJS), +
	$(TARGET),, +
	$(LIBS) $(LLIBS) import32.lib cw32mt.lib, +
	, +
	$(RES)
!
%EXE%%PACK%	@upx $(NAME).exe
# ---------------------------------------------------------------------------

%DLL%$(NAME).lib: $(NAME).dll
%DLL%	implib $(NAME) $(NAME).dll

%DLL%$(NAME).def: $(NAME).dll
%DLL%	impdef $(NAME) $(NAME).dll
