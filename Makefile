QMAKE = qmake6

# Detect the operating system
ifeq ($(OS),Windows_NT)
    OS := Win
    MAKE_CMD := mingw32-make
else
    UNAME := $(shell uname -s)
    ifeq ($(UNAME),Linux)
        OS := Linux
    else ifeq ($(UNAME),Darwin)
        OS := OSX
    endif
    MAKE_CMD := make
endif

CONFIG ?= release

all: debug

libXboxInternals: XboxInternals
	$(QMAKE) XboxInternals/XboxInternals.pro -o XboxInternals/Makefile CONFIG+=$(CONFIG)
	$(MAKE_CMD) -C XboxInternals

velocity_target: Velocity
	$(QMAKE) Velocity/Velocity.pro -o Velocity/Makefile CONFIG+=$(CONFIG)
	$(MAKE_CMD) -C Velocity

modules: libXboxInternals velocity_target

debug: CONFIG = debug
debug: modules

release: CONFIG = release
release: modules

clean:
	$(MAKE_CMD) clean -C XboxInternals
	$(MAKE_CMD) clean -C Velocity
ifeq ($(OS),Win)
	@if exist XboxInternals\$(CONFIG)\libXBoxInternals.* del /Q XboxInternals\$(CONFIG)\libXBoxInternals.*
	@if exist Velocity\Velocity del /Q Velocity\Velocity
	@for /D %%d in (XboxInternals-*) do if exist "%%d" rmdir /S /Q "%%d"
else
	rm -f XboxInternals/$(CONFIG)/libXBoxInternals.*
	rm -f Velocity/Velocity
	rm -rf XboxInternals-*
endif
