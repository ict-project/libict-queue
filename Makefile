all: build/.dir
	$(MAKE) $(MFLAGS) -C build

clean: build/.dir
	$(MAKE) $(MFLAGS) -C build clean

install: build/.dir
	$(MAKE) $(MFLAGS) -C build install

uninstall: build/.dir
	$(MAKE) $(MFLAGS) -C build uninstall

test: build/.dir
	$(MAKE) $(MFLAGS) -C build test

build/.dir:
	test -d build || ./build.linux.bsh
	echo > build/.dir
