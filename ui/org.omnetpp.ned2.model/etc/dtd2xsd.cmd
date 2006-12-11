@echo off
perl ..\..\..\_scripts\dtd2xsd.pl -prefix ned -ns http://omnetpp.org/schema/ned2 ned2-ned.dtd >ned2.xsd
del *.out