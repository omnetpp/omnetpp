//
// An example to the experimental message subclassing support. This file has
// to be processed by opp_msgc which generates C++ classes you can use from
// your models, as well as "reflection" classes that allow Tkenv to inspect
// these data stuctures.
//
// Check doc/msgsubclassing.txt for more information.
//
// The syntax and features shown in this file are all experimental.
// Feedback is welcome!
//


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
  bla bla bla
};

// message object
message FooPacket extends BasicPacket
{
    fields:
        char type='x';
        // not supported: IPAddress *x;
        short version=2;
        int protocolId enum(ProtocolTypes);
        bool hasPayload;
        long length;
        long route[4];
        string host[];
        IPAddress src;
        IPAddress dest;
        PayloadPacket payload[];
};

// demonstrate subclassing
message BarPacket extends FooPacket
{
    properties:
        customize = true;
    fields:
        int hopcount;
        double delay;
        abstract int myvar;
};

// class without base class (NOT derived from cObject!)
class MyClass
{
    properties:
        customize = true;
    fields:
        abstract char type='x';
        abstract short version=2;
};

// class not derived from cObject
class MyDerivedClass extends MyClass
{
    fields:
        char bla='x';
        short blabla=2;
};

// class derived from cObject
class MyCObjectClass extends cObject
{
    fields:
        char type='x';
        short version=2;
        mystring vendor;
};

// plain C struct (public members, no getter/setter methods, etc)
struct MyStruct
{
    fields:
        char array[10];
        short version;
};


// inheritance with structs
struct MyDerivedStruct extends MyStruct
{
    fields:
        char code;
        short prot enum(ProtocolTypes); 
        MyStruct substruct;  // also as embedded data member
};


