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

The Txc1 simple module type is represented by the C++ class Txc1, which
has to be subclassed from cSimpleModule, and registered in OMNeT++ with the
Define_Module() macro. We redefine two methods from cSimpleModule: initialize()
and handleMessage(). They are invoked from the simulation kernel:
the first one only once, and the second one whenever a message arrives at the module.

In initialize() we create a message object (cMessage), and send it out
on gate <tt>out</tt>. Since this gate in connected to the other module's
input gate, the simulation kernel will deliver this message to the other module
in the argument to handleMessage() -- after a 100ms propagation delay
assigned to the link in the NED file. The other module just sends it back
(another 100ms delay), so it will result in a contiuous ping-pong.

Messages (packets, frames, jobs, etc) and events (timers, timeouts) are
all represented by cMessage objects (or its subclasses) in OMNeT++.
After you send or schedule them, they will be held by the simulation
kernel in the "scheduled events" or "future events" list until
their time comes and they are delivered to the modules via handleMessage().

Note that there is no stopping condition built into this simulation:
it would continue forever. You will be able to stop it from the GUI.
(You could also specify a simulation time limit or CPU time limit
in the configuration file, but we don't do that in the tutorial.)

4. We now create the Makefile which will help us to compile and link our program
to create the executable tictoc:

\code
$ opp_makemake
\endcode

This command should have now created a Makefile in the working directory
tictoc.

@note Windows+MSVC users: the command is <tt>opp_<i>n</i>makemake</tt>,
and it will create <tt>Makefile.vc</tt>.

5. Let's now compile and link our very first simulation by issuing the make command:

\code
$ make
\endcode

@note Windows+MSVC: type <tt>nmake -f Makefile.vc</tt>.
If you get <i>'nmake' is not recognized as an internal or external command...</i>,
find <tt>vcvars32.bat</tt> somewhere in the MSVC directories, and run it first thing
in every command window in which you want to compile.

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
$ ./tictoc
\endcode

and hopefully you should now get the OMNeT++ simulation window.

@note Windows: the command is just <tt>tictoc</tt>.

8. Press the Run button on the toolbar to start the simulation. What you should
see is that tic and toc are exchanging messages with each other.

<img src="tictoc.gif">

The main window toolbar displays the simulated time. This is virtual time, it
has nothing to do with the actual (or wall-clock) time that the program takes to
execute. Actually, how many seconds you can simulate in one real-world second
depends highly on the speed of your hardware and even more on the nature and
complexity of the simulation model itself. (The toolbar contains
a simsec/sec gauge which displays you this value.)

Note that it takes zero simulation time for a node to process the message.
The only thing that makes the simulation time pass in this model is
the propagation delay on the connections.

9. You can exit the simulation program by clicking its Close icon or choosing File|Exit.



<hr>
@section twonodes Enhancing the 2-node TicToc

@subsection s2 Step 2: Refining the graphics, and adding debugging output

Here we make the model look a bit prettier in the GUI. We assign
the "proc1" icon (the file <tt>bitmaps/proc1.gif</tt>), and paint it cyan for tic
and yellow for toc. This is achieved by adding display strings to the
NED file. The <tt>i=</tt> tag in the display string specifies the icon.

@dontinclude tictoc2.ned
@skip submodules
@until connections

We also modify the C++ file to add some debug messages to Txc1 by
writing to the OMNeT++ <tt>ev</tt> object like this:

@dontinclude txc2.cc
@skipline ev <<

and

@skipline ev <<

When you run the simulation in the OMNeT++ GUI Tkenv, the following output
will appear in the main text window:

\code
* Sending initial message
* ** Event #0.  T=0.1000000000 (100ms).  Module #3 `tictoc2.toc'
* Received message `tic', send it out again
* ** Event #1.  T=0.2000000000 (200ms).  Module #2 `tictoc2.tic'
* Received message `tic', send it out again
* ** Event #2.  T=0.3000000000 (300ms).  Module #3 `tictoc2.toc'
* Received message `tic', send it out again
\endcode

You can also open separate output windows for tic and toc. This feature
will be useful when you have a large model and you experience the
"fast scrolling logs syndrome".

Sources: @ref tictoc2.ned, @ref txc2.cc, @ref omnetpp.ini


@subsection s3 Step 3: Adding state variables

In this step we add a counter (<tt>int counter</tt>) to the class,
and delete the message after ten exchanges. We set the variable to
10 in initialize(), and decrement it with every message arrival,
in handleMessage(). After it reaches zero, the simulation will
run out of events and terminate.

Sources: @ref tictoc3.ned, @ref txc3.cc, @ref omnetpp.ini


@subsection s4 Step 4: Adding parameters

In this step you'll learn how to add input parameters to the simulation:
we'll turn the "magic number" 10 into a parameter.

Module parameters have to be declared in the NED file.

@dontinclude tictoc4.ned
@skip simple
@until gates

Typically you'll want to assign values to them from omnetpp.ini, although you can also
hardcode assignments in NED files. Here we do the latter.

@skip module
@until connections

We also have to modify the C++ code to read the parameter in
initialize(), and assign it to the counter.

@dontinclude txc4.cc
@skipline par(

Sources: @ref tictoc4.ned, @ref txc4.cc, @ref omnetpp.ini


@subsection s5 Step 5: Modelling processing delay

In the previous models, tic and toc immediately sent back the
received message. Here we'll add some timing: tic and toc will hold the
message for 1 simulated second before sending it back. In OMNeT++
such timing is achieved by the module sending a message to itself.
Such messages are called self-messages (but only because of the way they
are used, otherwise they are ordinary message objects).
Self-messages can be "sent" with the scheduleAt() function, and you can
specify when they should arrive back at the module.

@dontinclude txc5.cc
@skip ::handleMessage
@skipline scheduleAt(

Note that we added two cMessage* variables, <tt>event</tt> and <tt>tictocMsg</tt>
to the class, to remember the message we use for timing and message whose
processing delay we are simulating. In handleMessage() now we have to
differentiate whether a new message has arrived via the input gate
or the self-message came back (timer expired). Here we are using

@dontinclude txc5.cc
@skipline msg==

but we could have written

@code
    if (msg->isSelfMessage())
@endcode

as well.

We have left out the counter, to keep the source code small.

Sources: @ref tictoc5.ned, @ref txc5.cc, @ref omnetpp.ini


@subsection s6 Step 6

In this step we'll introduce random numbers. We change the delay from 1s
to a random value which can be set from the NED file or from omnetpp.ini.
In addition, we'll "lose" (delete) the packet with a small probability.

For tic, we hardcode the delayTime parameter into the NED file.

For `toc', we leave the delayTime parameter unassigned in the NED file,
and put the value into omnetpp.ini instead, to make it easier to change.

Sources: @ref tictoc6.ned, @ref txc6.cc, @ref omnetpp.ini


@subsection s7 Step 7

Let us take a step back, and remove random delaying from the code.
We'll leave in, however, losing the packet with a small probability.
And, we'll we do something very common in telecommunication networks:
if the packet doesn't arrive within a certain period, we'll assume it
was lost and create another one. The timeout will be handled using
(what else?) a self-message.

Sources: @ref tictoc7.ned, @ref txc7.cc, @ref omnetpp.ini


@subsection s8 Step 8

In the previous model we just created another packet if we needed to
retransmit. This is OK because the packet didn't contain much, but
in real life it's usually more practical to keep a copy of the original
packet so that we can re-send it without the need to build it again.

Sources: @ref tictoc8.ned, @ref txc8.cc, @ref omnetpp.ini


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

Sources: @ref tictoc9.ned, @ref txc9.cc, @ref omnetpp.ini


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

Sources: @ref tictoc10.ned, @ref tictoc10.msg, @ref txc10.cc, @ref omnetpp.ini


@subsection s11 Step 11

This model is exciting enough so that we can collect some statistics.
We'll record in output vectors the hop count of every message upon arrival.
Output vectors are written into the omnetpp.vec file and can be visualized
with the Plove program.

We also collect basic statistics (min, max, mean, std.dev.) and histogram
about the hop count which we'll print out at the end of the simulation.

Sources: @ref tictoc11.ned, @ref tictoc11.msg, @ref txc11.cc, @ref omnetpp.ini

*/


/// @page tictoc1.ned tictoc1.ned
/// @include tictoc1.ned

/// @page tictoc2.ned tictoc2.ned
/// @include tictoc2.ned

/// @page tictoc3.ned tictoc3.ned
/// @include tictoc3.ned

/// @page tictoc4.ned tictoc4.ned
/// @include tictoc4.ned

/// @page tictoc5.ned tictoc5.ned
/// @include tictoc5.ned

/// @page tictoc6.ned tictoc6.ned
/// @include tictoc6.ned

/// @page tictoc7.ned tictoc7.ned
/// @include tictoc7.ned

/// @page tictoc8.ned tictoc8.ned
/// @include tictoc8.ned

/// @page tictoc9.ned tictoc9.ned
/// @include tictoc9.ned

/// @page tictoc10.ned tictoc10.ned
/// @include tictoc10.ned

/// @page tictoc11.ned tictoc11.ned
/// @include tictoc11.ned

/// @page txc1.cc txc1.cc
/// @include txc1.cc

/// @page txc2.cc txc2.cc
/// @include txc2.cc

/// @page txc3.cc txc3.cc
/// @include txc3.cc

/// @page txc4.cc txc4.cc
/// @include txc4.cc

/// @page txc5.cc txc5.cc
/// @include txc5.cc

/// @page txc6.cc txc6.cc
/// @include txc6.cc

/// @page txc7.cc txc7.cc
/// @include txc7.cc

/// @page txc8.cc txc8.cc
/// @include txc8.cc

/// @page txc9.cc txc9.cc
/// @include txc9.cc

/// @page txc10.cc txc10.cc
/// @include txc10.cc

/// @page txc11.cc txc11.cc
/// @include txc11.cc

/// @page tictoc10.msg tictoc10.msg
/// @include tictoc10.msg

/// @page tictoc11.msg tictoc11.msg
/// @include tictoc11.msg

/// @page omnetpp.ini omnetpp.ini
/// @include omnetpp.ini


