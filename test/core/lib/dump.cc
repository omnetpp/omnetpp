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
    virtual void dump(cModule *mod, std::ostream& out, std::string indent);
    virtual std::string formatProperties(cProperties *props);
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
    dump(getSimulation()->getSystemModule(), std::cout, "");
    printf("==============================\n");
}

std::string Dump::formatProperties(cProperties *props)
{
    if (!props)
        return " [props==nullptr]";  // should not happen
    std::string result;
    for (int i = 0; i < props->getNumProperties(); i++)
        result += " " + props->get(i)->str();
    return result;
}

void Dump::dump(cModule *mod, std::ostream& out, std::string indent)
{
    if (mod == this)
        return;

    out << indent << "module " << mod->getFullPath() << ": " << mod->getComponentType()->getFullName();
    if (!opp_isempty(mod->getDisplayName()))
        out << " \"" << mod->getDisplayName() << "\"";
    if (printClassNames)
        out << " (" << mod->getClassName() << ")";
    out << " {\n";

    mod->getDisplayString().str();  // important side effect: parse @display into display string; some test cases rely on this taking place here!

    cProperties *props = mod->getProperties();
    ASSERT(props != nullptr);
    bool parametersHeadingPrinted = false;
    for (int i = 0; i < props->getNumProperties(); i++) {
        if (!parametersHeadingPrinted) {
            out << indent << "    parameters:\n";
            parametersHeadingPrinted = true;
        }
        out << indent << "        " << props->get(i)->str() << "\n";
    }
    for (int i = 0; i < mod->getNumParams(); i++) {
        if (!parametersHeadingPrinted) {
            out << indent << "    parameters:\n";
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
        out << indent << "        " << mod->par(i).getFullName() << formatProperties(par.getProperties()) << " = " << valueString << locationString << "\n";
    }

    bool gatesHeadingPrinted = false;
    for (cModule::GateIterator it(mod); !it.end(); it++) {
        cGate *gate = *it;
        if (!gatesHeadingPrinted) {
            out << indent << "    gates:\n";
            gatesHeadingPrinted = true;
        }
        out << indent << "        " << gate->getFullName() << formatProperties(gate->getProperties()) << ": " << gate->str();
        if (printClassNames && gate->getChannel() != nullptr)
            out << " (" << gate->getChannel()->getClassName() << ")";
        out << "\n";
    }

    bool submodulesHeadingPrinted = false;
    for (cModule::SubmoduleIterator it(mod); !it.end(); it++) {
        if (!submodulesHeadingPrinted) {
            out << indent << "    submodules:\n";
            submodulesHeadingPrinted = true;
        }
        dump(*it, out, indent + "        ");
    }
    out << indent << "}\n";
}

}
