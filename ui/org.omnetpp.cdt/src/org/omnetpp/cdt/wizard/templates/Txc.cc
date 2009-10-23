${bannercomment}

#include "Txc.h"

#if($namespace!="")namespace ${namespace} {#end

Define_Module(Txc);

void Txc::initialize()
{
    if (par("sendInitialMessage").boolValue())
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

#if($namespace!="")}; // namespace#end

