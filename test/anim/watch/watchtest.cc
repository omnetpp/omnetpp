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

// no op<<
struct Unprintable
{
    Unprintable(int a1) : a(a1) {}
    int a;
};

std::ostream& operator<<(std::ostream& os, const Point& p)
{
    return os << "(" << p.x << "," << p.y << ")";
}

bool operator<(const Point& lhs, const Point& rhs)
{
    if (lhs.x != rhs.x) return lhs.x < rhs.x;
    return lhs.y < rhs.y;
}

std::ostream& operator<<(std::ostream& os, const GeneratedStruct& gs)
{
    return os << "GeneratedStruct (" << gs.foo << "," << gs.bar << "," << gs.baz << ")";
}

bool operator<(const GeneratedStruct& lhs, const GeneratedStruct& rhs)
{
    if (lhs.foo != rhs.foo) return lhs.foo < rhs.foo;
    if (lhs.bar != rhs.bar) return lhs.bar < rhs.bar;
    return lhs.baz < rhs.baz;
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
        out << "Poly: n=" << n << ", edgeLen=" << edgeLen << " (printed by str())";
        return out.str();
    }
};

std::ostream& operator<<(std::ostream& os, const APolygon& p)
{
    return os << "Poly: n=" << p.n << ", edgeLen=" << p.edgeLen << " (printed by op<<)";
}

class AStar : public APolygon
{
    public:
    int pointy;
    AStar(int nsides, int edgeLength, int pointiness) : APolygon(nsides, edgeLength) {pointy = pointiness;}
    std::string str() const override {
        std::stringstream out;
        out << "Star: n=" << n << ", edgeLen=" << edgeLen << ", pointy=" << pointy << " (printed by str())";
        return out.str();
    }
};

std::ostream& operator<<(std::ostream& os, const AStar& s) {
    return os << "Star: n=" << s.n << ", edgeLen=" << s.edgeLen << ", pointy=" << s.pointy << " (printed by op<<)";
}

std::string printGeneratedStruct(const GeneratedStruct& gs) {
    return "printed GeneratedStruct (" + std::to_string(gs.foo) + "," + std::to_string(gs.bar) + "," + gs.baz.c_str() + ")";
}

// no descriptor, no way to get base struct descriptor
struct GeneratedStructSub : public GeneratedStruct { };

// no descriptor, should use base class descriptor via cObject::getDescriptor
class GeneratedPacketSub : public GeneratedPacket {
public:
    using GeneratedPacket::GeneratedPacket;
};

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
    WATCH(point_rw);

    Unprintable up{42};
    WATCH(up); // displayed as "(unprintable)", see also the Printability module

    //
    // Structs/classes via cObject and str(); no structdesc.
    //
    APolygon poly_WATCH(5, 100);
    WATCH(poly_WATCH);

    APolygon poly_WATCH_OBJ(5, 100);
    WATCH(poly_WATCH_OBJ);

    //
    // Generated structs/classes (with structdesc.)
    //

    GeneratedStruct gs;
    WATCH(gs);

    GeneratedClass gc;
    GeneratedMessage gm("gm-obj");
    GeneratedPacket gp("gp-obj");
    WATCH(gc);
    WATCH(gm);
    WATCH(gp);

    GeneratedClass *gcp = new GeneratedClass;
    cMessage *gmp = new GeneratedMessage("gmp-obj");
    cPacket *gpp = new GeneratedPacket("gpp-obj");
    WATCH(gcp);
    WATCH(gmp);
    WATCH(gpp);

    const GeneratedClass *cgcp = new GeneratedClass;
    const cMessage *cgmp = new GeneratedMessage("cgmp-obj");
    const cPacket *cgpp = new GeneratedPacket("cgpp-obj");
    WATCH(cgcp);
    WATCH(cgmp);
    WATCH(cgpp);

    cObject *nada = nullptr;
    WATCH(nada);

    cObject *this_ = this;
    WATCH(this_);

    // pointer is captured by reference, and descriptor is not supposed to be cached
    cObject *changing = nullptr;
    WATCH(changing);

    // int *wrongp = (int *)gcp;
    // WATCH(wrongp); -- this has to give a compile error

    //
    // Vectors, lists, sets, and maps
    //
    std::vector<int> vi;
    vi.push_back(2);
    vi.push_back(3);
    vi.push_back(5);
    vi.push_back(7);
    WATCH(vi);

    std::list<std::string> ls;
    ls.push_back("two");
    ls.push_back("three");
    ls.push_back("five");
    ls.push_back("seven");
    WATCH(ls);

    std::map<int, std::string> m;
    m[1] = "one";
    m[2] = "two";
    m[3] = "three";
    WATCH(m);

    //
    // Nested containers
    //
    std::vector<std::vector<int>> vv;
    vv.push_back(std::vector<int>{2,3});
    vv.push_back(std::vector<int>{5,7});
    vv.push_back(std::vector<int>{11,13});
    WATCH(vv);

    std::vector<std::map<int, double>> vm;
    vm.push_back(std::map<int, double>{{1, 1.1}, {2, 2.2}});
    vm.push_back(std::map<int, double>{{3, 3.3}, {4, 4.4}});
    vm.push_back(std::map<int, double>{{5, 5.5}, {6, 6.6}});
    WATCH(vm);

    std::map<int, std::vector<std::string>> mv;
    mv[1] = {"one", "uno", "un"};
    mv[2] = {"two", "dos", "deux"};
    mv[3] = {"three", "tres", "trois"};
    WATCH(mv);

    std::map<int, std::map<std::string, int>> mm;
    mm[1] = {{"one", 1}, {"uno", 1}, {"un", 1}};
    mm[2] = {{"two", 2}, {"dos", 2}, {"deux", 2}};
    mm[3] = {{"three", 3}, {"tres", 3}, {"trois", 3}};
    WATCH(mm);

    std::set<std::string> ss;
    ss.insert("Dopey");
    ss.insert("Sleepy");
    ss.insert("Grumpy");
    ss.insert("Sneezy");
    ss.insert("Happy");
    ss.insert("Bashful");
    ss.insert("Doc");
    WATCH(ss);

    //
    // Containers with compound elements
    //
    GeneratedPacket *pk1 = new GeneratedPacket("packet1");
    GeneratedPacket *pk2 = new GeneratedPacket("packet2");
    GeneratedPacket *pk3 = new GeneratedPacket("packet3");

    std::vector<GeneratedStruct> vgs;
    GeneratedStruct gs1;
    gs1.foo = 1;
    gs1.bar = 2;
    gs1.baz = "one";
    vgs.push_back(gs1);
    GeneratedStruct gs2;
    gs2.foo = 11;
    gs2.bar = 22;
    gs2.baz = "eleven";
    vgs.push_back(gs2);
    GeneratedStruct gs3;
    gs3.foo = 33;
    gs3.bar = 44;
    gs3.baz = "thirtythree";
    vgs.push_back(gs3);
    WATCH(vgs);

    GeneratedStructSub gss;
    gss.foo = 55;
    gss.bar = 66;
    gss.baz = "fiftyfive";
    std::vector<GeneratedStruct *> vgsp;
    vgsp.push_back(&gs1);
    vgsp.push_back(&gs2);
    vgsp.push_back(&gs3);
    vgsp.push_back(&gss);
    WATCH(vgsp);

    // no way to get desc for this, but op<< of GeneratedStruct should work
    std::vector<GeneratedStructSub *> vgssp;
    vgssp.push_back(&gss);
    WATCH(vgssp);

    GeneratedPacketSub *pks = new GeneratedPacketSub("packetSub");
    std::vector<GeneratedPacket *> vpacket;
    vpacket.push_back(pk1);
    vpacket.push_back(pk2);
    vpacket.push_back(pk3);
    vpacket.push_back(pks); // should be described by desc of GeneratedPacket
    WATCH(vpacket);

    // this should still use desc of GeneratedPacket
    // NOTE: see the comment in cStlContainerWatcherBase::getDescriptor() for why this doesn't work
    std::vector<GeneratedPacketSub *> vpkssp;
    vpkssp.push_back(pks);
    WATCH(vpkssp);

    std::list<GeneratedStruct> lgs;
    lgs.push_back(gs1);
    lgs.push_back(gs2);
    lgs.push_back(gs3);
    WATCH(lgs);

    std::list<GeneratedStruct *> lgsp;
    lgsp.push_back(&gs1);
    lgsp.push_back(&gs2);
    lgsp.push_back(&gs3);
    WATCH(lgsp);

    std::list<GeneratedPacket *> lpacket;
    lpacket.push_back(pk1);
    lpacket.push_back(pk2);
    lpacket.push_back(pk3);
    WATCH(lpacket);

    std::set<GeneratedStruct> sgs;
    sgs.insert(gs1);
    sgs.insert(gs2);
    sgs.insert(gs3);
    WATCH(sgs);

    std::set<GeneratedStruct *> sgsp;
    sgsp.insert(&gs1);
    sgsp.insert(&gs2);
    sgsp.insert(&gs3);
    WATCH(sgsp);

    std::set<const GeneratedStruct *> csgsp;
    csgsp.insert(&gs1);
    csgsp.insert(&gs2);
    csgsp.insert(&gs3);
    WATCH(csgsp);

    std::set<GeneratedPacket *> spacket;
    spacket.insert(pk1);
    spacket.insert(pk2);
    spacket.insert(pk3);
    WATCH(spacket);

    std::set<const GeneratedPacket *> cspacket;
    cspacket.insert(pk1);
    cspacket.insert(pk2);
    cspacket.insert(pk3);
    WATCH(cspacket);

    std::map<int, GeneratedStruct> mgs;
    mgs[1] = gs1;
    mgs[2] = gs2;
    mgs[3] = gs3;
    WATCH(mgs);

    std::map<int, GeneratedStruct *> mgsp;
    mgsp[1] = &gs1;
    mgsp[2] = &gs2;
    mgsp[3] = &gs3;
    WATCH(mgsp);

    std::map<int, const GeneratedStruct *> cmgsp;
    cmgsp[1] = &gs1;
    cmgsp[2] = &gs2;
    cmgsp[3] = &gs3;
    WATCH(cmgsp);

    std::map<int, GeneratedPacket *> mpacket;
    mpacket[1] = pk1;
    mpacket[2] = pk2;
    mpacket[3] = pk3;
    WATCH(mpacket);

    std::map<int, const GeneratedPacket *> cmpacket;
    cmpacket[1] = pk1;
    cmpacket[2] = pk2;
    cmpacket[3] = pk3;
    WATCH(cmpacket);

    std::map<int, Unprintable *> mup;
    mup[1] = new Unprintable(11);
    mup[2] = nullptr;
    mup[3] = new Unprintable(33);
    WATCH(mup);

    std::map<Unprintable *, std::string> mupkey;
    mupkey[new Unprintable(100)] = "one hundred";
    mupkey[nullptr] = "null";
    mupkey[new Unprintable(200)] = "two hundred";
    WATCH(mupkey);

    std::map<int *, std::string> mnullptrkey;
    mnullptrkey[nullptr] = "null";
    mnullptrkey[new int(42)] = "forty-two";
    WATCH(mnullptrkey);

    std::map<Point, std::string> mpointkey;
    mpointkey[Point(1, 2)] = "one-two";
    mpointkey[Point(3, 4)] = "three-four";
    mpointkey[Point(0, 0)] = "origin";
    WATCH(mpointkey);

    std::map<GeneratedStruct, std::string> mgskey;
    mgskey[gs1] = "first";
    mgskey[gs2] = "second";
    mgskey[gs3] = "third";
    WATCH(mgskey);

    std::vector<Unprintable *> vup;
    vup.push_back(new Unprintable(1));
    vup.push_back(new Unprintable(2));
    vup.push_back(nullptr);
    WATCH(vup);

    std::vector<APolygon *> vpoly;
    vpoly.push_back(new AStar(3, 100, 20)); // this will still use op<< of APolygon
    vpoly.push_back(new APolygon(4, 100));
    vpoly.push_back(nullptr);
    WATCH(vpoly);

    std::vector<const APolygon *> cvpoly;
    cvpoly.push_back(new AStar(3, 100, 20)); // this will still use op<< of APolygon
    cvpoly.push_back(new APolygon(4, 100));
    cvpoly.push_back(nullptr);
    WATCH(cvpoly);

    std::vector<AStar *> vstar;
    vstar.push_back(new AStar(3, 100, 20));
    vstar.push_back(new AStar(5, 150, 30));
    vstar.push_back(nullptr);
    WATCH(vstar);

    std::vector<const AStar *> cvstar;
    cvstar.push_back(new AStar(3, 100, 20));
    cvstar.push_back(new AStar(5, 150, 30));
    cvstar.push_back(nullptr);
    WATCH(cvstar);

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
        WATCH(subdis);
        cObject *parent = getParentModule();
        WATCH(parent);

        for ( ; ; )
            wait(10);
    }
};

Define_Module(Sub);


//
// Demonstrates the string displayed for values that have no @toString or str():
// the output of operator<< if there is one; otherwise "" for compound values
// (that have fields, which can be displayed by expanding the value), and
// "(unprintable)" for others.
//
class Printability : public cSimpleModule
{
  public:
    Printability() : cSimpleModule(16384) {}
    virtual void activity() override;
};

Define_Module(Printability);

void Printability::activity()
{
    // via @toString in the msg file
    GeneratedStruct withToString;
    withToString.foo = 1;
    WATCH(withToString);

    // via operator<<: a C++ struct and a msg struct
    Point point(2, 3);
    PrintableStruct printable;
    printable.a = 4;
    WATCH(point);
    WATCH(printable);

    // compound (has fields), no operator<<: "" (expand to see the fields)
    PlainStruct plain;
    plain.b = 5;
    WATCH(plain);

    // not compound (no fields known), no operator<<: "(unprintable)"
    Unprintable unprintable(6);
    WATCH(unprintable);

    // the same values as container elements, pair members and smart pointer pointees
    std::vector<PrintableStruct> vprintable(2, printable);
    std::vector<PlainStruct> vplain(2, plain);
    std::vector<Unprintable> vunprintable(2, unprintable);
    std::pair<PlainStruct, Unprintable> pplainunprintable(plain, unprintable);
    std::unique_ptr<PlainStruct> uplain = std::make_unique<PlainStruct>(plain);
    std::unique_ptr<Unprintable> uunprintable = std::make_unique<Unprintable>(unprintable);
    WATCH(vprintable);
    WATCH(vplain);
    WATCH(vunprintable);
    WATCH(pplainunprintable);
    WATCH(uplain);
    WATCH(uunprintable);

    // the same as message fields: open the inspector of the message
    PrintabilityMessage *msg = new PrintabilityMessage("printability-msg");
    msg->setWithToString(withToString);
    msg->setPrintable(printable);
    msg->setPlain(plain);
    msg->setPrintablePtr(&printable);
    msg->setPlainPtr(&plain);
    WATCH(msg);

    for ( ; ; )
        wait(10);
}
