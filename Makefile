EXTLIB_CONF=lib/cspec/Makefile

# SHELL COLORS
BLUE_BG="\033[0;44m"
NO_COLOR="\033[00m"

all: bin spec
	@echo $?

libraries: $(EXTLIB_CONF)
	@echo $(BLUE_BG)Compiling external libraries...$(NO_COLOR)
	$(MAKE) -C lib/cspec/

$(EXTLIB_CONF):
	@echo $(BLUE_BG)Configuring external libraries...$(NO_COLOR)
	cd lib/cspec && ./autogen.sh && ./configure

bin: libraries
	@echo $(BLUE_BG)Compiling binaries...$(NO_COLOR)
	$(MAKE) -C src/

spec: libraries bin
	@echo $(BLUE_BG)Compiling specs...$(NO_COLOR)
	$(MAKE) -C spec/

clean:
	$(MAKE) clean -C spec/
	$(MAKE) clean -C src/
