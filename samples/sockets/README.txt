Sockets (Hardware-in-the Loop Simulation Demo)
==============================================

This is an example how one can implement hardware-in-the-loop simulation
with OMNeT++/OMNEST. Hardware-in-the-loop means that some external device
is taking part in the simulation. (In other scenarios, the simulation may
implement a device in a real system).

This example contains two simulation models:
 * HTTPNet models a network where several clients send HTTP requests to a
   web server, which replies with the appropriate web pages or 404 Not Found.
   (The simulation is performed on a very abstract level: TCP or IP details
   are not simulated at all, etc.) Most HTTP clients are simulated ones,
   but one of them is a proxy which represents a real-life user (that is,
   you): by pointing your browser to http://localhost:4242 while the
   simulation is running, you can inject real HTTP requests into the
   simulated network.

   To try:
   - start the simulation with the "Run" button on the toolbar
   - fire up your browser, and point it to: http://localhost:4242
   - then watch how your HTTP requests get into the simulated network

 * TelnetNet is similar to HTTPNet, but clients maintain telnet sessions
   to the server. The server echoes every character (in uppercase, to make it
   more interesting, and prints control characters in a readable form).
   Telnet clients are simulated except one, which is a proxy for a real-life
   client. By telnetting to localhost, port 4242 you can send your own
   keypresses over the simulated network to the simulated server.

   To try:
   - start the simulation with the "Run" button on the toolbar
   - on Windows: open a command prompt (Start->Run->cmd.exe), and type
     "telnet localhost 4242"
   - on Linux/Unix: type "telnet localhost 4242" on the shell prompt, then
     put your telnet into character mode (otherwise it'll send your keypresses
     only when you hit Enter.) On my telnet this was done by pressing Ctrl+],
     then typing "mode character" followed by Enter.
   - then type into the telnet program, and watch how your keypresses get
     into the simulated network.

Both simulations are executed in real-time, that is, simulation time is
synchronized to real (wall clock) time. It is recommended that you set
animation speed to near maximum (using the slider at the top of the graphics
window) for the simulation to be able to keep up with real time (you can check
on the performance gauge bar: the simsec/sec gauge should stay around 1.0).

How it works
------------
Hardware-in-the-loop simulation has two ingredients:

 - the simulation has to be synchronized to real time;

 - there has to be a way the simulation can communicate with the real world:
   it has to be able to receive external events, and possibly also to send
   events out of the simulation. The implementation of this external
   communication can be very different from one simulation scenario to another:
   it can be as simple as TCP sockets (as here) or pipes, it can be raw
   communication on an Ethernet NIC or an USB port, or communication with a
   interface card via interrupts and DMA transfer. If you plan to do
   hardware-in-the-loop simulation, you'll most like have to implement your
   own external communication interface.

The word "real-time" means that the simulation is executed in a way such that
simulation time is synchronized to real time: simulating one second
in the model will also take one "real" second. This is in contrast to
"normal" simulation, when we want the simulation to run as fast as possible.
Of course, the CPU has to be fast enough so that the simulation can keep up
with real time.

The external interface used in this example simulation is a single TCP socket.

Hardware-in-the-loop simulation is enabled by the following omnetpp.ini entry:

[General]
scheduler-class = "cSocketRTScheduler"

Interesting functionality is in the cSocketRTScheduler class. Its source code
is in socketrtscheduler.h/cc. It combines synchronizing to real-time with
socket communication.

ExtHTTPClient....


