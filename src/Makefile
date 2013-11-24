CC = wcc386
LD = wlink

RM = rm -f

EXE = ducks.exe

CFLAGS = -bt=dos -za99 -6r -mf -oahkl+tx -fo=$^*.obj
LFLAGS = @ducks_c.lnk

OBJS = alloc.obj dpmi.obj error.obj image.obj main.obj mouse.obj palette.obj pcx.obj timer.obj vbe.obj vga.obj xmalloc.obj xmemcpy.obj

.suffixes:
.suffixes: .exe .obj .c .h

.c.obj: .autodepend
	$(CC) $(CFLAGS) $<

all: $(EXE) .symbolic

$(EXE): $(OBJS)
	$(LD) $(LFLAGS)

clean: .symbolic
	$(RM) *.obj
	$(RM) *.map
	$(RM) $(EXE)
