#include <omnetpp.h>

// The dynamic library created from this file is used for the signals_static_registration_1 test.

using namespace omnetpp;

static simsignal_t inLibrary = cComponent::registerSignal("registeredInLibrary");
