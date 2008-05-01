#include <omnetpp.h>

class Dump : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void dump(cModule *mod, std::string currentIndent);
    virtual std::string props2str(cProperties *props);
};

Define_Module(Dump);

void Dump::initialize()
{
    printf("==============================\n");
    dump(simulation.systemModule(), std::string());
    printf("==============================\n");
}

std::string Dump::props2str(cProperties *props)
{
    if (!props)
        return " [props==NULL]"; // should not happen
    std::string result;
    for (int i=0; i<props->numProperties(); i++)
        result += " " + props->get(i)->info();
    return result;
}

void Dump::dump(cModule *mod, std::string currentIndent)
{
    if (mod==this)
        return;

    const char *indent = currentIndent.c_str();

    printf("%smodule %s: %s {\n", indent, mod->fullPath().c_str(), mod->componentType()->fullName());

    mod->displayString().str(); //important side effect: parse @display into display string; some test cases rely on this taking place here!

    cProperties *props = mod->properties();
    ASSERT(props != NULL);
    bool paramheadingprinted = false;
    for (int i=0; i<props->numProperties(); i++) {
        if (!paramheadingprinted) {printf("%s    parameters:\n", indent);paramheadingprinted=true;}
        printf("%s        %s\n", indent, props->get(i)->info().c_str());
    }
    for (int i=0; i<mod->params(); i++) {
        if (!paramheadingprinted) {printf("%s    parameters:\n", indent);paramheadingprinted=true;}
        printf("%s        %s%s = %s\n", indent, mod->par(i).fullName(), props2str(mod->par(i).properties()).c_str(), mod->par(i).info().c_str());
    }

    bool gateheadingprinted = false;
    for (cModule::GateIterator i(mod); !i.end(); i++) {
        cGate *gate = i();
        if (!gateheadingprinted) {printf("%s    gates:\n", indent);gateheadingprinted=true;}
        printf("%s        %s%s: %s\n", indent, gate->fullName(), props2str(gate->properties()).c_str(), gate->info().c_str());
    }

    bool submodheadingprinted = false;
    for (cModule::SubmoduleIterator submod(mod); !submod.end(); submod++) {
        if (!submodheadingprinted) {printf("%s    submodules:\n", indent);submodheadingprinted=true;}
        dump(submod(), currentIndent+"        ");
    }
    printf("%s}\n", indent);
}

