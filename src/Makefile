CC = wcc386
LD = wlink

RM = rm -f
CP = cp -u

CFLAGS = -bt=dos -za99 -6r -mf -oahkl+tx -fo=$^*.obj
LFLAGS = system dos32x option map

OBJS =  alloc.obj   &
        dpmi.obj    &
        duck.obj    &
        error.obj   &
        game.obj    &
        gfx.obj     &
        image.obj   &
        list.obj    &
        main.obj    &
        mouse.obj   &
        palette.obj &
        pcx.obj     &
        res.obj     &
        scene.obj   &
        sprite.obj  &
        timer.obj   &
        vbe.obj     &
        vga.obj     &
        xmalloc.obj &
        xmemcpy.obj

.suffixes:
.suffixes: .exe .obj .c .h

.c.obj: .autodepend
	$(CC) $(CFLAGS) $<

ducks.exe: $(OBJS)
	$(LD) $(LFLAGS) @$^*
	$(CP) $^@ ..

vbeinfo.exe: vbeinfo.obj dpmi.obj error.obj vbe.obj
        $(LD) $(LFLAGS) @$^*

all: ducks.exe vbeinfo.exe .symbolic

clean: .symbolic
	$(RM) *.obj
	$(RM) *.map
	$(RM) ducks.exe
	$(RM) vbeinfo.exe
