/**

@mainpage TicToc Tutorial for OMNeT++

This short tutorial to OMNeT++ guides you through an example
of modeling and simulation, showing you along the way some
of the commonly used OMNeT++ features.

The tutorial is based on the Tictoc example simulation, which you
can find in the <tt>samples/tictoc</tt> directory of your
OMNeT++ installation, so you can try out immediately how
the examples work. However, you'll find the tutorial much more useful
if you actually carry out at least the first steps described here.

@attention We assume here that you have a working OMNeT++ installation,
you have a good C++ knowledge, and you are in general familiar with
C/C++ development (editing source files, compiling, debugging etc.)
on your operating system. (The latter two are out of our scope here --
there are excellent books, tutorials on the web, etc. if you need
to update your knowledge on that.)

This document and the TicToc model are an expanded version of
the original TicToc tutorial from Ahmet Sekercioglu (Monash University).

@section contents Contents

  - @ref firststeps
  - @ref twonodes
  - @ref nnodes


<hr>
@section firststeps Creating your first simulation model

The purpose of this tutorial is to introduce you to the simulation
of telecommunications networks with OMNeT++. For a start, let us begin
with a "network" that consists of two nodes. The nodes will do something
simple: one the nodes will create a packet, and the two nodes
will keep passing the same packet back and forth. We'll call the nodes
"tic" and "toc".

Here are the steps you take to implement your first simulation from scratch:

1. Create a working directory called tictoc, and cd to this directory.

2. Describe your example network by creating a topology file. A topology file is a
text file that identifies the network's nodes and the links between them.
You can create it with your favourite text editor.
Let's call it tictoc1.ned:

@include tictoc1.ned

The file is best read from the bottom up. Here's what it says:

   - we define a network called tictoc1, which is an instance the
     module type Tictoc1 (<tt>network..endnetwork</tt>);
   - Tictoc1 is a compound module, which is assembled from two submodules,
     tic and toc. tic and toc are instances of the same module type
     called Txc1. We connect tic's output gate (named out) to toc's input
     gate (named in), and vica versa (<tt>module..endmodule</tt>). There
     will be a 100ms propagation delay both ways;
   - Txc1 is a simple module type (which means it is atomic on NED level, and
     will be implemented in C++). Txc1 has one input gate named in,
     and one output gate named out (<tt>simple..endsimple</tt>).

3. We now need to implement the functionality of the simple module Txc1. This is
achieved by writing a C++ file txc1.cc:

@include txc1.cc

4. We now create the Makefile which will help us to compile and link our program
to create the executable tictoc:

\code
opp_makemake
\endcode

This command should have now created a Makefile in the working directory
tictoc.

@note Windows+MSVC users: the command is opp_<i>n</i>makemake,
and it will create Makefile.vc.

5. Let's now compile and link our very first simulation by issuing the make command:

\code
make
\endcode

@note Windows+MSVC: type nmake -f Makefile.vc. If you get <i>'nmake' is not recognized
as an internal or external command...</i>, find vcvars32.bat somewhere in the
MSVC directories, and run it first thing in every command window in which you
want to compile.

If there are compilation errors, you need to rectify those and repeat the make until
you get an error-free compilation and linking.

6. If you start the executable now, it will complain that it cannot find
the file omnetpp.ini, so you have to create one. omnetpp.ini tells the
simulation program which network you want to simulate (yes, several networks
can live in the same simulation program), you can pass parameters
to the model, explicitly specify seeds for the random number generators etc.

Create the following very simple omnetpp.ini:

\code
[General]
network = tictoc1
\endcode

tictoc2 and further steps will all share the following omnetpp.ini:

@include omnetpp.ini

which even doesn't specify the network (the simulation program ask it
in a dialog when it starts).

7. Once you complete the above steps, you launch the simulation by issuing this
command:

\code
./tictoc
\endcode

and hopefully you should now get the OMNeT++ simulation window.

@note Windows: the command is just tictoc.

8. Press the Run button on the toolbar to start the simulation. What you should
see is that tic and toc are exchanging messages with each other.

<img src="tictoc.gif">

The main window toolbar displays the simulated time. This is virtual time, it
has nothing to do with the actual (or wall-clock) time that the program takes to
execute. Actually, how many seconds you can simulate in one real-world second
depends highly on the speed of your hardware and even more on the nature and
complexity of the simulation model itself (Exercise 2 will emphasize this issue).
Note that the toolbar also contains a simsec/sec gauge which displays you this
value.

9. You can exit the simulation program by clicking its Close icon or choosing File|Exit.



<hr>
@section twonodes Enhancing the 2-node TicToc

@subsection s2 Step 2: adding icons and debug output

Complete sources: @ref f2

// Here we make the model look a bit prettier in the GUI. We assign
// the "proc1" icon (bitmaps/proc1.gif), and paint it cyan for `tic'
// and yellow for `toc'.

In this step we add some debug messages to Txc1. When you run the
simulation in the OMNeT++ GUI Tkenv, the output will appear in
the main text window, and you can also open separate output windows
for `tic' and `toc'.

The main window behind displays text messages generated via the ev << ...
lines from these modules. Observe that the messages "Hello World! I'm tic."
and "Hello World! I'm toc." are only printed once at the beginning of the
simulation.



@subsection s3 Step 3

In this class we add a counter, and delete the message after ten exchanges.

Complete sources: @ref f3


@subsection s4 Step 4

In this step you'll learn how to add input parameters to the simulation:
we'll turn the "magic number" 10 into a parameter.

For that, we need to declare the parameter in the NED file, and
then to get its value from the C++ code.

Complete sources: @ref f4


@subsection s5 Step 5

In the previous models, `tic' and `toc' immediately sent back the
received message. Here we'll add some timing: tic and toc will hold the
message for 1 simulated second before sending it back. In OMNeT++
such timing is achieved by the module sending a message to itself.
Such messages are called self-messages (but only because of the way they
are used, otherwise they are completely ordinary messages) or events.
Self-messages can be "sent" with the scheduleAt() function, and you can
specify when they should arrive back at the module.

We leave out the counter, to keep the source code small.

Complete sources: @ref f5


@subsection s6 Step 6

In this step we'll introduce random numbers. We change the delay from 1s
to a random value which can be set from the NED file or from omnetpp.ini.
In addition, we'll "lose" (delete) the packet with a small probability.

For tic, we hardcode the delayTime parameter into the NED file.

For `toc', we leave the delayTime parameter unassigned in the NED file,
and put the value into omnetpp.ini instead, to make it easier to change.

Complete sources: @ref f6


@subsection s7 Step 7

Let us take a step back, and remove random delaying from the code.
We'll leave in, however, losing the packet with a small probability.
And, we'll we do something very common in telecommunication networks:
if the packet doesn't arrive within a certain period, we'll assume it
was lost and create another one. The timeout will be handled using
(what else?) a self-message.

Complete sources: @ref f7


@subsection s8 Step 8

In the previous model we just created another packet if we needed to
retransmit. This is OK because the packet didn't contain much, but
in real life it's usually more practical to keep a copy of the original
packet so that we can re-send it without the need to build it again.

Complete sources: @ref f8


<hr>
@section nnodes Turning it into a real network


@subsection s9 Step 9

Let's make it more interesting by using several (n) `tic' modules,
and connecting every module to every other. For now, let's keep it
simple what they do: module 0 generates a message, and the others
keep tossing it around in random directions until it arrives at
module 2.

NED file is completely different:

tictoc9.ned: @include tictoc9.ned

Complete sources: @ref f9


@subsection s10 Step 10

In this step the destination address is no longer node 2 -- we draw a
random destination, and we'll add the destination address to the message.

The best way is to subclass cMessage and add destination as a data member.
Hand-coding the message class is usually tiresome because it contains
a lot of boilerplate code, so we let OMNeT++ generate the class for us.
The message class specification is in tictoc10.msg -- tictoc10_m.h
and .cc will be generated from this file automatically.

To make the model execute longer, after a message arrives to its destination
the destination node will generate another message with a random destination
address, and so forth.

Complete sources: @ref f10


@subsection s11 Step 11

This model is exciting enough so that we can collect some statistics.
We'll record in output vectors the hop count of every message upon arrival.
Output vectors are written into the omnetpp.vec file and can be visualized
with the Plove program.

We also collect basic statistics (min, max, mean, std.dev.) and histogram
about the hop count which we'll print out at the end of the simulation.

Complete sources: @ref f11

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

