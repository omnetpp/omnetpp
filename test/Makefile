
PERL=perl
TESTFILES=*.test
TFLAGS=-v

all: test

test:
	if ! [ -d work ];  then mkdir work; fi
	$(PERL) opp_test.pl -g $(TFLAGS) $(TESTFILES)
	cd work; opp_makemake -f -u cmdenv; make
	$(PERL) opp_test.pl -r $(TFLAGS) $(TESTFILES)

clean:
	rm -rf work