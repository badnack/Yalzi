all: bin spec

libraries: lib/cspec/Makefile
	@echo Compiling external libraries...
	$(MAKE) -C lib/cspec/

lib/cspec/Makefile:
	cd lib/cspec && ./autogen.sh && ./configure

bin: libraries
	@echo Compiling binaries...
	$(MAKE) -C src/

spec: libraries bin
	@echo Compiling specs...
	$(MAKE) -C spec/

clean:
	$(MAKE) clean -C spec/
	$(MAKE) clean -C src/
