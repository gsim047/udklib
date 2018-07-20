.autodepend

!ifndef NAME
!error "NAME is not defined !"
!endif

!ifndef MKFILE
!error "MKFILE is not defined !"
!endif

LIBPATH = $(BCB)\lib;$(BCLIB)

CC=bcc32
CFLAGS=-tWM -5
CF_C=-c
CF_E=-e

LINK=ilink32
LFLAGS = -D"" -ap -Tpe -x -Gn -v

RCC=brcc32
ARC=$(NAME)_src
RC=$(NAME).rc
RES=$(NAME).res
LIBS=cMain.lib udk.lib 
LLIBS=


OBJS=\
	$(NAME).obj


all: $(NAME).exe $(OBJS) $(ARC).rar

#$(NAME).exe : $(OBJS) $(NAME).res $(LLIBS) $(MKFILE)
#	$(CC) $(CFLAGS) $(CF_E)$(NAME) $(OBJS)  $(LIBS) $(LLIBS)
#	@upx $(NAME).exe

.cpp.obj:
	$(CC) $(CFLAGS) $(CF_C) $<

.c.obj:
	$(CC) $(CFLAGS) $(CF_C) $<

$(ARC).rar : $(OBJS) $(MKFILE)
	-1 @rar u $(ARC) *.h *.cpp *.c *.mak

.rc.res:
	$(RCC) -fo$@ $<

# ---------------------------------------------------------------------------
$(NAME).exe: $(OBJS) $(MKFILE)
	vinc $(RC)
	$(RCC) -fo$(RES) $(RC)
	$(LINK) @&&!
	$(LFLAGS) -L$(LIBPATH) +
	c0x32.obj $(OBJS), +
	$(NAME).exe,, +
	$(LIBS) $(LLIBS) import32.lib cw32mt.lib, +
	, +
	$(RES)
!
	@upx $(NAME).exe
# ---------------------------------------------------------------------------
