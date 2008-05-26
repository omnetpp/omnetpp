#
# Definitions
#
DOXYGEN=doxygen

#
# Targets
#
all: apis do-tictoc-tutorial do-manual do-userguide

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

do-tictoc-tutorial:
	cd tictoc-tutorial && $(MAKE)

do-queues-tutorial:
	cd queues-tutorial && $(MAKE)

clean:
	rm -rf ../api ../nedxml-api ../parsim-api ../tictoc-tutorial
	cd manual && $(MAKE) clean
	cd userguide && $(MAKE) clean
