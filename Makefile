# ALIASES
EXTLIB_CONF=lib/cspec/Makefile

# SHELL COLORS
GREEN_BG="\033[0;42m"
BLUE_BG="\033[0;44m"
NO_COLOR="\033[00m"


all: libraries spec ylz
	@echo $(GREEN_BG)DONE!$(NO_COLOR)

ylz:
	@echo $(BLUE_BG)Compiling binaries...$(NO_COLOR)
	$(MAKE) -C src/

libraries: $(EXTLIB_CONF)
	@echo $(BLUE_BG)Compiling external libraries...$(NO_COLOR)
	$(MAKE) -C lib/cspec/

$(EXTLIB_CONF):
	@echo $(BLUE_BG)Configuring external libraries...$(NO_COLOR)
	cd lib/cspec && ./autogen.sh && ./configure

spec: libraries ylz
	@echo $(BLUE_BG)Compiling spec...$(NO_COLOR)
	$(MAKE) -C spec/

run_spec:
	@echo $(BLUE_BG)Executing spec...$(NO_COLOR)
	@$(MAKE) run -s -C spec/

clean:
	$(MAKE) clean -C spec/
	$(MAKE) clean -C src/
	$(MAKE) clean -C lib/cspec/

fixme:
	@grep -riIn FIXME src/ spec/
