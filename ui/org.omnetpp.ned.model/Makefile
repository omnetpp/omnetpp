#
# Global definitions
#

CONFIGFILE = ../../Makefile.inc
include $(CONFIGFILE)

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

genjava: $(CONFIGFILE)
	mkdir -p $(JMODELDIR)
	$(PERL) etc/dtdclassgen.pl etc/all-in-one.dtd $(BASEPKG) $(JMODELPKG) $(JMODELDIR)

clean:
	-rm -rf $(JMODELDIR)

