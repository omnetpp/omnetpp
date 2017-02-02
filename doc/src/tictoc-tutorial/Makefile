#
# Definitions
#
DOXYGEN=doxygen
TARGET_DIR=../../tictoc-tutorial

#
# Targets
#
all:
	rm -rf $(TARGET_DIR)
	mkdir $(TARGET_DIR)
	$(DOXYGEN) tictoc.doxyfile
	cp *.png *.gif $(TARGET_DIR)
	@# Replacing original xhtml doctype with html because doxygen 1.8.11 generates invalid XML as output
	@# and the IDE needs valid XML to be able to display it.
	perl -i -pe 's|<!DOCTYPE.*>|<!DOCTYPE html>|' $(TARGET_DIR)/*.html
clean:
	-rm -rf $(TARGET_DIR)
