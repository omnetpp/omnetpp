/**

@mainpage TicToc Tutorial for OMNeT++

This short tutorial to OMNeT++ guides you through an example
of modeling and simulation, showing you along the way some
of the commonly used OMNeT++ features.

Here are the steps you take to implement your first simulation:

1. Create a working directory called tictoc, and cd to this directory.

2. Describe your example network by creating a topology file. A topology file is a
text file that identifies the network's nodes and the links between them. Let's call
this file tictoc.ned:

tictoc1.ned: @include tictoc1.ned

In this file, we define a network called tictoc, which consists of a compound
module Tictoc. The compound module, in turn, consists of submodules tic and toc.
tic and toc are instances of the same simple module type called Txc 1 . Txc has one
input gate (named in), and one output gate (named out). We connect tic's output
gate to toc's input gate, and vice versa.

We now need to implement the functionality of the simple module Txc. This is
achieved by writing two C++ files: txc.h and txc.cc:

txc1.cc: @include txc1.cc

Then, we need to write the file omnetpp.ini which will tell the simulation tool
what to do:

omnetpp.ini: @include omnetpp.ini

We now create the Makefile which will help us to compile and link our program
to create the executable tictoc:

opp_makemake

This command should have now created a Makefile in the working directory
tictoc.

6. Let's now compile and link our very first simulation by issuing the make command:

make

If there are compilation errors, you need to rectify those and repeat the make until
you get an error-free compilation and linking.

8. Once you complete the above steps, you launch the simulation by issuing this
command:

./tictoc

and, hopefully you should now get the OMNeT++ simulation window similar to
the one shown in Figure 1.

9. Press the ihrunlr button to start the simulation. What you should see is that tic
and toc are exchanging messages with each other. This is the result of the send()
and receive() calls in the C++ code.

The main window behind displays text messages generated via the ev << ...
lines from these modules. Observe that the messages "Hello World! I'm tic."
and "Hello World! I'm toc." are only printed once at the beginning of the
simulation.

The main window toolbar displays the simulated time. This is virtual time, it
has nothing to do with the actual (or wall-clock) time that the program takes to
execute. Actually, how many seconds you can simulate in one real-world second
depends highly on the speed of your hardware and even more on the nature and
complexity of the simulation model itself (Exercise 2 will emphasize this issue).
Note that the toolbar also contains a simsec/sec gauge which displays you this
value.


@section steps Adding more

@subsection s2 Step 2: adding icons and debug output

@ref f2


@subsection s3 Step 3

@ref f3


@subsection s4 Step 4

@ref f4


@subsection s5 Step 5

@ref f5


@subsection s6 Step 6

@ref f6


@subsection s7 Files: Step 7

@ref f7


@subsection s8 Step 8

@ref f8


@subsection s9 Step 9

@ref f9


@subsection s10 Step 10

@ref f10


@subsection s11 Step 11

@ref f11
*/


/**
@page f1 Tictoc1 files

tictoc1.ned: @include tictoc1.ned
txc1.cc: @include txc1.cc
omnetpp.ini: @include omnetpp.ini
*/

/**
@page f2 Tictoc2 files

tictoc2.ned: @include tictoc2.ned
txc2.cc: @include txc2.cc
omnetpp.ini: @include omnetpp.ini
*/

/**
@page f3 Tictoc3 files

tictoc3.ned: @include tictoc3.ned
txc3.cc: @include txc3.cc
omnetpp.ini: @include omnetpp.ini
*/

/**
@page f4 Tictoc4 files

tictoc4.ned: @include tictoc4.ned
txc4.cc: @include txc4.cc
omnetpp.ini: @include omnetpp.ini
*/

/**
@page f5 Tictoc5 files

tictoc5.ned: @include tictoc5.ned
txc5.cc: @include txc5.cc
omnetpp.ini: @include omnetpp.ini
*/

/**
@page f6 Tictoc6 files

tictoc6.ned: @include tictoc6.ned
txc6.cc: @include txc6.cc
omnetpp.ini: @include omnetpp.ini
*/

/**
@page f7 Tictoc7 files

tictoc7.ned: @include tictoc7.ned
txc7.cc: @include txc7.cc
omnetpp.ini: @include omnetpp.ini
*/

/**
@page f8 Tictoc8 files

tictoc8.ned: @include tictoc8.ned
txc8.cc: @include txc8.cc
omnetpp.ini: @include omnetpp.ini
*/

/**
@page f9 Tictoc9 files

tictoc9.ned: @include tictoc9.ned
txc9.cc: @include txc9.cc
omnetpp.ini: @include omnetpp.ini
*/

/**
@page f10 Tictoc10 files

tictoc10.ned: @include tictoc10.ned
txc10.cc: @include txc10.cc
tictoc10.msg: @include tictoc10.msg
omnetpp.ini: @include omnetpp.ini
*/

/**
@page f11 Tictoc11 files

tictoc11.ned: @include tictoc11.ned
txc11.cc: @include txc11.cc
tictoc11.msg: @include tictoc11.msg
omnetpp.ini: @include omnetpp.ini
*/

