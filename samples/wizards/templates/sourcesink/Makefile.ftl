all: makefiles
	cd src && $(MAKE)

clean:
	cd src && $(MAKE) clean

cleanall:
	cd src && $(MAKE) MODE=release clean
	cd src && $(MAKE) MODE=debug clean

makefiles:
	cd src && opp_makemake -f --deep
