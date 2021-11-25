#include <omnetpp.h>

using namespace omnetpp;

namespace testlib {

class Dump : public cSimpleModule
{
  protected:
    bool printClassNames;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void dump(cModule *mod, std::string currentIndent);
    virtual std::string props2str(cProperties *props);
};

Define_Module(Dump);

void Dump::initialize()
{
    scheduleAt(1, new cMessage());
}

void Dump::handleMessage(cMessage *msg)
{
    delete msg;

    printClassNames = par("printClassNames").boolValue();
    printf("==============================\n");
    dump(getSimulation()->getSystemModule(), std::string());
    printf("==============================\n");
}

std::string Dump::props2str(cProperties *props)
{
    if (!props)
        return " [props==nullptr]";  // should not happen
    std::string result;
    for (int i = 0; i < props->getNumProperties(); i++)
        result += " " + props->get(i)->str();
    return result;
}

void Dump::dump(cModule *mod, std::string currentIndent)
{
    if (mod == this)
        return;

    const char *indent = currentIndent.c_str();

    printf("%smodule %s: %s", indent, mod->getFullPath().c_str(), mod->getComponentType()->getFullName());
    if (!opp_isempty(mod->getDisplayName()))
        printf(" \"%s\"", mod->getDisplayName());
    if (printClassNames)
        printf(" (%s)", mod->getClassName());
    printf(" {\n");

    mod->getDisplayString().str();  // important side effect: parse @display into display string; some test cases rely on this taking place here!

    cProperties *props = mod->getProperties();
    ASSERT(props != nullptr);
    bool paramheadingprinted = false;
    for (int i = 0; i < props->getNumProperties(); i++) {
        if (!paramheadingprinted) {
            printf("%s    parameters:\n", indent);
            paramheadingprinted = true;
        }
        printf("%s        %s\n", indent, props->get(i)->str().c_str());
    }
    for (int i = 0; i < mod->getNumParams(); i++) {
        if (!paramheadingprinted) {
            printf("%s    parameters:\n", indent);
            paramheadingprinted = true;
        }
        printf("%s        %s%s = %s\n", indent, mod->par(i).getFullName(), props2str(mod->par(i).getProperties()).c_str(), mod->par(i).str().c_str());
    }

    bool gateheadingprinted = false;
    for (cModule::GateIterator it(mod); !it.end(); it++) {
        cGate *gate = *it;
        if (!gateheadingprinted) {
            printf("%s    gates:\n", indent);
            gateheadingprinted = true;
        }
        printf("%s        %s%s: %s", indent, gate->getFullName(), props2str(gate->getProperties()).c_str(), gate->str().c_str());
        if (printClassNames && gate->getChannel() != nullptr)
            printf(" (%s)", gate->getChannel()->getClassName());
        printf("\n");
    }

    bool submodheadingprinted = false;
    for (cModule::SubmoduleIterator it(mod); !it.end(); it++) {
        if (!submodheadingprinted) {
            printf("%s    submodules:\n", indent);
            submodheadingprinted = true;
        }
        dump(*it, currentIndent+"        ");
    }
    printf("%s}\n", indent);
}

}
