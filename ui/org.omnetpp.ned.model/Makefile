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
	$(PERL) etc/dtdclassgen.pl $(OMNETPP_ROOT)/doc/ned2/ned2.dtd $(BASEPKG) $(JMODELPKG) $(JMODELDIR)

depend:

clean:
	-rm -f $(JMODELDIR)/*.java

# DO NOT DELETE THIS LINE -- make depend depends on it.

