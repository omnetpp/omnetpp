all: test_histograms test_stress

test_histograms:
	cd histograms && opp_makemake -f && make

test_stress:
	cd stress && opp_makemake -f && make

clean:
	cd histograms && make clean
	cd stress && make clean
