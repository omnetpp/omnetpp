/**
@mainpage OMNeT++ API Reference

<p>This chapter contains a full example program that can give you
some basic idea of using the simulator. An enhanced version of
the Nim example can be found among the sample programs.

@include txc1.cc

@include omnetpp.ini

@include omnetpp.ini

<p>Nim is an ancient game with two players and a bunch of sticks.
The players take turns, removing 1, 2, 3 or 4 sticks from the
heap of sticks at each turn. The one who takes the last stick
is the loser.

<p>
Of course, building a model of the Nim game is not much of a
simulation project, but it nicely demonstrates the modeling approach
used by OMNeT++.

<p>
Describing the model consists of two phases:
<ul>
<li><strong></strong>topology description
<li><strong></strong>defining the operation of components
</ul>

<p>

<p><hr><H2>Topology</H2>

<p>The game can be modelled in OMNeT++ as a network with three modules:
the ``game'' (a manager module) and two players.
The modules will communicate by exchanging messages. The ``game''
module keeps the current number of tokens and organizes the game;
in each turn, the player modules receives the number of tokens
from the Game module and sends back its move.

<p>
<div align=center>
<img src="figures/usmanFig6.gif">
<center><i>Figure: Module structure for the Nim game.</i></center>
</div>

<p>
<tt>Player1</tt>, <tt>Player2</tt> and <tt>Game</tt> are simple
modules (e.g. they have no submodules.) Each module is an instance of
a module type. We'll need a module type to represent the <tt>Game</tt> module;
let's call it <tt>Game</tt> too.

<p>We can implement two kinds of players: <tt>SmartPlayer</tt>, which knows
the winning algorithm, and <tt>SimplePlayer</tt>, which simply takes a
random number of sticks. In our example, <tt>Player1</tt> will be a <tt>SmartPlayer</tt>
and <tt>Player2</tt> will be a <tt>SimplePlayer</tt>.

<p>The enclosing module, <tt>Nim</tt> is a compound module (it has submodules).
It is also defined as a module type of which one instance is
created, the system module.

<p>Modules have input and output gates (the tiny boxes
labeled <tt>in</tt>, <tt>out</tt>, <tt>fromPlayer1</tt>, etc. in the
figure). An input and an output gate can be connected: connections (or
links) are shown as in the figure as arrows.  During the simulation,
modules communicate by sending messages through the connections.

<p>
The user defines the topology of the network in NED files.

<p>
We placed the model description in two files; the first file
defines the simple module types and the second one the system
module.

<p>The first file (NED keywords are typed in boldface):

<p><pre>
//-
// file: nim_mod.ned
// Simple modules in nim.ned
//-

<p>
// Declaration of simple module type Game.

<p><b>simple</b> Game
    <b>parameters</b>:
         numSticks, // initial number of sticks
         firstMove; // 1=Player1, 2=Player2

<p>    <b>gates</b>:
        <b>in</b>:
             fromPlayer1, // input and output gates
             fromPlayer2; // for connecting to Player1/Player2
        <b>out</b>:
             toPlayer1,
             toPlayer2;
<b>endsimple</b>

<p>
// Now the declarations of the two simple module types.
// Any one of the two types can be Player1 or Player2.

<p>// A player that makes random moves
<b>simple</b> SimplePlayer
    <b>gates</b>:
        <b>in</b>: in; // gates for connecting to Game
        <b>out</b>: out;
<b>endsimple</b>

<p>// A player who knows the winning algorithm
<b>simple</b> SmartPlayer
    <b>gates</b>:
        <b>in</b>: in; // gates for connecting to Game
        <b>out</b>: out;
<b>endsimple</b>
</pre>

<p>The second file:

<p><pre>
//-
// file: nim.ned
// Nim compound module + system module
//-

<p><b>import</b> "nim_mod";

<p><b>module</b> Nim
    <b>submodules</b>:
        game: Game
            <b>parameters</b>:
                numSticks = intuniform(21, 31),
                firstMove = intuniform(1, 2);
        player1: SmartPlayer;
        player2: SimplePlayer;
    <b>connections</b>:
        player1.out -&gt; game.fromPlayer1,
        player1.in &lt;- game.toPlayer1,
        player2.out -&gt; game.fromPlayer2,
        player2.in &lt;- game.toPlayer2;
<b>endmodule</b>

<p>// system module creation
<b>network</b>
    nim: Nim
<b>endnetwork</b>
</pre>

<p>

<p><hr><H2>Simple modules</H2>

<p>The module types <tt>SmartPlayer</tt>, <tt>SimplePlayer</tt> and <tt>Game</tt> are implemented
in C++, using the OMNeT++ library classes and functions.

<p>Each simple module type is derived from the C++
class <tt>cSimpleModule</tt>, with its <tt>activity()</tt> member
function redefined. The <tt>activity()</tt> functions of all
simple modules in the network are executed as
coroutines, so they appear as if they were running in
parallel.  Messages are instances of the class <tt>cMessage</tt>.

<p>We present here the C++ sources of the <tt>SmartPlayer</tt> and <tt>Game</tt> module
types.

<p>The <tt>SmartPlayer</tt> first introduces himself by sending its name
to the <tt>Game</tt> module. Then it enters an infinite loop; with each
iteration, it receives a message from <tt>Game</tt> with the number of
sticks left, it calculates its move and sends back a message
containing the move.

<p>Here's the source:

<pre>
#include &lt;stdio.h&gt;
#include &lt;string.h&gt;
#include &lt;time.h&gt;

#include &lt;omnetpp.h&gt;

// derive SmartPlayer from cSimpleModule
class SmartPlayer : public cSimpleModule
{
    Module_Class_Members( SmartPlayer, cSimpleModule, 8192)
    // this is a macro; it expands to constructor definition etc.
    // 8192 is the size for the coroutine stack (in bytes)

virtual void activity();
    // this redefined virtual function holds the algorithm
};

// register the simple module class to OMNeT++
Define_Module( SmartPlayer );

// define operations of SmartPlayer
void SmartPlayer::activity()
{
    int move;

    // initialization phase: send module type to Game module
    // create a message with the name "SmartPlayer" and send it to Game

    cMessage *msg = new cMessage("SmartPlayer");
    send(msg, "out");

    // infinite loop to process moves;
    // simulation will be terminated by Game

    for (;;)
    {
        // messages have several fields; here, we'll use the message
        // kind member to store the number of sticks
        cMessage *msgin = receive();    // receive message from Game
        int numSticks = msgin-&gt;kind();  // extract message kind (an int)
                                        // it hold the number of sticks
                                        // still on the stack
        delete msgin;                   // dispose of the message

        move = (numSticks + 4) % 5;     // calculate move
        if (move == 0)                  // we cannot take zero
            move = 1;                   // seems like we going to lose

        ev &lt;&lt; "Taking " &lt;&lt; move         // some debug output. The ev
           &lt;&lt; " out of " &lt;&lt; numSticks   // object represents the user
           &lt;&lt; " sticks.\n";             // interface of the simulator

        cMessage *msgout = new cMessage;// create empty message
        msgout-&gt;setKind( move );        // use message kind as storage
                                        // for move
        send( msgout, "out"); // send the message to Game
    }
}
</pre>
<p>
The <tt>Game</tt> module first waits for a message from both players
and extracts the message names that are also the players' names.  Then
it enters a loop, with the <tt>playerToMove</tt> variable
alternating between 1 and 2. With each iteration, it sends out a
message with the current number of sticks to the corresponding player
and gets back the number of sticks taken by that player. When the
sticks are out, the module announces the winner and ends the
simulation.

<p>The source:

<p>
<pre>
//-------------------------------------------------------------
// file: game.cc
// (part of NIM - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#include &lt;stdio.h&gt;
#include &lt;string.h&gt;

#include &lt;omnetpp.h&gt;

// derive Game from cSimpleModule
class Game : public cSimpleModule
{
    Module_Class_Members(Game,cSimpleModule,8192)
      // this is a macro; it expands to constructor definition etc.
      // 8192 is the size for the coroutine stack (in bytes)

    virtual void activity();
      // this redefined virtual function holds the algorithm
};

// register the simple module class to OMNeT++
Define_Module( Game );

// operation of Game:
void Game::activity()
{
    // strings to store player names; player[0] is unused
    char player[3][32];

    // read parameter values
    int numSticks = par("numSticks");
    int playerToMove = par("firstMove");

    // waiting for players to tell their names
    for (int i=0; i&lt;2; i++)
    {
        cMessage *msg = receive();
        if (msg-&gt;arrivedOn("fromPlayer1"))
            strcpy( player[1], msg-&gt;name());
        else
            strcpy( player[2], msg-&gt;name());
        delete msg;
    }

    // ev represents the user interface of the simulator
    ev &lt;&lt; "Let the game begin!\n";
    ev &lt;&lt; "Player 1: " &lt;&lt; player[1] &lt;&lt; "   Player 2: " &lt;&lt; player[2]
       &lt;&lt; "\n\n";

    do
    {
        ev &lt;&lt; "Sticks left: " &lt;&lt; numSticks &lt;&lt; "\n";
        ev &lt;&lt; "Player " &lt;&lt; playerToMove &lt;&lt; " ("
           &lt;&lt; player[playerToMove] &lt;&lt; ") to move.\n";

        cMessage *msg = new cMessage("", numSticks);
                        // numSticks will be the msg kind

        if (playerToMove == 1)
            send(msg, "toPlayer1");
        else
            send(msg, "toPlayer2");

        msg = receive();
        int sticksTaken = msg-&gt;kind();
        delete msg;

        numSticks -= sticksTaken;

        ev &lt;&lt; "Player " &lt;&lt; playerToMove &lt;&lt; " ("
           &lt;&lt; player[playerToMove] &lt;&lt; ") took "
           &lt;&lt; sticksTaken &lt;&lt; " stick(s).\n";

        playerToMove = 3 - playerToMove;
    }
    while (numSticks&gt;0);

    ev &lt;&lt; "\nPlayer " &lt;&lt; playerToMove &lt;&lt; " ("
       &lt;&lt; player[playerToMove] &lt;&lt; ") won!\n";

    endSimulation();
}
</pre>

<p>

<p>
<hr><H2>Running the simulation</H2>

<p>Once the source files are ready, one needs to compile and link
them into a simulation executable. One can specify the user interface
to be linked.

<p>Before running the simulation, one can put parameter values and
all sorts of other settings into an initialization file that
will be read when the simulation program starts:

<p>

<pre>
#
# file: omnetpp.ini
#

[General]
network = nim
random-seed = 3
ini-warnings = false

[Cmdenv]
express-mode = no
</pre>
<p>
Suppose we link the Nim simulation with the command line user
interface. We get the executable <tt>nim</tt> (<tt>nim.exe</tt> under Windows).
When we run it, we'll get the following screen output:

<pre>
% ./nim
</pre>
<p>
Or:

<pre>
C:\OMNeT++\samples\nim&gt; nim

OMNeT++ Discrete Event Simulation  (C) 1992-2003 Andras Varga
See the license for distribution terms and warranty disclaimer
Setting up Cmdenv (command-line user interface)...

Preparing for Run #1...
Setting up network `nim'...
Running simulation...

Let the game begin!
Player 1: SmartPlayer Player 2: SimplePlayer

Sticks left: 29
Player 2 (SimplePlayer) to move.
SimplePlayer is taking 2 out of 29 sticks.
Player 2 (SimplePlayer) took 2 stick(s).
Sticks left: 27
Player 1 (SmartPlayer) to move.
SmartPlayer is taking 1 out of 27 sticks.
Player 1 (SmartPlayer) took 1 stick(s).
Sticks left: 26
[...]
Sticks left: 5
Player 1 (SmartPlayer) to move.
SmartPlayer is taking 4 out of 5 sticks.
Player 1 (SmartPlayer) took 4 stick(s).
Sticks left: 1
Player 2 (SimplePlayer) to move.
SimplePlayer is taking 1 out of 1 sticks.
Player 2 (SimplePlayer) took 1 stick(s).

Player 1 (SmartPlayer) won!
&lt;!&gt; Module nim.game: Simulation stopped with endSimulation().

End run of OMNeT++
</pre>
<p>

<p>
<hr><H2>Other examples</H2>

<p>An enhanced version of the Nim example can be found among the sample
programs. It adds a third, interactive player and derives specific
player types from a <tt>Player</tt> abstract class. It also adds the
possibility that actual types for <tt>player1</tt> and
<tt>player2</tt> can be specified in the ini file or interactively
entered by the user at the beginning of the simulation.

<p>Nim does not show very much of how complex algorithms like communication
protocols can be implemented in OMNeT++. To have an idea about
that, look at the Token Ring example. It is also extensively
commented, though you may need to peep into the user manual to
fully understand it. The Dyna simulation models a simple
client-server network and demonstrates dynamic module creation.
The FDDI example is an accurate FDDI MAC simulation
which was written on the basis of the ANSI standard.

<p>
The following table summarizes the sample simulations:

<p><table border>
<TR> <TD align=left>

<p><b>NAME</b> </TD> <TD align=left> <b>TOPIC</b> </TD> <TD align=left> <b>DEMONSTRATES</b></TD></TR>
<TR> <TD align=left> <b>nim</b> </TD> <TD align=left> a simple two-player game
</TD> <TD align=left>
 module inheritance;
module type as parameter</TD></TR>
<TR> <TD align=left> <b>hcube</b>
</TD> <TD align=left>
 hypercube network with deflection routing
</TD> <TD align=left>
 hypercube topology with dimension as parameter;
topology templates;
output vectors</TD></TR>
<TR> <TD align=left> <b>token</b> </TD> <TD align=left> Token Ring network
</TD> <TD align=left>
 ring topology with the number of nodes as parameter;
using <tt>cQueue</tt>;
<tt>wait()</tt>;
output vectors</TD></TR>
<TR> <TD align=left> <b>fifo1</b> </TD> <TD align=left> single-server queue
</TD> <TD align=left>
 simple module inheritance;
decomposing <tt>activity()</tt> into several functions;
using simple statistics and output vectors;
printing stack usage info to help optimize memory consumption;
using <tt>finish()</tt></TD></TR>
<TR> <TD align=left> <b>fifo2</b> </TD> <TD align=left> another fifo implementation
</TD> <TD align=left>
 using <tt>handleMessage()</tt>;
decomposing <tt>handleMessage()</tt> into several functions;
the FSM macros;
simple module inheritance;
using simple statistics and output vectors;
using <tt>finish()</tt></TD></TR>
<TR> <TD align=left> <b>fddi</b> </TD> <TD align=left> FDDI MAC simulation
</TD> <TD align=left>
 using statistics classes;
and many other features</TD></TR>
<TR> <TD align=left> <b>hist</b> </TD> <TD align=left> demo of the histogram classes
</TD> <TD align=left>
 collecting observations into statistics objects;
saving statistics objects to file and restoring them</TD></TR>
<TR> <TD align=left> <b>dyna</b> </TD> <TD align=left> a client-server network
</TD> <TD align=left>
 dynamic module creation;
using <tt>WATCH()</tt>;
star topology with the number of modules as parameters</TD></TR>
<TR> <TD align=left> <b>topo</b> </TD> <TD align=left> various topologies
</TD> <TD align=left>
 using NED for creating parametrized topologies</TD></TR>
<TR> <TD align=left> <b>demo</b> </TD> <TD align=left> tour of OMNeT++ samples </TD> <TD align=left> shows how to link several sim. models into one executable</TD></TR>
</table>

<p>

*/