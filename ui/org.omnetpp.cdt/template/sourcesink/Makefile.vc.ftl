<#include "main.fti">
<#if srcFolder!=".">
all: checkmakefiles
	cd ${srcFolder} && $(MAKE) -f Makefile.vc

clean: checkmakefiles
	cd ${srcFolder} && $(MAKE) -f Makefile.vc clean

cleanall: checkmakefiles
	cd ${srcFolder} && $(MAKE) -f Makefile.vc MODE=release clean
	cd ${srcFolder} && $(MAKE) -f Makefile.vc MODE=debug clean

makefiles:
	cd ${srcFolder} && call opp_nmakemake -f --deep

checkmakefiles:
	@if not exist ${srcFolder}\Makefile.vc ( \
	echo. && \
	echo ============================================================================ && \
	echo ${srcFolder}/Makefile.vc does not exist. Please use the following command to generate it: && \
	echo nmake -f Makefile.vc makefiles && \
	echo ============================================================================ && \
	echo. && \
	exit 1 )

</#if>
