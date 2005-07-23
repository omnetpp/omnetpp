#include <omnetpp.h>

class WatchTest : public cSimpleModule
{
    Module_Class_Members(WatchTest,cSimpleModule,16384)
    virtual void activity();
};

Define_Module(WatchTest);

void WatchTest::activity()
{
    bool b1 = true;
    bool b2 = false;
    char c = 'a';
    unsigned char uc = 'b';
    signed char sc = 'c';
    short s = -1234;
    unsigned short us = 1234;
    int i = -123456;
    unsigned int ui = 123456;
    long l = -7654321;
    unsigned long ul = 7654321;
    std::string str = "some string";

    WATCH(b1);
    WATCH(b2);
    WATCH(c;
    WATCH(uc);
    WATCH(sc);
    WATCH(s);
    WATCH(us);
    WATCH(i);
    WATCH(ui);
    WATCH(l);
    WATCH(ul);

    for(;;) wait(1);
}

