#include <omnetpp.h>

class Dump : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void dump(cModule *mod, int level);
};

Define_Module(Dump);

void Dump::initialize()
{
    dump(simulation.systemModule(), 0);
    printf("done\n");
}

void Dump::dump(cModule *mod, int level)
{
    if (mod==this)
        return;

    printf("module %s: %s\n", mod->fullPath().c_str(), mod->className());
    if (strlen(mod->displayString().toString())>0)
        printf("  display: %s\n", mod->displayString().toString());

    cProperties *props = mod->properties();
    for (int i=0; i<props->numProperties(); i++)
        printf("  %s\n", props->get(i)->info().c_str());

    for (int i=0; i<mod->params(); i++)
        printf("  P %s = %s\n", mod->par(i).fullName(), mod->par(i).info().c_str());

    for (int i=0; i<mod->gates(); i++)
        if (mod->gate(i))
            printf("  G %2d %s: %s\n", i, mod->gate(i)->fullName(), mod->gate(i)->info().c_str());

    for (cSubModIterator submod(*mod); !submod.end(); submod++)
        dump(submod(), level+1);
}

