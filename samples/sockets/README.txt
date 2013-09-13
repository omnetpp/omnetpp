Sockets (Hardware-in-the Loop Simulation Demo)
==============================================

This is an example how one can implement hardware-in-the-loop simulation
with OMNeT++/OMNEST. Hardware-in-the-loop means that some external device
is taking part in the simulation. (In other scenarios, the simulation
program may emulate a device in a real system).

   |  *** ATTENTION Windows users ***
   |
   |  This program will open a socket for incoming TCP connections.
   |  Windows may pop up a dialog asking whether you want to allow
   |  the "sockets"  program to accept TCP connections; choose yes.
   |  If you have already mistakenly told Windows to block the program
   |  from accepting connections, it won't ask you any more -- then
   |  you'll have to go to the "Windows Firewall with Advanced Security"
   |  dialog (it is called like that on Windows 7), and delete the rules
   |  that refer to the "sockets" program from the "Inbound Rules" table.

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
synchronized to real (wall clock) time.

It is recommended that you set animation speed to near maximum (using the
slider at the top of the graphics window) for the simulation to be able
to keep up with real time. You can check on the performance gauge bar:
the simsec/sec gauge should stay around 1.0.

More detailed documentation is available in the htdocs/ subdirectory of the
simulation. You can conveniently view it at the http://localhost:4242 URL
while the HTTPNet simulation is running, or you can simply open index.html
in that directory in the browser as a file.


