<#include "main.fti">
<#if srcFolder!=".">
all: makefiles
	cd ${srcFolder} && $(MAKE)

clean:
	cd ${srcFolder} && $(MAKE) clean

cleanall:
	cd ${srcFolder} && $(MAKE) MODE=release clean
	cd ${srcFolder} && $(MAKE) MODE=debug clean

makefiles:
	cd ${srcFolder} && opp_makemake -f --deep
</#if>
