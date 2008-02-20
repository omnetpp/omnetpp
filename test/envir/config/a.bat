:FIXME turn this into a decent test suite
set PROG=..\..\..\samples\fifo\fifo.exe

: %PROG% -u Cmdenv -g -n test1 a.ini
: %PROG% -u Cmdenv -g -n test2 a.ini
: %PROG% -u Cmdenv -g -n test3 a.ini
: %PROG% -u Cmdenv -g -n Bogus a.ini
: %PROG% -u Cmdenv -g -n NoIteration a.ini
: %PROG% -u Cmdenv -g -n Refs a.ini
%PROG% -u Cmdenv -g -n Refs2 a.ini
: %PROG% -u Cmdenv -g -n NoSuchConfig a.ini
: %PROG% -u Cmdenv -g -n test3 a.ini
: %PROG% -u Cmdenv -g -c test3 -r 1 a.ini