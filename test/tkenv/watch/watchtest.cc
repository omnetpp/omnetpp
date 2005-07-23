#include <omnetpp.h>
#include "watchtest_m.h"

class WatchTest : public cSimpleModule
{
    Module_Class_Members(WatchTest,cSimpleModule,16384);
    virtual void activity();
};

Define_Module(WatchTest);

//
// Test data structures
//
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

class APolygon : public cPolymorphic
{
  public:
    int n;
    int edgeLen;
    APolygon(int nsides, int edgeLength) {n=nsides; edgeLen=edgeLength;}
    std::string info() const {
        std::stringstream out;
        out << "n=" << n << ", edgeLen=" << edgeLen << " (printed by info())";
        return out.str();
    }
};

std::ostream& operator<<(std::ostream& os, const APolygon& p)
{
    return os << "n=" << p.n << ", edgeLen=" << p.edgeLen << " (printed by op<<)";
}

//
// Main function
//
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

    APolygon poly_WATCH(5,100);
    WATCH(poly_WATCH);

    APolygon poly_WATCH_OBJ(5,100);
    WATCH_OBJ(poly_WATCH_OBJ);

    //GeneratedStruct gs;
    //WATCH(gs), WATCH_OBJ(gs) -- don't work because no op<<, and not cPolymorphic

    GeneratedClass gc;
    GeneratedMessage gm("gm-obj");
    WATCH_OBJ(gc);
    WATCH_OBJ(gm);

    GeneratedClass *gcp = new GeneratedClass;
    GeneratedMessage *gmp = new GeneratedMessage("gmp-obj");
    WATCH_PTR((cPolymorphic*&)gcp);
    WATCH_PTR((cPolymorphic*&)gmp);

    for(;;) wait(1);
}

