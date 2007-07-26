#include <omnetpp.h>

class Gen : public cSimpleModule
{
    Module_Class_Members(Gen,cSimpleModule,16384)

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
    displayString().setTagArg("i", 0, "block/app");  show();
    displayString().setTagArg("i", 0, "block/app_s");  show();
    displayString().setTagArg("i", 0, "block/app_vs");  show();
    displayString().setTagArg("i", 0, "device/satellite_s");  show();
    displayString().setTagArg("i", 0, "device/satellite");  show();
    displayString().setTagArg("i", 0, "device/satellite_l");  show();
    displayString().setTagArg("i", 0, "device/switch"); show();
    displayString().setTagArg("i", 0, "device/terminal"); show();
    displayString().setTagArg("i", 0, "device/printer"); show();
    displayString().setTagArg("i", 0, "abstract/penguin"); show();
    displayString().setTagArg("i", 0, ""); show();
    displayString().removeTag("i"); show();
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
                displayString().setTagArg("b", 0, *w);
                displayString().setTagArg("b", 1, *h);
                displayString().setTagArg("b", 2, *sh);
                show();
            }
        }
    }
}

void Gen::options()
{
    ev << "OPTIONS -- valid values only:\n";
    displayString().setTagArg("b",0,"50");
    displayString().setTagArg("b",1,"50");
    displayString().setTagArg("b",2,"oval");
    displayString().setTagArg("i",0,"device/laptop");

    const char *fill[] = {"", "-", "yellow", "#00ff00", "@2080ff", NULL};
    const char *outline[] = {"", "-", "blue", "#ff0000", "@ff80a0", NULL};
    const char *borderwidth[] = {"", "0", "1", "6", NULL};
    for (const char **f = fill; *f; f++)
    {
        for (const char **o = outline; *o; o++)
        {
            for (const char **bd = borderwidth; *bd; bd++)
            {
                displayString().setTagArg("b", 3, *f);
                displayString().setTagArg("b", 4, *o);
                displayString().setTagArg("b", 5, *bd);
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
        displayString().setTagArg("b", 3, buf);
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
        displayString().setTagArg("b", 3, buf);
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
        displayString().setTagArg("b", 3, buf);
        show();
    }
}

void Gen::show()
{
    ev << "display string: \"" << displayString().getString() << "\"" << endl;
    wait(0);
}

class Sink : public cSimpleModule
{
    Module_Class_Members(Sink,cSimpleModule,0)
    virtual void handleMessage(cMessage *msg);
};

void Sink::handleMessage(cMessage *msg)
{
    delete msg;
}

Define_Module(Sink);

