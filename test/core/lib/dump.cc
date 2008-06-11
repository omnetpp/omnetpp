#include <omnetpp.h>

class Dump : public cSimpleModule
{
  protected:
    bool printClassNames;

  protected:
    virtual void initialize();
    virtual void dump(cModule *mod, std::string currentIndent);
    virtual std::string props2str(cProperties *props);
};

Define_Module(Dump);

void Dump::initialize()
{
    printClassNames = par("printClassNames").boolValue();
    printf("==============================\n");
    dump(simulation.getSystemModule(), std::string());
    printf("==============================\n");
}

std::string Dump::props2str(cProperties *props)
{
    if (!props)
        return " [props==NULL]"; // should not happen
    std::string result;
    for (int i=0; i<props->getNumProperties(); i++)
        result += " " + props->get(i)->info();
    return result;
}

void Dump::dump(cModule *mod, std::string currentIndent)
{
    if (mod==this)
        return;

    const char *indent = currentIndent.c_str();

    printf("%smodule %s: %s", indent, mod->getFullPath().c_str(),mod->getComponentType()->getFullName());
    if (printClassNames)
        printf(" (%s)", mod->getClassName());
    printf(" {\n");

    mod->getDisplayString().str(); //important side effect: parse @display into display string; some test cases rely on this taking place here!

    cProperties *props = mod->getProperties();
    ASSERT(props != NULL);
    bool paramheadingprinted = false;
    for (int i=0; i<props->getNumProperties(); i++) {
        if (!paramheadingprinted) {printf("%s    parameters:\n", indent);paramheadingprinted=true;}
        printf("%s        %s\n", indent, props->get(i)->info().c_str());
    }
    for (int i=0; i<mod->getNumParams(); i++) {
        if (!paramheadingprinted) {printf("%s    parameters:\n", indent);paramheadingprinted=true;}
        printf("%s        %s%s = %s\n", indent, mod->par(i).getFullName(), props2str(mod->par(i).getProperties()).c_str(), mod->par(i).info().c_str());
    }

    bool gateheadingprinted = false;
    for (cModule::GateIterator i(mod); !i.end(); i++) {
        cGate *gate = i();
        if (!gateheadingprinted) {printf("%s    gates:\n", indent);gateheadingprinted=true;}
        printf("%s        %s%s: %s", indent, gate->getFullName(), props2str(gate->getProperties()).c_str(), gate->info().c_str());
        if (printClassNames && gate->getChannel()!=NULL)
            printf(" (%s)", gate->getChannel()->getClassName());
        printf("\n");
    }

    bool submodheadingprinted = false;
    for (cModule::SubmoduleIterator submod(mod); !submod.end(); submod++) {
        if (!submodheadingprinted) {printf("%s    submodules:\n", indent);submodheadingprinted=true;}
        dump(submod(), currentIndent+"        ");
    }
    printf("%s}\n", indent);
}

