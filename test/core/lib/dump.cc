#include <omnetpp.h>

class Dump : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void dump(cModule *mod, std::string currentIndent);
};

Define_Module(Dump);

void Dump::initialize()
{
    printf("==============================\n");
    dump(simulation.systemModule(), std::string());
    printf("==============================\n");
}

void Dump::dump(cModule *mod, std::string currentIndent)
{
    if (mod==this)
        return;

    const char *indent = currentIndent.c_str();

    printf("%smodule %s: %s {\n", indent, mod->fullPath().c_str(), mod->componentType()->fullName());
    if (strlen(mod->displayString().toString())>0)
        printf("%s      display: %s\n", indent, mod->displayString().toString());

    cProperties *props = mod->properties();
    bool paramheadingprinted = false;
    for (int i=0; i<props->numProperties(); i++) {
            if (!paramheadingprinted) {printf("%s    parameters:\n", indent);paramheadingprinted=true;}
        printf("%s        %s\n", indent, props->get(i)->info().c_str());
    }
    for (int i=0; i<mod->params(); i++) {
        if (!paramheadingprinted) {printf("%s    parameters:\n", indent);paramheadingprinted=true;}
        printf("%s        %s = %s\n", indent, mod->par(i).fullName(), mod->par(i).info().c_str());
    }

    bool gateheadingprinted = false;
    for (int i=0; i<mod->gates(); i++) {
        if (mod->gate(i)) {
            if (!gateheadingprinted) {printf("%s    gates:\n", indent);gateheadingprinted=true;}
            printf("%s        %2d %s: %s\n", indent, i, mod->gate(i)->fullName(), mod->gate(i)->info().c_str());
        }
    }

    bool submodheadingprinted = false;
    for (cSubModIterator submod(*mod); !submod.end(); submod++) {
        if (!submodheadingprinted) {printf("%s    submodules:\n", indent);submodheadingprinted=true;}
        dump(submod(), currentIndent+"        ");
    }
    printf("%s}\n", indent);
}

