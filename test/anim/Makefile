FOLDERS = animation animation2 broadcast broadcast2 canvas changeparent chartinspector disabledconn \
          displaystring dynamic dynamic2 envirapi figureapi methodcall plugin refreshdisplay \
          senddirect swarm texthalo usman watch

all: makefiles
	for i in $(FOLDERS); do (cd $$i && make) || exit 1; done

makefiles:
	for i in $(FOLDERS); do (cd $$i && opp_makemake -f) || exit 1; done

clean: makefiles
	for i in $(FOLDERS); do (cd $$i && make clean) || exit 1; done

