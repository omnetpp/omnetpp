FOLDERS = stress histograms

all: makefiles
	for i in $(FOLDERS); do (cd $$i && make MODE=debug) || exit 1; done

makefiles:
	for i in $(FOLDERS); do (cd $$i && opp_makemake -f) || exit 1; done

clean: makefiles
	for i in $(FOLDERS); do (cd $$i && make clean MODE=debug) || exit 1; done

