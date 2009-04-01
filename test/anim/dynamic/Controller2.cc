#include <stdlib.h>
#include <stdarg.h>
#include <vector>
#include <omnetpp.h>


#ifdef _WIN32
#ifndef vsnprintf
#define vsnprintf _vsnprintf
#endif
#endif

std::string stringf(const char *format, ...)
{
    const int BUFLEN = 1024;
    char buffer[BUFLEN];
    va_list va;
    va_start(va, format);
    vsnprintf(buffer, BUFLEN, format, va);
    buffer[BUFLEN-1] = '\0';
    va_end(va);
    return buffer;
}


class Controller2 : public cSimpleModule
{
  public:
    Controller2() : cSimpleModule(16384) {}
    virtual void activity();
};

Define_Module(Controller2);


void Controller2::activity()
{
    cModuleType *type = cModuleType::find("Node");
    std::vector<cModule*> nodes;

    for (int i=0; true; i++)
    {
        if (intuniform(0,20)==0)
        {
            // create a node
            cModule *node = type->createScheduleInit(stringf("node%d",i).c_str(), getParentModule());
            nodes.push_back(node);
            cDisplayString& d = node->getDisplayString();
            d.setTagArg("p", 0, intuniform(0,600));
            d.setTagArg("p", 1, intuniform(0,400));
            d.setTagArg("t", 0, "NEW");
        }

        if (intuniform(0,20)==0 && nodes.size()>0) {
            // delete a node
            int j = intuniform(0, nodes.size()-1);
            nodes[j]->deleteModule();
            nodes.erase(nodes.begin()+j);
        }

        if (intuniform(0,5)==0) {
            // create connection
        }

        if (intuniform(0,5)==0) {
            // delete connection
        }

        // move nodes
        for (int j=0; j<nodes.size(); j++)
        {
            cDisplayString& d = nodes[j]->getDisplayString();
            if (true)
            {
                // move node
                int x = atoi(d.getTagArg("p",0));
                int y = atoi(d.getTagArg("p",1));
                d.setTagArg("p", 0, x+intuniform(-5,5));
                d.setTagArg("p", 1, y+intuniform(-3,3));
            }
            if (intuniform(0,3)==0)
            {
                // change text
                d.setTagArg("t", 0, stringf("changed at %d",i).c_str());
            }
        }

        wait(1);
    }
}

