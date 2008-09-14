{{copyright}}

#include "Txc.h"

{{namespace:}}namespace {{namespace}} {

Define_Module(Txc);

void Txc::initialize()
{
    // the module named "tic" generates the initial message
    if (strcmp(getName(), "tic") == 0)
    {
        cMessage *msg = new cMessage("tictocMsg");
        send(msg, "out");
    }
}

void Txc::handleMessage(cMessage *msg)
{
    // just send back the message we received
    send(msg, "out");
}

{{namespace:}}}; // namespace

