
all: test_core test_makemake

test_core:
	cd core && ./runtest

test_makemake:
	cd makemake && ./runtest

clean:
	rm -rf core/work
	rm -rf makemake/work
	rm -rf makemake/out
