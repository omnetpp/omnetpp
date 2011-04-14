all: checkmakefiles
	cd src && $(MAKE) -f Makefile.vc

clean: checkmakefiles
	cd src && $(MAKE) -f Makefile.vc clean

cleanall: checkmakefiles
	cd src && $(MAKE) -f Makefile.vc MODE=release clean
	cd src && $(MAKE) -f Makefile.vc MODE=debug clean

makefiles:
	cd src && call opp_nmakemake -f --deep

checkmakefiles:
	@if not exist src\Makefile.vc ( \
	echo. && \
	echo ============================================================================ && \
	echo src/Makefile.vc does not exist. Please use the following command to generate it: && \
	echo nmake -f Makefile.vc makefiles && \
	echo ============================================================================ && \
	echo. && \
	exit 1 )

