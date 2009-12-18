all: makefiles
	cd src && $(MAKE) -f Makefile.vc

clean:
	cd src && $(MAKE) -f Makefile.vc clean

cleanall:
	cd src && $(MAKE) -f Makefile.vc MODE=release clean
	cd src && $(MAKE) -f Makefile.vc MODE=debug clean

makefiles:
	cd src && call opp_nmakemake -f --deep
