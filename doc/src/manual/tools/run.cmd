call ..\..\..\..\setenv.bat
perl stripgrammar.pl ../../../../src/nedxml/ned2.y >ned.txt
perl stripgrammar.pl ../../../../src/nedxml/msg2.y >msg.txt
opp_run -h nedfunctions >nedfuncs.txt
perl processnedfuncs.pl <nedfuncs.txt >nedfuncs.tex
