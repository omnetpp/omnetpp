#include <omnetpp.h>

class Gen : public cSimpleModule
{
    Module_Class_Members(Gen,cSimpleModule,16384)

    virtual void activity();

    void setTag(const char *tagname, int k, const char *value);
    void removeTag(const char *tagname);
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
    setTag("i", 0, "ball");  show();
    setTag("i", 0, "ball_s");  show();
    setTag("i", 0, "ball_vs");  show();
    setTag("i", 0, "cloud_s");  show();
    setTag("i", 0, "cloud");  show();
    setTag("i", 0, "cloud_l");  show();
    setTag("i", 0, "switch1"); show();
    setTag("i", 0, "switch2"); show();
    setTag("i", 0, "printer"); show();
    setTag("i", 0, "penguin"); show();
    setTag("i", 0, ""); show();
    removeTag("i"); show();
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
                setTag("b", 0, *w);
                setTag("b", 1, *h);
                setTag("b", 2, *sh);
                show();
            }
        }
    }
}

void Gen::options()
{
    ev << "OPTIONS -- valid values only:\n";
    setTag("b",0,"50");
    setTag("b",1,"50");
    setTag("b",2,"oval");
    setTag("i",0,"laptop2");

    const char *fill[] = {"", "-", "yellow", "#00ff00", "@2080ff", NULL};
    const char *outline[] = {"", "-", "blue", "#ff0000", "@ff80a0", NULL};
    const char *borderwidth[] = {"", "0", "1", "6", NULL};
    for (const char **f = fill; *f; f++)
    {
        for (const char **o = outline; *o; o++)
        {
            for (const char **bd = borderwidth; *bd; bd++)
            {
                setTag("o", 0, *f);
                setTag("o", 1, *o);
                setTag("o", 2, *bd);
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
        setTag("o", 0, buf);
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
        setTag("o", 0, buf);
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
        setTag("o", 0, buf);
        show();
    }
}

void Gen::setTag(const char *tagname, int k, const char *value)
{
    cDisplayStringParser p(displayString());
    p.setTagArg(tagname, k, value);
    setDisplayString(p.getString());
}

void Gen::removeTag(const char *tagname)
{
    cDisplayStringParser p(displayString());
    p.removeTag(tagname);
    setDisplayString(p.getString());
}

void Gen::show()
{
    ev << "display string: \"" << displayString() << "\"" << endl;
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

