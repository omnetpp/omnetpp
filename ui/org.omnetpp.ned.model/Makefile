#
# Global definitions
#
include ../../Makefile.inc

#
# Local definitions
#
BASEPKG = org.omnetpp.ned.model
JMODELPKG = $(BASEPKG).pojo
JMODELDIR = src/$(subst .,/,$(JMODELPKG))

#
# Targets
#
all: genjava

genjava:
	mkdir -p $(JMODELDIR)
	$(PERL) etc/dtdclassgen.pl $(OMNETPP_ROOT)/doc/etc/ned2.dtd $(BASEPKG) $(JMODELPKG) $(JMODELDIR)

clean:
	-rm -rf $(JMODELDIR)

