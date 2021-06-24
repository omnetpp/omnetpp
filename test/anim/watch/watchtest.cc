#include <omnetpp.h>
#include "watchtest_m.h"

using namespace omnetpp;

class WatchTest : public cSimpleModule
{
  public:
    WatchTest() : cSimpleModule(16384) {}
    virtual void activity() override;
    virtual void handleParameterChange(const char *parname) override;
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

class APolygon : public cObject
{
  public:
    int n;
    int edgeLen;
    APolygon(int nsides, int edgeLength) {n=nsides; edgeLen=edgeLength;}
    std::string str() const override {
        std::stringstream out;
        out << "n=" << n << ", edgeLen=" << edgeLen << " (printed by str())";
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
    //
    // Basic types
    //
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

    //
    // Structs/classes via op<<
    //
    Point point(100, 200);
    WATCH(point);

    Point point_rw(100, 200);
    WATCH_RW(point_rw);

    //
    // Structs/classes via cObject and str(); no structdesc.
    //
    APolygon poly_WATCH(5, 100);
    WATCH(poly_WATCH);

    APolygon poly_WATCH_OBJ(5, 100);
    WATCH_OBJ(poly_WATCH_OBJ);

    //
    // Generated structs/classes (with structdesc.)
    //

    // GeneratedStruct gs;
    // WATCH(gs), WATCH_OBJ(gs) -- don't work because no op<<, and not cObject

    GeneratedClass gc;
    GeneratedMessage gm("gm-obj");
    GeneratedPacket gp("gp-obj");
    WATCH_OBJ(gc);
    WATCH_OBJ(gm);
    WATCH_OBJ(gp);

    GeneratedClass *gcp = new GeneratedClass;
    cMessage *gmp = new GeneratedMessage("gmp-obj");
    cPacket *gpp = new GeneratedPacket("gpp-obj");
    WATCH_PTR(gcp);
    WATCH_PTR(gmp);
    WATCH_PTR(gpp);

    cObject *no = nullptr;
    WATCH_PTR(no);

    cObject *dis = this;
    WATCH_PTR(dis);

    // pointer is captured by reference, and descriptor is not supposed to be cached
    cObject *changing = nullptr;
    WATCH_PTR(changing);

    // int *wrongp = (int *)gcp;
    // WATCH_PTR(wrongp); -- this has to give a compile error

    //
    // Vectors, lists and maps
    //
    std::vector<int> vi;
    vi.push_back(2);
    vi.push_back(3);
    vi.push_back(5);
    vi.push_back(7);
    WATCH_VECTOR(vi);

    std::list<std::string> ls;
    ls.push_back("two");
    ls.push_back("three");
    ls.push_back("five");
    ls.push_back("seven");
    WATCH_LIST(ls);

    std::map<int, std::string> m;
    m[1] = "one";
    m[2] = "two";
    m[3] = "three";
    WATCH_MAP(m);

    std::set<std::string> ss;
    ss.insert("Dopey");
    ss.insert("Sleepy");
    ss.insert("Grumpy");
    ss.insert("Sneezy");
    ss.insert("Happy");
    ss.insert("Bashful");
    ss.insert("Doc");
    WATCH_SET(ss);

    // TBD: PTRVECTOR, PTRMAP etc.
    for ( ; ; ) {
        wait(1);
        changing = new cPacket("Packet");
        wait(1);
        delete changing;
        changing = nullptr;
        wait(1);
        changing = new cMessage("Message");
        wait(1);
        delete changing;
        changing = nullptr;
    }
}

void WatchTest::handleParameterChange(const char *parname)
{
    EV << "handleParameterChange(): " << parname << "\n";
    new cMessage("dummymsg");  // test that this module will be the owner
}


// Testing that watching an indirect ancestor isn't causing any trouble.
class Sub : public cSimpleModule
{
  public:
    Sub() : cSimpleModule(16384) {}
    virtual void activity() override {
        cObject *subdis = this;
        WATCH_PTR(subdis);
        cObject *parent = getParentModule();
        WATCH_PTR(parent);

        for ( ; ; )
            wait(10);
    }
};

Define_Module(Sub);