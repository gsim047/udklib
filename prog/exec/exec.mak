# This file was created by MkExe ver. 1.9.19 [2004-12-01 15:14:23]

!ifndef NAME
!error "NAME is not defined !"
!endif

!ifndef MKFILE
!error "MKFILE is not defined !"
!endif

CC=cl
CFLAGS=/MT /W3 /O2 /Gi /G5 /GX /GR
CF_C=/c
CF_E=/Fe

RCC=rc

LINK=link
LFLAGS = -D"" -ap -Tpe -x -Gn -v

ARC=$(NAME)_src
RC=$(NAME).rc
RES=$(NAME).res
LIBS=udk.lib version.lib kernel32.lib advapi32.lib shell32.lib user32.lib
#cMain.lib udk.lib 
LLIBS=


OBJS=\
	$(NAME).obj

#	$(NAME).obj


all: $(NAME).exe $(OBJS) # $(ARC).rar

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
	-1 @rar u $(ARC) *.h *.cpp *.c *.rc $(MKFILE)

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
