set PERL=d:\perl\bin\perl.exe
%PERL% ltoh.pl history.tex
%PERL% ltoh.pl ch-introduction.tex
%PERL% ltoh.pl ch-overview.tex
%PERL% ltoh.pl ch-example-nim.tex
%PERL% ltoh.pl ch-ned-lang.tex
%PERL% ltoh.pl ch-simple-modules.tex
%PERL% ltoh.pl ch-sim-lib.tex
%PERL% ltoh.pl ch-msg-subcl.tex
%PERL% ltoh.pl ch-build-sim-progs.tex
%PERL% ltoh.pl ch-run-sim.tex
%PERL% ltoh.pl ch-ana-sim.tex
%PERL% ltoh.pl ch-parallel-exec.tex
%PERL% ltoh.pl ch-opp-design.tex
%PERL% ltoh.pl ch-opnet.tex
%PERL% ltoh.pl ch-parsec.tex
%PERL% ltoh.pl ch-ned-grammar.tex
%PERL% ltoh.pl references.tex

@echo. > usman.tmp
@type history.html >> usman.tmp
@type ch-introduction.html >> usman.tmp
@type ch-overview.html >> usman.tmp
@type ch-example-nim.html >> usman.tmp
@type ch-ned-lang.html >> usman.tmp
@type ch-simple-modules.html >> usman.tmp
@type ch-sim-lib.html >> usman.tmp
@type ch-msg-subcl.html >> usman.tmp
@type ch-build-sim-progs.html >> usman.tmp
@type ch-run-sim.html >> usman.tmp
@type ch-ana-sim.html >> usman.tmp
@type ch-parallel-exec.html >> usman.tmp
@type ch-opp-design.html >> usman.tmp
@type ch-opnet.html >> usman.tmp
@type ch-parsec.html >> usman.tmp
@type ch-ned-grammar.html >> usman.tmp
@type references.html >> usman.tmp
del ch-*.html history.html references.html

%PERL% prephtml2 usman.tmp

