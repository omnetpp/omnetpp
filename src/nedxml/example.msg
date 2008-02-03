//
// An example for message subclassing.
//
// This file has to be processed by opp_msgc which generates C++ classes you
// can use from your models, as well as "reflection" classes that allow Tkenv
// to inspect these data stuctures.
//
// Check doc/msgsubclassing.txt for more information.
//

// put generated classes into a (nested) namespace
namespace exam::ple;

// includes necessary to compile the generated C++ header file
cplusplus {{
#include "basicpacket.h"
#include "payloadpacket.h"
#include "mystring.h"
}};

// announce types that come from included C++ code
struct IPAddress;
class BasicPacket;
class PayloadPacket;
class noncobject mystring;

// imports are not yet supported :-(
// import "othermsgs.msg";

// create an enum
enum ProtocolTypes
{
    IP = 1;
    TCP = 2;
};

// add new values to ProtocolTypes
enum MoreProtocolTypes extends ProtocolTypes
{
    CLNP = 3;
    TP4 = 4;
};

// empty message object
message BlankMsg {
    // garbage
};

// message object
message FooPacket extends BasicPacket
{
    char type='x';
    // not supported: IPAddress *x;
    short version=2;
    int protocolId enum(ProtocolTypes);
    bool hasPayload;
    long length;
    unsigned long route[4];
    string host[];
    IPAddress src;
    IPAddress dest;
    PayloadPacket twoPayloads[2];
    PayloadPacket payload[];
};

// demonstrate subclassing
message BarPacket extends FooPacket
{
    @customize(true);
    int hopcount;
    double delay;
    abstract int myvar;
};

// class without base class (NOT derived from cOwnedObject!)
class SomeClass
{
    int src;
};

class MyClass
{
    @customize(true);
    abstract char type;
    abstract short version;
};

// class not derived from cOwnedObject
class MyDerivedClass extends MyClass
{
    char bla='x';
    short blabla=2;
};

// class derived from cOwnedObject
class MyCObjectClass extends cOwnedObject
{
    char type='x';
    short version=2;
    mystring vendor;
};

// plain C struct (public members, no getter/setter methods, etc)
struct MyStruct
{
    char array[10];
    short version = 5;
};

// inheritance with structs
struct MyDerivedStruct extends MyStruct
{
    unsigned char code;
    short prot enum(ProtocolTypes);
    MyStruct substruct;  // also as embedded data member
};


