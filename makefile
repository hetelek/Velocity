QMAKE = qmake-qt4
UNAME = $(shell uname -s)

ifeq ($(UNAME), Linux)
	OS = Linux
else ifeq ($(UNAME), Darwin)
	OS = OSX
else
	OS = Windows
endif

all: release

libXboxInternals: XboxInternals/
	$(QMAKE) XboxInternals/XboxInternals.pro -o XboxInternals/Makefile CONFIG+=$(CONFIG)
	make -C XboxInternals
	mkdir -p XboxInternals-$(OS)/$(CONFIG)
	cp XboxInternals/libXboxInternals.* XboxInternals-$(OS)/$(CONFIG)

velocity: Velocity/
	$(QMAKE) Velocity/Velocity.pro -o Velocity/Makefile CONFIG+=$(CONFIG)
	make -C Velocity

modules: libXboxInternals velocity

debug: CONFIG = debug
debug: modules

release: CONFIG = release
release: modules

clean:
	make clean -C XboxInternals
	rm -f XboxInternals/libXboxInternals.*
	make clean -C Velocity
	rm -f Velocity/Velocity
	rm -rf XboxInternals-*