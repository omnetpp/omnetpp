#include <omnetpp.h>

class WatchTest : public cSimpleModule
{
    Module_Class_Members(WatchTest,cSimpleModule,16384);
    virtual void activity();
};

Define_Module(WatchTest);

struct Point
{
    int x; int y;
    Point(int x1, int y1) {x=x1; y=y1;}
};

std::ostream& operator<<(std::ostream& os, const Point& p)
{
    return os << "(" << p.x << "," << p.y << ")";
}

std::istream& operator>>(std::istream& is, Point& p)
{
    char dummy;
    return is >> dummy >> p.x >> dummy >> p.y >> dummy;
}

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
    WATCH(c);
    WATCH(uc);
    WATCH(sc);
    WATCH(s);
    WATCH(us);
    WATCH(i);
    WATCH(ui);
    WATCH(l);
    WATCH(ul);
    WATCH(str);

    Point point(100,200);
    WATCH(point);

    Point point_rw(100,200);
    WATCH_RW(point_rw);

    for(;;) wait(1);
}

