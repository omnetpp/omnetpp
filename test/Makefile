#
# This Makefiles contains the list of official regression tests that
# are REQUIRED to pass before each release.
#


all: test_anim test_models test_build test_common test_core test_envir test_makemake test_featuretool test_fingerprint test_sqliteresultfiles test_msys

test_anim:
	cd anim && make

test_models:
	cd models && make

test_build:
	cd build && ./test-builds

test_common:
	cd common && ./runtest

test_core:
	cd core && ./runtest

test_makemake:
	cd makemake && ./runtest

test_fingerprint: test_models
	cd fingerprint && ./runtest

test_envir:
	cd envir && ./runtest

test_featuretool:
	cd featuretool && ./runtest

test_sqliteresultfiles:
	cd sqliteresultfiles && ./runtest

test_msys:
	cd msys && ./runtest

clean:
	rm -rf core/work envir/work common/work makemake/work makemake/out featuretool/work fingerprint/results test_sqliteresultfiles/results-*
	cd anim && make clean
