CD = cd
CP = cp -u

EXE = ducks.exe

all: .symbolic
	@$(CD) src
	@$(MAKE) $@
	@$(CD) ..
	@$(CP) src/$(EXE) .

clean: .symbolic
	@$(CD) src
	@$(MAKE) $@
	@$(CD) ..
