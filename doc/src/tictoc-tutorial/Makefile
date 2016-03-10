#
# Definitions
#
DOXYGEN=doxygen

#
# Targets
#
all:
	rm -rf ../../tictoc-tutorial
	mkdir ../../tictoc-tutorial
	$(DOXYGEN) tictoc.doxyfile
	cp *.png ../../tictoc-tutorial
clean:
	-rm -rf ../../tictoc-tutorial
