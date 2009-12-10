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
	$(PERL) etc/dtdclassgen.pl $(OMNETPP_ROOT)/doc/ned2/ned2.dtd $(BASEPKG) $(JMODELPKG) $(JMODELDIR)

depend:

clean:
	-rm -rf $(JMODELDIR)

# DO NOT DELETE THIS LINE -- make depend depends on it.

