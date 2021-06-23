Wired PHY
=========

A simulation that demonstrates frame transmissions on a wired link,
including aborting, shortening and extending ongoing transmissions.
The crucial components in the model are the Tx and Rx modules (there
are two versions of Rx), all other ingredients are kept minimalistic.

The API it demonstrates is send() with SendOptions, especially
updateTx()/finishTx() in SendOptions.

A network with three nodes (source, switch, sink) that contain simple network
interfaces, for demonstrating how to model things like aborting/preempting
transmissions, and "streaming" packets between two interfaces of a node
to implement cut-through.
