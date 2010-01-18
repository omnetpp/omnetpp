#
# Definitions
#
DOXYGEN=doxygen

#
# Targets
#
all: apis do-tictoc-tutorial do-manual do-userguide do-migrationguide do-ide-overview do-ide-customization-guide do-ide-develpers-guide

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

do-ide-customization-guide:
	cd ide-customization-guide && $(MAKE)

do-ide-developers-guide:
	cd IDE-DevelopersGuide && $(MAKE)

do-ide-overview:
	cd ide-overview && $(MAKE)

do-tictoc-tutorial:
	cd tictoc-tutorial && $(MAKE)

do-queues-tutorial:
	cd queues-tutorial && $(MAKE)

clean:
	rm -rf ../api ../nedxml-api ../parsim-api ../tictoc-tutorial
	cd manual && $(MAKE) clean
	cd userguide && $(MAKE) clean
	cd migrationguide && $(MAKE) clean
	cd ide-customization-guide && $(MAKE) clean
	cd ide-overview && $(MAKE) clean
	cd tictoc-tutorial && $(MAKE) clean
