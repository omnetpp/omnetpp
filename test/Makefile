
all: test_common test_core test_envir test_makemake test_featuretool test_fingerprint test_builds

test_builds:
	cd build && ./test-builds

test_common:
	cd common && ./runtest

test_core:
	cd core && ./runtest

test_makemake:
	cd makemake && ./runtest

test_fingerprint:
	cd fingerprint && ./runtest

test_envir:
	cd envir && ./runtest

test_featuretool:
	cd featuretool && ./runtest

clean:
	rm -rf core/work envir/work common/work makemake/work makemake/out featuretool/work fingerprint/results
