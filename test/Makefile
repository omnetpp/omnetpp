#
# This Makefiles contains the list of official regression tests that
# are REQUIRED to pass before each release.
#
MAKEFLAGS=-j1

# a (relatively) fast test which runs all tests that can finish in reasonable time. (i.e. full builds excluded)
test_quick: | test_common test_envir test_core test_anim test_models test_makemake test_featuretool \
              test_sqliteresultfiles test_fingerprint test_scave_results_api \
              test_scave_charttemplates test_scave_analysis test_scave_multi_project test_scave_workspace

# Test everything.
all: | test_quick test_build test_toolchain

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

test_toolchain:
	cd toolchain && ./runtest

test_scave_results_api:
	cd scave/results_api && ./runtest

test_scave_charttemplates:
	cd scave/charttemplates && ./runtest

test_scave_analysis:
	cd scave/analysis && ./runtest

test_scave_multi_project:
	cd scave/multi_project && ./runtest

test_scave_workspace:
	cd scave/workspace && ./runtest

cleanall: clean   # TODO

clean:
	rm -rf core/work envir/work common/work makemake/work makemake/out featuretool/work fingerprint/results test_sqliteresultfiles/results-*
	cd anim && make clean
	cd models && make clean
