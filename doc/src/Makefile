#
# Definitions
#
DOXYGEN=doxygen

#
# Targets
#
all: apis do-manual do-userguide do-ide-overview do-ide-customization-guide do-installguide do-ide-developers-guide

apis: api nedxml-api parsim-api

api:
	$(DOXYGEN) api.doxyfile

nedxml-api:
	$(DOXYGEN) nedxml.doxyfile

parsim-api:
	$(DOXYGEN) parsim.doxyfile

do-manual:
	cd manual && $(MAKE)

do-userguide:
	cd userguide && $(MAKE)

do-installguide:
	cd installguide && $(MAKE)

do-ide-customization-guide:
	cd ide-customization-guide && $(MAKE)

do-ide-developers-guide:
	cd ide-developersguide && $(MAKE)

do-ide-overview:
	cd ide-overview && $(MAKE)

do-queues-tutorial:
	cd queues-tutorial && $(MAKE)

clean:
	rm -rf ../api ../nedxml-api ../parsim-api ../manual ../userguide ../installguide ../ide-customization-guide ../ide-overview ../*.pdf
	cd manual && $(MAKE) clean
	cd userguide && $(MAKE) clean
	cd installguide && $(MAKE) clean
	cd ide-customization-guide && $(MAKE) clean
	cd ide-developersguide && $(MAKE) clean
	cd ide-overview && $(MAKE) clean
