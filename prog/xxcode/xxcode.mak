# This file was created by MkExe ver. 1.9.19 [2008-10-15 15:53:22]

!ifndef NAME
!error "NAME is not defined !"
!endif

!ifndef MKFILE
!error "MKFILE is not defined !"
!endif

CC=cl
CFLAGS=/MT /W3 /O2 /GR /EHsc
#CFLAGS=/MT /W3 /O2 /Gi /G5 /GX /GR /GD /LD
CF_C=/c
CF_E=/Fe

RCC=rc

LINK=link
LFLAGS = -D"" -ap -Tpe -x -Gn -v

ARC=$(NAME)_src
RC=$(NAME).rc
RES=$(NAME).res
LIBS=cMain.lib cMain.lib udk.lib version.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib urlmon.lib 
#LIBS=dMain.lib cMain.lib udk.lib version.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib urlmon.lib 
#cMain.lib udk.lib 
LLIBS=


OBJS=\
	$(NAME).obj

#	$(NAME).obj


all: $(NAME).exe $(OBJS) $(ARC).rar

$(NAME).exe : $(OBJS) $(LLIBS) $(MKFILE) $(RC)
	vinc $(RC)
	$(RCC) $(RC)
	$(CC) $(CFLAGS) $(CF_E)$(NAME) $(OBJS) $(LIBS) $(LLIBS) $(RES)
#	@upx $(NAME).exe

.cpp.obj:
	$(CC) $(CFLAGS) $(CF_C) $<

.c.obj:
	$(CC) $(CFLAGS) $(CF_C) $<

$(ARC).rar : $(OBJS) $(MKFILE)
	-1 @rar u $(ARC) *.h *.cpp *.c *.rc *.mak
# cp $(ARC).rar \Doc\h

.rc.res:
	$(RCC) $<

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
##	@upx $(NAME).exe
# ---------------------------------------------------------------------------
