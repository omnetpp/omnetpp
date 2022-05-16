#include <cstdio>
#include <cstring>
#include <omnetpp.h>
#include "tictoc17_m.h"

using namespace omnetpp;

class Txc17 : public cSimpleModule
{
private:
    simsignal_t arrivalSignal;

protected:
    virtual TicTocMsg17 *generateMessage();
    virtual void forwardMessage(TicTocMsg17 *msg);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Txc17);

void Txc17::initialize()
{
    arrivalSignal = registerSignal("arrival");
    // Module 0 sends the first message
    if (getIndex() == 0) {
        // Boot the process scheduling the initial message as a self-message.
        TicTocMsg17 *msg = generateMessage();
        scheduleAt(0.0, msg);
    }
}

void Txc17::handleMessage(cMessage *msg)
{
    TicTocMsg17 *ttmsg = check_and_cast<TicTocMsg17 *>(msg);

    if (ttmsg->getDestination() == getIndex()) {
        // Message arrived
        int hopcount = ttmsg->getHopCount();
        // send a signal
        emit(arrivalSignal, hopcount);

        if (hasGUI()) {
            char label[50];
            // Write last hop count to string
            snprintf(label, sizeof(label), "last hopCount = %d", hopcount);
            // Get pointer to figure
            cCanvas *canvas = getParentModule()->getCanvas();
            cTextFigure *textFigure = check_and_cast<cTextFigure*>(canvas->getFigure("lasthopcount"));
            // Update figure text
            textFigure->setText(label);
        }

        EV << "Message " << ttmsg << " arrived after " << hopcount << " hops.\n";
        bubble("ARRIVED, starting new one!");

        delete ttmsg;

        // Generate another one.
        EV << "Generating another message: ";
        TicTocMsg17 *newmsg = generateMessage();
        EV << newmsg << endl;
        forwardMessage(newmsg);
    }
    else {
        // We need to forward the message.
        forwardMessage(ttmsg);
    }
}

TicTocMsg17 *Txc17::generateMessage()
{
    // Produce source and destination addresses.
    int src = getIndex();
    int n = getVectorSize();
    int dest = intuniform(0, n-2);
    if (dest >= src)
        dest++;

    char msgname[20];
    snprintf(msgname, sizeof(msgname), "tic-%d-to-%d", src, dest);

    // Create message object and set source and destination field.
    TicTocMsg17 *msg = new TicTocMsg17(msgname);
    msg->setSource(src);
    msg->setDestination(dest);
    return msg;
}

void Txc17::forwardMessage(TicTocMsg17 *msg)
{
    // Increment hop count.
    msg->setHopCount(msg->getHopCount()+1);

    // Same routing as before: random gate.
    int n = gateSize("gate");
    int k = intuniform(0, n-1);

    EV << "Forwarding message " << msg << " on gate[" << k << "]\n";
    send(msg, "gate$o", k);
}
