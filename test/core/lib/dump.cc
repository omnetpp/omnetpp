#include <omnetpp.h>

using namespace omnetpp;

namespace testlib {

class Dump : public cSimpleModule
{
  protected:
    bool printClassNames;
    bool printParamAssignmentLocations;

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
    printParamAssignmentLocations = par("printParamAssignmentLocations").boolValue();
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
    bool parametersHeadingPrinted = false;
    for (int i = 0; i < props->getNumProperties(); i++) {
        if (!parametersHeadingPrinted) {
            printf("%s    parameters:\n", indent);
            parametersHeadingPrinted = true;
        }
        printf("%s        %s\n", indent, props->get(i)->str().c_str());
    }
    for (int i = 0; i < mod->getNumParams(); i++) {
        if (!parametersHeadingPrinted) {
            printf("%s    parameters:\n", indent);
            parametersHeadingPrinted = true;
        }
        cPar& par = mod->par(i);
        std::string valueString = par.str();
        if (par.isVolatile()) {
            try {
                valueString += " <volatile value>: " + par.getValue().str();
            }
            catch (std::exception& e) {
                valueString += std::string(" <error>: ") + e.what();
            }
        }
        std::string locationString;
        if (printParamAssignmentLocations) {
            std::string loc = par.getSourceLocation();
            if (loc.empty())
                loc = "n/a";
            size_t pos = loc.rfind("/");
            if (pos != std::string::npos)
                loc = "..." + loc.substr(pos);  // abbreviate
            locationString = "  (from " + loc + ")";
        }
        printf("%s        %s%s = %s%s\n", indent, mod->par(i).getFullName(), props2str(par.getProperties()).c_str(), valueString.c_str(), locationString.c_str());
    }

    bool gatesHeadingPrinted = false;
    for (cModule::GateIterator it(mod); !it.end(); it++) {
        cGate *gate = *it;
        if (!gatesHeadingPrinted) {
            printf("%s    gates:\n", indent);
            gatesHeadingPrinted = true;
        }
        printf("%s        %s%s: %s", indent, gate->getFullName(), props2str(gate->getProperties()).c_str(), gate->str().c_str());
        if (printClassNames && gate->getChannel() != nullptr)
            printf(" (%s)", gate->getChannel()->getClassName());
        printf("\n");
    }

    bool submodulesHeadingPrinted = false;
    for (cModule::SubmoduleIterator it(mod); !it.end(); it++) {
        if (!submodulesHeadingPrinted) {
            printf("%s    submodules:\n", indent);
            submodulesHeadingPrinted = true;
        }
        dump(*it, currentIndent+"        ");
    }
    printf("%s}\n", indent);
}

}
