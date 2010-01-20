#
# Definitions
#
DOXYGEN=doxygen

#
# Targets
#
all: apis do-tictoc-tutorial do-manual do-userguide do-migrationguide do-ide-overview do-ide-customization-guide do-installguide

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

do-migrationguide:
	cd migrationguide && $(MAKE)

do-installguide:
	cd installguide && $(MAKE)

do-ide-customization-guide:
	cd ide-customization-guide && $(MAKE)

do-ide-overview:
	cd ide-overview && $(MAKE)

do-tictoc-tutorial:
	cd tictoc-tutorial && $(MAKE)

do-queues-tutorial:
	cd queues-tutorial && $(MAKE)

clean:
	rm -rf ../api ../nedxml-api ../parsim-api ../tictoc-tutorial ../manual ../userguide ../migrationguide ../installguide ../ide-customization-guide ../ide-overview ../*.pdf
	cd manual && $(MAKE) clean
	cd userguide && $(MAKE) clean
	cd migrationguide && $(MAKE) clean
	cd installguide && $(MAKE) clean
	cd ide-customization-guide && $(MAKE) clean
	cd ide-overview && $(MAKE) clean
	cd tictoc-tutorial && $(MAKE) clean
