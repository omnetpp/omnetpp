<#if srcFolder!=".">
all: checkmakefiles
	cd ${srcFolder} && $(MAKE)

clean: checkmakefiles
	cd ${srcFolder} && $(MAKE) clean

cleanall: checkmakefiles
	cd ${srcFolder} && $(MAKE) MODE=release clean
	cd ${srcFolder} && $(MAKE) MODE=debug clean
	rm -f ${srcFolder}/Makefile

makefiles:
	cd ${srcFolder} && opp_makemake -f --deep

checkmakefiles:
	@if [ ! -f ${srcFolder}/Makefile ]; then \
	echo; \
	echo '======================================================================='; \
	echo '${srcFolder}/Makefile does not exist. Please use "make makefiles" to generate it!'; \
	echo '======================================================================='; \
	echo; \
	exit 1; \
	fi
</#if>
