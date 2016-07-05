all:
	cd src && $(MAKE)
	cd simulations && $(MAKE)

clean:
	cd src && $(MAKE) clean
	cd simulations && $(MAKE) clean

cleanall:
	cd src && $(MAKE) MODE=release clean
	cd src && $(MAKE) MODE=debug clean
	rm -f src/Makefile
	cd simulations && $(MAKE) MODE=release clean
	cd simulations && $(MAKE) MODE=debug clean
	rm -f simulations/Makefile
