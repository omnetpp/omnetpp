DYNA
====

Models a client-server application.

Demonstrates:
 - message subclassing
 - dynamic module creation
 - using WATCH()
 - star topology (with the number of modules passed as parameters)

Dyna is a model of a simple client-server network. The network contains
several clients and one server interconnected via a switch.

The number of clients is configurable. Each client computer periodically
connects to the server for data exchange. In each connection, the client
sends a random number of "queries" then closes the connection.

The server handles several incoming connections concurrently. Each
connection is served by a separate process, which is implemented with
a dynamically created simple module. These serverproc modules can
be seen when you open an inspector for the "server" module (right click
on the icon --> Inspect as object) and click on the "Submods" tab.
They can also be seen in the object tree in the main window -- just
open the "server" node in the tree.

Note that the modeled protocol (CONN_REQ, CONN_ACK, etc) is rather a "metaphor"
of a transport-level protocol than a model of any existing protocol. (For one
thing, it doesn't contain three-way handshake, a proper retransmission scheme
etc.) It serves only demonstration purposes.

The module types are:
  Client:
    A client computer which periodically connects to the
    server for data exchange.

  Switch:
    A very simple module which models the network between
    the server and the clients.

  Server:
    Models a simple server which accepts connections from the
    client computers. It serves multiple connections at a time;
    each connection is handled by a ServerProcess module, created
    on demand.

  ServerProcess:
    Handles one connection in the server.

  ClientServer:
    Model of the network, consisting of several clients, a server
    and a switch. The number of clients is a parameter.


