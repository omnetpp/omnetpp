
all: test_anim test_common test_core test_envir test_makemake test_featuretool test_fingerprint test_histograms

test_anim:
	cd anim && make

test_build:
	cd build && ./test-builds

test_common:
	cd common && ./runtest

test_core:
	cd core && ./runtest

test_makemake:
	cd makemake && ./runtest

test_fingerprint: test_histograms
	cd fingerprint && ./runtest

test_envir:
	cd envir && ./runtest

test_featuretool:
	cd featuretool && ./runtest

test_histograms:
	cd histograms && opp_makemake -f && make

clean:
	rm -rf core/work envir/work common/work makemake/work makemake/out featuretool/work fingerprint/results
	cd anim && make clean
