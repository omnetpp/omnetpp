#include "cgatewrapper.h"


void cTransmissionGateWrapper::refresh()
{
    datarateChannel = check_and_cast<cDatarateChannel *>(gate->getTransmissionChannel());

    // compute leadingDelay, the propagation delay up to the transmission channel
    leadingDelay = 0;
    for (const cGate *g = gate; g; g = g->getNextGate())
    {
        cChannel *channel = g->getChannel();
        if (channel && channel->isTransmissionChannel())
            break;
        if (dynamic_cast<cDelayChannel *>(channel))
            leadingDelay += ((cDelayChannel *)channel)->getDelay();
    }
}

