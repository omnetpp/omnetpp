<#include "main.fti">
<#if srcFolder!=".">
all: makefiles
	cd ${srcFolder} && $(MAKE) -f Makefile.vc

clean:
	cd ${srcFolder} && $(MAKE) -f Makefile.vc clean

cleanall:
	cd ${srcFolder} && $(MAKE) -f Makefile.vc MODE=release clean
	cd ${srcFolder} && $(MAKE) -f Makefile.vc MODE=debug clean

makefiles:
	cd ${srcFolder} && call opp_nmakemake -f --deep
</#if>
