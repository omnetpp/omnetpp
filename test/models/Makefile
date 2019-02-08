all: test_histograms test_stress

test_histograms:
	cd histograms && opp_makemake -f && make MODE=debug

test_stress:
	cd stress && opp_makemake -f && make MODE=debug

clean:
	cd histograms && make clean MODE=debug
	cd stress && make clean MODE=debug
