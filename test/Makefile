
all: test_builds

test_builds:
	cd build && ./test-builds

test_common:
	cd common && ./runtest

test_core:
	cd core && ./runtest

test_makemake:
	cd makemake && ./runtest

test_envir:
	cd envir && ./runtest

clean:
	rm -rf core/work
	rm -rf common/work
	rm -rf makemake/work
	rm -rf makemake/out
	rm -rf envir/work
