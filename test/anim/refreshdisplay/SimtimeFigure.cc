#include "SimtimeFigure.h"

Register_Figure("simtime", SimtimeFigure);

SimtimeFigure::SimtimeFigure()
{
    setText("SimtimeFigure::refreshDisplay() not yet called -- SHOULD NOT HAPPEN!");
}

void SimtimeFigure::throwException()
{
    exceptionThrown = true;
    throw cRuntimeError("BANGGG from SimtimeFigure");
}

void SimtimeFigure::refreshDisplay()
{
    if (throwExceptionInRefresh)
        throwException();

    if (exceptionThrown)
        throw cRuntimeError("*** GUI ERROR *** refreshDisplay() invoked after a simulation error. This should not have happened");

    refreshCount++;

    std::stringstream os;
    os << "refresh #" << refreshCount << ", at event #" << getSimulation()->getEventNumber() << " t=" << simTime();
    setText(os.str().c_str());
}

