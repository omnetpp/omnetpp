#include <omnetpp.h>

using namespace omnetpp;

class DisplayStringMessage : public cMessage
{
    const char *dispStr = "";
public:
    using cMessage::cMessage;

    void setDisplayString(const char *ds) { dispStr = ds; }
    const char *getDisplayString() const override { return dispStr; }
};

class Gen : public cSimpleModule
{
  public:
    Gen() : cSimpleModule(16384) {}

    virtual void activity() override;

    void show();

    void icons();
    void boxes();
    void options();
    void hue();
    void saturation();
    void brightness();
    void messages();
};

Define_Module(Gen);

void Gen::activity()
{
    const char *what = par("what");
    simtime_t startTime = par("startTime");

    wait(startTime);

    if (!strcmp(what, "icons"))
        icons();
    else if (!strcmp(what, "boxes"))
        boxes();
    else if (!strcmp(what, "options"))
        options();
    else if (!strcmp(what, "hue"))
        hue();
    else if (!strcmp(what, "saturation"))
        saturation();
    else if (!strcmp(what, "brightness"))
        brightness();
    else if (!strcmp(what, "messages"))
        messages();
    else
        error("wrong value '%s'", what);
}

void Gen::icons()
{
    EV << "ICONS -- valid values only:\n";
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
    EV << "BOXES -- valid values only:\n";
    const char *shape[] =  {"", "rect", "oval", nullptr};
    const char *width[] =  {"", "0", "1", "20", "40", "80", nullptr};
    const char *height[] = {"", "0", "1", "20", "40", "80", nullptr};

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
    EV << "OPTIONS -- valid values only:\n";
    getDisplayString().setTagArg("b",0,"50");
    getDisplayString().setTagArg("b",1,"50");
    getDisplayString().setTagArg("b",2,"oval");
    getDisplayString().setTagArg("i",0,"device/laptop");

    const char *fill[] = {"", "-", "yellow", "#00ff00", "@2080ff", nullptr};
    const char *outline[] = {"", "-", "blue", "#ff0000", "@ff80a0", nullptr};
    const char *borderwidth[] = {"", "0", "1", "6", nullptr};
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
    EV << "HSB hue -- valid values only:\n";
    for (int i = 0; i < 255; i += 8) {
        char buf[10];
        snprintf(buf, sizeof(buf), "@%2.2xffff", i);
        getDisplayString().setTagArg("b", 3, buf);
        show();
    }
}

void Gen::saturation()
{
    EV << "HSB saturation -- valid values only:\n";
    for (int i = 0; i < 255; i += 8) {
        char buf[10];
        snprintf(buf, sizeof(buf), "@40%2.2xff", i);
        getDisplayString().setTagArg("b", 3, buf);
        show();
    }
}

void Gen::brightness()
{
    EV << "HSB brightness -- valid values only:\n";
    for (int i = 0; i < 255; i += 8) {
        char buf[10];
        snprintf(buf, sizeof(buf), "@4080%2.2x", i);
        getDisplayString().setTagArg("b", 3, buf);
        show();
    }
}

void Gen::messages()
{
    static const char *const displayStrings[] = {
        "", "", "", "", "", "", "", "", // 8 colored kinds with no displaystring
        "b=15,15,rect,white,kind,5",
        "b=",
        "b=20",
        "b=,20",
        "b=25,15,rect,kind,invalidcolor",
        "b=15,25,rect,invalidcolor,kind",
        "b=,,,kind",
        "b=15,15,oval,yellow,green,6",
        "i=msg/package",
        "i=msg/package;is=vl",
        "i=msg/packet,lightgreen",
        "i=msg/mail,purple,100",
        "b=25,25,oval;i=misc/cloud_s",
        "i=device/antennatower_vl",
        "b=${3*10},${100/100}5,r${'e'+'c'}t}", // NED expressions
    };

    EV << "Messages:\n";

    char msgname[32];
    for (unsigned int i = 0; i < sizeof(displayStrings) / sizeof(const char *); ++i) {
        snprintf(msgname, sizeof(msgname), "job-%d", i);
        DisplayStringMessage *msg = new DisplayStringMessage(msgname, i);
        msg->setDisplayString(displayStrings[i]);
        EV << "Sending message #" << i << " with display string: \"" << displayStrings[i] << "\".\n";
        send(msg, "out");
        wait(0);
    }
}

void Gen::show()
{
    EV << "display string: \"" << getDisplayString().str() << "\"" << endl;
    wait(0);
}

class Sink : public cSimpleModule
{
  protected:
    virtual void handleMessage(cMessage *msg) override;
};

void Sink::handleMessage(cMessage *msg)
{
    delete msg;
}

Define_Module(Sink);
