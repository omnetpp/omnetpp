//--subclassing--
// note: the above line is necessary for nedcwrapper.sh to work!

//
// An example to the experimental message subclassing support. This file has
// to be processed by msgc.pl which generates C++ classes you can use from
// your models as well as "reflection" classes that allow Tkenv to inspect
// these data stuctures.
//
// Check msgdoc.txt for some more information.
//
// The syntax and features shown in this file are all experimental.
// Feedback is welcome!
//
// msgc.pl is an intermediate solution until the new extended nedc
// becomes available.
//


// necessary #includes for the
cppinclude <stdio.h>
cppinclude "mystuff.h"

// announce C++ types to nedc
struct IPAddress;
cobject BasicPacket;
cobject PayloadPacket;
noncobject mystringclass;

// create an enum
enum ProtocolTypes
{
    IP = 1;
    TCP = 2;
};

// add new values to ProtocolTypes
enum MoreProtocolTypes updates ProtocolTypes
{
    CLNP = 3;
    TP4 = 4;
};

// message object
message FooPacket extends BasicPacket
{
    fields:
        char type='x';
        IPAddress *x;
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
        virtual int myvar;
};

// class without base class (NOT derived from cObject!)
class MyClass
{
    properties:
        customize = true;
    fields:
        virtual char type='x';
        virtual short version=2;
}

// class not derived from cObject
class MyDerivedClass extends MyClass
{
    fields:
        char bla='x';
        short blabla=2;
}

// class derived from cObject
class MyCObjectClass extends cObject
{
    fields:
        char type='x';
        short version=2;
        mystringclass dunno;
}

// struct with public members, without getter/setter methods (also without vtable!)
struct MyStruct
{
    fields:
        char type='x';
        short version=2;
}

// inheritance with structs
struct MyDerivedStruct extends MyStruct
{
    fields:
        char bla='x';
        short blabla=2;
}


