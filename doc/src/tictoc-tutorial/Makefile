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
	cp *.gif *.png ../../tictoc-tutorial
clean:
	-rm -rf ../../tictoc-tutorial
