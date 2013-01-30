CC = wcc386
LD = wlink

CP = cp
RM = rm

EXE = ducks.exe

CFLAGS = -bt=dos -za99 -6r -mf -oahkl+tx -fo=$^*.obj
LFLAGS = @ducks_c.lnk

OBJS = dpmi.obj error.obj image.obj main.obj mouse.obj pcx.obj timer.obj &
       vbe.obj vga.obj xmalloc.obj

.suffixes:
.suffixes: .exe .obj .c .h

.c.obj: .autodepend
	$(CC) $(CFLAGS) $<

all: $(EXE) .symbolic

$(EXE): $(OBJS)
	$(LD) $(LFLAGS)
	$(CP) $(EXE) ..

clean: .symbolic
	$(RM) *.obj
	$(RM) *.map
	$(RM) $(EXE)
