FOLDERS = stress histograms

all: makefiles
	for i in $(FOLDERS); do (cd $$i && make) || exit 1; done

makefiles:
	for i in $(FOLDERS); do (cd $$i && opp_makemake -f -o $$i) || exit 1; done

clean: makefiles
	for i in $(FOLDERS); do (cd $$i && make clean) || exit 1; done

