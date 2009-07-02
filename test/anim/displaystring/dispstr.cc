#include <omnetpp.h>

class Gen : public cSimpleModule
{
  public:
    Gen() : cSimpleModule(16384) {}

    virtual void activity();

    void show();

    void icons();
    void boxes();
    void options();
    void hue();
    void saturation();
    void brightness();
};

Define_Module(Gen);

void Gen::activity()
{
    const char *what = par("what");
    simtime_t startTime = par("startTime");

    wait(startTime);

    if (!strcmp(what,"icons"))
        icons();
    else if (!strcmp(what,"boxes"))
        boxes();
    else if (!strcmp(what,"options"))
        options();
    else if (!strcmp(what,"hue"))
        hue();
    else if (!strcmp(what,"saturation"))
        saturation();
    else if (!strcmp(what,"brightness"))
        brightness();
    else
        error("wrong value '%s'", what);
}

void Gen::icons()
{
    ev << "ICONS -- valid values only:\n";
    getDisplayString().setTagArg("i", 0, "block/app");  show();
    getDisplayString().setTagArg("i", 0, "block/app_s");  show();
    getDisplayString().setTagArg("i", 0, "block/app_vs");  show();
    getDisplayString().setTagArg("i", 0, "device/satellite_s");  show();
    getDisplayString().setTagArg("i", 0, "device/satellite");  show();
    getDisplayString().setTagArg("i", 0, "device/satellite_l");  show();
    getDisplayString().setTagArg("i", 0, "device/switch"); show();
    getDisplayString().setTagArg("i", 0, "device/terminal"); show();
    getDisplayString().setTagArg("i", 0, "device/printer"); show();
    getDisplayString().setTagArg("i", 0, "abstract/penguin"); show();
    getDisplayString().setTagArg("i", 0, ""); show();
    getDisplayString().removeTag("i"); show();
}

void Gen::boxes()
{
    ev << "BOXES -- valid values only:\n";
    const char *shape[] =  {"", "rect", "oval", NULL};
    const char *width[] =  {"", "0", "1", "20", "40", "80", NULL};
    const char *height[] = {"", "0", "1", "20", "40", "80", NULL};

    for (const char **sh = shape; *sh; sh++)
    {
        for (const char **w = width; *w; w++)
        {
            for (const char **h = height; *h; h++)
            {
                getDisplayString().setTagArg("b", 0, *w);
                getDisplayString().setTagArg("b", 1, *h);
                getDisplayString().setTagArg("b", 2, *sh);
                show();
            }
        }
    }
}

void Gen::options()
{
    ev << "OPTIONS -- valid values only:\n";
    getDisplayString().setTagArg("b",0,"50");
    getDisplayString().setTagArg("b",1,"50");
    getDisplayString().setTagArg("b",2,"oval");
    getDisplayString().setTagArg("i",0,"device/laptop");

    const char *fill[] = {"", "-", "yellow", "#00ff00", "@2080ff", NULL};
    const char *outline[] = {"", "-", "blue", "#ff0000", "@ff80a0", NULL};
    const char *borderwidth[] = {"", "0", "1", "6", NULL};
    for (const char **f = fill; *f; f++)
    {
        for (const char **o = outline; *o; o++)
        {
            for (const char **bd = borderwidth; *bd; bd++)
            {
                getDisplayString().setTagArg("b", 3, *f);
                getDisplayString().setTagArg("b", 4, *o);
                getDisplayString().setTagArg("b", 5, *bd);
                show();
            }
        }
    }
}

void Gen::hue()
{
    ev << "HSB hue -- valid values only:\n";
    for (int i=0; i<255; i+=8)
    {
        char buf[10];
        sprintf(buf, "@%2.2xffff", i);
        getDisplayString().setTagArg("b", 3, buf);
        show();
    }
}

void Gen::saturation()
{
    ev << "HSB saturation -- valid values only:\n";
    for (int i=0; i<255; i+=8)
    {
        char buf[10];
        sprintf(buf, "@40%2.2xff", i);
        getDisplayString().setTagArg("b", 3, buf);
        show();
    }
}

void Gen::brightness()
{
    ev << "HSB brightness -- valid values only:\n";
    for (int i=0; i<255; i+=8)
    {
        char buf[10];
        sprintf(buf, "@4080%2.2x", i);
        getDisplayString().setTagArg("b", 3, buf);
        show();
    }
}

void Gen::show()
{
    ev << "display string: \"" << getDisplayString().str() << "\"" << endl;
    wait(0);
}

class Sink : public cSimpleModule
{
  protected:
    virtual void handleMessage(cMessage *msg);
};

void Sink::handleMessage(cMessage *msg)
{
    delete msg;
}

Define_Module(Sink);

