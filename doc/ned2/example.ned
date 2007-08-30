//
// File comment (copyright statement, whatever...)
//

// NEW!! withcppclass not needed: to be replaced with @class()

//
// NAME SCOPES:
// - all component type names must be unique
// - identifier names within a component must be unique (ie submodule
//   or inner type cannot be named the same, or the same as a gate or
//   parameter of the parent module)
//     THIS MUST BE ENFORCED DURING VALIDATION!!!
//
// - sizeof() resolution:
//     IN SUBMODULE SCOPE:
//         sizeof(foo): must refer to a parent module gate vector, or
//                      sibling submodule vector (there cannot be ambiguity
//                      here, because of the name uniqueness rule above)
//                      NOTE: IT CANNOT REFER TO LOCAL GATE VECTOR !!!
//         sizeof(this.foo): refers to local gate vector
//         sizeof(foo.bar): refers to gatevector of sibling submodule
//     IN CONNECTION SCOPE:
//         same as above, only sizeof(this.foo) does not make sense
//     IN [PARENT] MODULE SCOPE:
//         sizeof(foo), sizeof(this.foo): refers to that module's gatevector
//         sizeof(foo.bar): makes no sense (thus illegal)
//
// - "index" operator: it always returns the same (sub)module's index in whose "parameters"
//    block it is defined. It is NOT POSSIBLE to obtain the parent module's index.
//
// TODO:
// - CHANNELS: two implicit (built-in) channel types exist: NullChannel, BasicChannel.
//   Their names can be referenced, ie one can write "channel X extends NullChannel..."
//         channel withcppclass BasicChannel {
//             double delay = 0;  // make it "volatile double" ???
//             double error = 0;
//             double datarate = 0;
//         };
//         channel withcppclass NullChannel {
//         };
// - Channel inheritance:  ultimate base class is always a "channel withcppclass";
//   "channel withcppclass X extends Y" is ILLEGAL! (ie "withcppclass" can only be
//   an ultimate base class).
//
// - alternative name for "channel withcppclass": "channelbase X {...}" ???
//
// - The default base class is BasicChannel. That is:
//     1. "--> {...} -->" means "--> BasicChannel {...} -->"
//     2. "channel X {...}" means "channel X extends BasicChannel {...}"
//
//    It was considered to force the user spell out "extends BasicChannel", and discarded (inconvenient)
//    It was considered to make NullChannel the default, and discarded (impractical and error-prone)
//
// - This was considered and discarded: "module X extends ASimple" instead of "simple X extends ASimple"
//   Was proposed because otherwise, if base type's implementation gets changed from
//   simple to compound, all subclasses will have to be changed.
//   Idea discarded because the confusion this would create is worse than the problem itself.
//
// - Note: "simple X extends ASimpleModule": inherits the C++ implementation of ASimpleModule!
//   (That is, code CANNOT be modified via "extends" -- it always stays the same,
//   that of the ultimate base class.) Subclass may add new parameters and gates
//   (if it makes sense...) and set parameters and gate vector sizes.
//
// - WHAT IF: "simple" would mean "underlying C++ class expected", and
//   "module" would mean "no underlying C++ class" ?
//   That is, both "module X extends ASimpleModule" and "simple X extends ASimpleModule"
//   would be legal and would mean DIFFERENT things.
//   ("simple X extends ACompoundModule" would still be illegal)
//
// - "module X extends ACompoundModule": subclass may add further submodules,
//   gates, connections. Parameters can be added or assigned a value. It may not
//   remove or modify inherited submodules (there's no syntax for that!)
//
//     XXX TODO: revise all "extends" examples below!
//
// - Extra params CANNOT be added in submodules and connection channelspecs --
//   they can only be added in new types...
//
// - add constants, modelled after Java's public static final stuff
//     module Foo {
//       parameters:
//         constant string RADIO_FREQ=1.4e9; //Hz
//         constant string LOOPBACK_IP_ADDRESS="127.0.0.1";
//     }
//   and usage: Foo.RADIO_FREQ.
//
// - XXX rename "gate" to "port" everywhere? that is the industry standard terminology...
//   OTOH port is already too overloaded in the INET Framework: TCP port (number),
//   router port/switch port -- so maybe it is better to leave it as it is
//
// - XXX find a better name for "channel withclass"!!!
//
// - XXX do we need to support the "like *" phrase?
//
// - XXX IBidirectionalChannel, IUnidirectionalChannel: if a module
//   is "like" them, can it also be "like" channel interfaces...?
//
// - rules of inheritance:
//     o for inner types:
//        - can I define an inner type with the same name in subclasses? NO
//     o for properties:
//        - contents will be merged (rules like for display strings: values on
//          same key and same position will overwrite old ones)
//     o for parameters:
//        - type cannot be redefined
//        - value may be redefined in subclasses or place of usage
//     o for gates:
//        - type cannot be redefined
//        - vector size may be redefined in subclasses or place of usage
//     o for gate/parameter properties:
//        - extra properties can be added
//        - existing properties can be overridden/extended like for standalone properties (?)
//     o for submodules:
//        - new submodules may be added, but inherited ones cannot be modified
//     o for connections:
//        - new connections may be added, but inherited ones cannot be modified
//
// - rules of conditionals must be defined exactly!
//     o for inner types: NO CONDITIONAL
//     o for properties: YES
//     o for parameters: only for assignment/property change, but not for declaration
//     o for gates: only for gatesize/property change, but not for declaration
//     o for submodules: NO
//     o for connections: YES
//
// - XXX is it possible to set a parameter to its default value? e.g. queueCapacity = default;
//
// - submodule array: was
//         eth: EtherMAC[n];
//   now:
//         eth[n]: EtherMAC;
//   Restore old syntax? But it would mean this too:
//         eth: <macType>[n] like EtherMAC;
//
//
// DO WE NEED "like *" ? Not much value in there...
//
// - CONDITIONAL PARAMETERS, PROPERTIES (@display) AND GATES (GATESIZES) ARE
//   NOT SUPPORTED. Reasons:
//    - inefficient at runtime. At network building, one cannot aggregate
//      parameters on a "prototype module" and then copy it to instances,
//      but on every instance the whole *procedure* would have to be repeated
//      (turns declarative impl into imperative...)
//    - as param values, gate size, param/gate/module/submodule properties may
//      involve conditionals, they are UNDETERMINABLE with static analysis.
//      e.g. one cannot tell with static analysis whether a parameter has
//      been assigned or not.
//    - strange things like multiple assigments would have to be allow too:
//         submod: Foo {
//            parameters:
//               int a;
//               a = 3 if index==1;
//               a = 5;
//            }
//    - INCONSISTENCY: with wildcards, FIRST match counts; with conditional
//      assignments, LAST match counts (it's impossible to do the other way!!!)
//    - for params and gatesizes, it can be easily rewritten to use "?:" operator:
//       example:
//            par = (5 if index<3, 0 otherwise);
//        generalized (v1 if c1, v2 if c2, v3 if c3, otherwise vdefault);
//        same as (c1 ? v1 : c2 ? v2 : c3 ? v3 : vdefault)
//
// - parameter once assigned in base class or type MUST be allowed to be
//   set to a different value in subclass/place of usage!!!
//
// RULE: moduleinterface/channelinterface: allow assignments in its parameters/
//   allow gatesizes? if parameter is volatile, it will have to be compared as
//   expression!!
//
// - VALIDATION:
//
//

package generic.queueing;  // default package for following components

import "foo.ned";

@someproperty(somekey;otherkey); // file-level property

//
// Simple FIFO module with a msg/sec processing rate and a finite buffer
// capacity. Messages which arrive when the queue is full are dropped.
//
simple Queue
{
    parameters: // this keyword is optional
        @display(i="queue");   // default presentation
        double msgPerSec = default(100); // maybe overridden by another default()
        int capacity = 100;  // fixed value, cannot be overridden
        int processingTime = exponential(10); // const value returns always the same number
        int novalue;  // no value assigned here
        volatile int waitTime = exponential(20); // evaluates exp(20) each
                                                 // time the parameter accessed
    gates:
        input in[];  // input gates
        input in2[5];  // input gates with fixed size
        output out;  // output gate
        inout thirdGate;
}

simple VeryFastQueue1 extends Queue   // uses "Queue" C++ class
{
}

simple VeryFastQueue2 extends Queue  // uses "Queue" C++ class; inherits params/gates
{
    parameters: // this keyword is optional
        msgPerSec = 1000000;  // no type -> sets base class' parameter
        bool isVeryFast = true;  // add new parameter
        novalue @addedproperty; // attach a property to novalue parameter without assigning a value to it
}

simple QueueWithFourInputs extends Queue
{
    gates:
        in[4];  // sets vector gate size to a fixed value
        input plusOneInput;  // adds plus one gate
}

simple QueueWithNInputs extends Queue
{
    parameters:
        int numInputs @prompt("Number of input ports"); // add new parameter
    gates:
        in[numInputs];  // sets vector gate size to a fixed value
}

// property syntax:
//   @propertyname(key="value1","value2"; key="value"; key=5; key=false; key=true;
//                 "value", value, this value, 5, true, false, 10s 531ms)
//   @propertyname;             // propertyname.default="true"
//   @propertyname(false);      // propertyname.default="false"
//   @propertyname(Hello);      // propertyname.default="Hello"
//   @propertyname(Hello,World);// propertyname.default={"Hello","World"}
//   @propertyname("egy","ketto",3,more=true);  // propertyname.default={"egy","ketto","3","..."}, ~.more="true"
//   @propertyname(10s 531ms);  // propertyname.default="10s 531ms"
//   @propertyname(starttime=10s 531ms,true);  // propertyname.starttime={"10s 531ms",true}
//
// value in itself is interpreted as belonging to key "default"; and one can override
// it with the key=... syntax.
//
// property representation:
//   list of key-valuelist pairs; either key or valuelist can be empty
// actual storage may be:
//   hashtable based on key; content is a list (vector) of values
//
// One property name may occur more than once; runtime effect is that the keys/values will be merged.
// Rules of property merging:
//  - a value at a key/position OVERWRITES earlier value at same key/position.
//  - the '-' value removes value from that position; one van write literal minus as "-"
//    example:
//       @display(i="device/pc",red,10);
//       @display(i=,,77);   ====> results in "i=device/pc,red,77"
//       @display(i=,-);     ====> removes "red", so results is "i=device/pc,,77"
//
//
simple TCP1
{
    parameters:
        int mss; // maximum segmentsize
        int advertisedWindow; // in bytes (Note: normally, NIC queues should be at least this size)
        string tcpAlgorithmClass1 @enum("TCPTahoe", "TCPReno", "TCPNoCongestionControl", "DumbTCP"); // "..."
        string tcpAlgorithmClass2 @classname("ITCPAlgorithm"); // alternative syntax:
                                                             // specify it accepts a class name, and specify base class
        @kernel(starttime=49);
        @recordstats[stat1](storeintervall=5; averaging=true); // recording seqNum etc. into output vectors on/off, 'id' is the primary key (must be unique)
        @recordstats[stat2](storeintervall=9; averaging=false); // recording seqNum etc. into output vectors on/off
        @display(i="block/transport");
    gates:
        inout toApp[] @labels(TCPControlInfo);  // inout; label means "sends/expects msg with TCPControlInfo"
        inout toNetw @labels(TCPSegment);       // inout; label means "sends/expects TCPSegment"
}


module MobileHost1
{
    parameters:
        double x;
        double y;
        @display(p=$x,$y); // $paramname is only resolved at runtime
}


//
//    // Alternative: gate types as identifiers not strings.
//    // Undefined or non-matching gatetypes may only cause warning but not error!
//
//    // Messages with attached TCPControlInfo sent/expected
//    gatetype withTCPControlInfo;
//
//    // Messages of type TCPSegment sent/expected
//    gatetype TCPSegment;
//
//    // Gate between Ethernet MAC and Ethernet Bus
//    gatetype EthernetBus;
//

simple TCP2
{
    gates:
        // receiver's labels must be superset of sender's labels (must be able to receive anything sent to it)
        // inout gates: labels must be exactly the same (superset relation must hold in both directions)
        inout toApp[] @inlabels(tcp2app) @outlabels(app2tcp) @mustbeconnected;
        inout toNetw @inlabels(tcp2net) @outlabels(net2tcp);
}

simple Queue2
{
    gates:
        input in[]; // means @labels("*");
        output out; // means @labels("*");
}


//
// modules that can be connected to TCP
//
moduleinterface ITCPApp   // no C++ impl; cannot be instantiated
{
    gates:
        inout toTCP @inlabels(tcp2app) @outlabels(app2tcp);
}

//
// modules that can be connected to UDP
//
moduleinterface IUDPApp
{
    gates:
        inout toUDP @inlabels(udp2app) @outlabels(app2udp);
}

simple GenericFTPApp
{
    parameters:
        double connectInterval;
        volatile int fileLength;
    gates:
        inout toTCP @inlabels(tcp2app) @outlabels(app2tcp);
}


simple FTPApp extends GenericFTPApp like ITCPApp   // or: like TCPApp
{
    parameters:
        connectInterval = 10s;
        fileLength = normal(10Mb, 1Mb);
    // gates & params come from the "extends" base classes;
    // then they have to match the "like" interfaces.
}

module FlexibleApp like ITCPApp, IUDPApp // may be connected to *either* TCP or UDP
{
    parameters:
        double connectInterval = 10s;
        int fileLength = normal(10Mb, 1Mb);
    gates:
        inout toTCP @inlabels(tcp2app) @outlabels(app2tcp);
        inout toUDP @inlabels(udp2app) @outlabels(app2udp);
}


moduleinterface ITCPUDPApp extends ITCPApp, IUDPApp   // both TCP and UDP gates required!!! (e.g. OSPF)
{
   // if there're conflicting gates/params between ITCPApp & IUDPApp ==> error
}

module OSPFRouting like ITCPUDPApp // connects to *both* TCP and UDP
{
}


property @prop1;  // properties must be declared (guard against typos); values for the default key are accepted
property @prop2[](foo;bar);  // valid key names MUST be declared (only declared keys can be used)

//property examples
@prop2[one];
@prop2[two]();
@prop2[three](,,,foo);
@prop2[four](,,,foo,,);
@prop2[five](foo=1,2,-,4;bar=40,,0);


module Host0
{
    parameters:
        @host;  // custom properties can serve as "marker interfaces"
    gates:
        inout pppPort[] @labels(PPP);
        inout ethPort[] @labels(Ethernet);  // type label implies "EthernetFrame" and UTP
        inout ethBusPort[] @labels(EthernetBus);  // type label implies "EthernetFrame" and coaxial cable
    submodules:
        app: <> like TCPApp {};
        tcp: TCP1 {
            mss = 1024;
        };
        ip: IP {};
        ppp[sizeof(pppPort)]: PPP {};

    connections allowunconnected:
        app[i].toTcp <--> { @display(c="red"); delay=0; } <--> tcp.toApp;

        // connect both directions separately (impl: 1 bidir obj, plus 2 child objects on-demand)
        app[i].toTcp$o --> { @display(c="red"); delay=0; } --> tcp.toApp$i;
        app[i].toTcp$i <-- { @display(c="red"); delay=0; } <-- tcp.toApp$o;

        // the right syntax with $i/$o:
        node1.port$o++ --> node2.port$i++;  //XXX and not node1.port++$o
        node1.port$o[i] --> node2.port$i[i];

        tcp.toNetw <--> ip.toTcp;

        // IP to network interfaces
        ppp[i].toNetw <--> ip.toIf[i] for i=0..sizeof(ppp)-1;
        // or:  ..... <--> ip.toIf++ where...

        // network interfaces to parent
        ppp[i].phy <--> pppPort[i] for i=0..sizeof(ppp)-1;
}

//
// parameter scopes
//
simple UDP1
{
    int maxpk;
}

module Host1
{
    parameters:
        double delay;
        int mss0 = 1000;
        int mss = 500;
        int bla = -1;
    types:
        simple TCP1X extends TCP1
        {
            int tmp;
        }
    submodules:
        udp: UDP1 {
            maxpk = 2000;
        };
        tcp: TCP1X {
            // reserved keywords: "this"
            // scope on right side: "parent"; one can explicitly write "this."
            // left side scope is always "this."
            tmp = 200;
            mss = mss0; // --> 1000 (parent's)
            mss = this.tmp; // --> 200
            mss = mss;  // --> 500 (parent's)
            mss = udp.maxpk;
        };
        ip: IP {};
    connections:
        tcp.out --> {delay = delay;} --> ip.in;
}

//property @syntax error $#%$#%

channel Eth
{
    parameters:
        @display(c="red");
        @labels(PPP, Eth);
        double delay1 = 10us;  // TODO distance-based delay spec?
        double delay2 = dist2delay(15m, 200000);
        double delay3 = dist2delay(15m); // default 200,000 km/s
        // channel parameters are implemented exactly like module parameters
}


network Network1   // "network" is exactly as "module", but: not gates
{
    parameters:
        int a;
    types: // defines locally used channel and module types  FIXME allow types before gates/params as well?
        channel PPPLink
        {
            parameters:
                @display(c="red");
                double delay = 10us;  // TODO distance-based delay spec?
        }
        module HostX extends Host1
        {
            double linkspeed;
        }
        channel SomeLink extends BasicChannel
        {
        }
    submodules:
        host[100]: HostX {};
    connections:
        host[0].pppPort++ <--> PPPLink <--> host[1].pppPort++;
        host[0].pppPort++ <--> PPPLink {@display(c="green");} <--> host[3].pppPort++;
        host[0].pppPort[host[0].destgate] <--> SomeLink {datarate = host[0].linkspeed;} <--> host[3].pppPort++;
}


simple Node
{
    gates:
        inout port;
}

network RandomGraph
{
    submodules:
        node[100]: Node {};
    connections:
        // brief syntax:
        node[i].port++ <--> node[j].port++ for i=0..98, for j=i+1..99, if uniform()<0.3;

        // using a connection group
        for i=0..98, for j=i+1..99, if uniform()<0.3 {
            node[i].port++ <--> node[j].port++;
        };

        for i=0..98, for j=i+1..99 {
            node[i].port++ <--> node[j].port++ if uniform()<0.3;
            node[i].port++ <--> node[j].port++ if uniform()<0.1;
        };


        for i=0..98, for j=i+1 .. 99, if uniform()<0.3 {
            node[i].port++ <--> node[j].port++;
            node[i].port++ <--> node[j].port++;
        };

        for i=0..98, for j=i+1..99 {
            node[i].port++ <--> node[j].port++ if uniform()<0.3;

            // dsfdsfdsfsd
            node[i].port++
               <--> // hhhh
                node[j].port++ if uniform()<0.1; // dfdsfsdfsd
                                                 //fgdffgfg

            // fggfdgf

            node[i].port++ <--> node[j].port++  // dfdsfsdfsd
                                    if uniform()<0.1;     // dfdsfsdfsd

        };

        for i=0..98 {
            node[i].port++ <--> node[j].port++ for j=i+1..99, if uniform()<0.3;
            node[i].port++ <--> node[j].port++ for j=i+1..99, if uniform()<0.1;
        };
}

//
// "like" syntax:
//
module Host2
{
    parameters:
        int N;
        string appType @moduleWithInterface(TCPApp); // "like TCPApp"
    submodules:
        apps[N]: <appType> like TCPApp {}; // they're all of the same type
}

module ConfigurableHost
{
    parameters:
        int N;
    submodules:
        app: <> like TCPApp {};
        apps[N]: <> like * {}; // uses "typename" virtual parameter, type unchecked  FIXME do we need this???
        apps[N]: <> like TCPApp {};  // uses "typename" virtual parameter (idea of @type property was discarded)
        apps[N]: <appType> like TCPApp {};   // all modules are the same type (appType); conceptually, <> operator converts string to type
        //apps[]: <appType[]> like TCPApp {};  // with string arrays; not yet implemented
        tcp: TCP {};
}

network Network2
{
    submodules:
        host: ConfigurableHost {
            N = 5;
            /apps[0].typename/ = "FTPApp";  // "typename" is a keyword
            /apps[1..4].typename/ = "WebBrowserApp";

            // if Host had a tcp submodule:
            /tcp*.mss/ = 1024;
        };
}

moduleinterface IHost
{
}

network Network3
{
    submodules:
        // this is also legal:
        host: <> like IHost {
            typename = "FTPApp";
        };
        // would be the same as:
        host: FTPApp {};
}



channelinterface IPPPChannel
{
}


// "like" syntax for channels
network Network4
{
    parameters:
        string channelType;
    submodules:
        host[100]: Host0 {};
    connections:
        host[0].pppPort++ <--> <channelType> like IPPPChannel <--> host[1].pppPort++;
        host[0].pppPort++ <--> <> like IPPPChannel <--> host[1].pppPort++; // how to specify typename?
        host[0].pppPort++ <--> <channelType> like * <--> host[1].pppPort++;
        host[0].pppPort++ <--> <> like * <--> host[1].pppPort++; // how to specify typename?
}

// channel labels
channel PPPLine
{
    parameters:
        @labels(PPP); // must be a superset of sender gates' labels (must be able to handle at least those pks, and possible more)
        @display(o="blue",3);
}

// channel delivers (gate expects) message on reception of first bit or last bit?
module EtherMAC1
{
    gates:
        inout gate @deliverOnFirstBit;
}
channel PPPLine1
{
    parameters:
        @deliverOnFirstBit;
}

// channel with or without corresponding C++ class
// (@withcppclass property idea was discarded)
channel withcppclass PPPLine2   // FIXME find better syntax...
{
}

// modules as channels
moduleinterface IBidirectionalChannel  // <-- predefined (built-in) type
{
    gates:
        inout a;
        inout b;
}
moduleinterface IUnidirectionalChannel  // <-- predefined (built-in) type
{
    gates:
        input i;
        output o;
}

module PPPLineModule like IBidirectionalChannel // <-- marks PPPLineModule as suitable for use as channel
{
    gates:
        inout a;
        inout b;
}

// example usage of modules as channels
network PPPNet
{
    types:
        simple Node { gates: inout g; }  //FIXME how could this be implemented in C++? what's the class name??

    submodules:
        node[10]: Node {};
    connections:
        node[0].g++ <--> PPPLineModule <--> node[1].g++; // would implicitly create submodules named c1,c2,..., and connect them
        node[0].g++ <--> PPPLineModule <--> node[3].g++; // allowed because PPPLineModule is "like" IBidirectionalChannel
        //...
}


// "volatile" parameters
simple Gen
{
    parameters:
        volatile double interArrivalTime;
}
network Network5
{
    parameters:
        volatile double iatime;
    submodules:
        gen1: Gen {  // there is no submodule group or conditional submodule; nor submodule refinement in subclasses
            interArrivalTime = 50s 200ms;
        };
        gen2: Gen {
            interArrivalTime = normal(1,0.3) * const( exponential(30) );
        };
        gen3: Gen {
            interArrivalTime = exponential(30);
        };
        gen4: Gen {
            interArrivalTime = iatime;  // by ref
            interArrivalTime = const(iatime); // by value (evaluated once)
        };
}

// units
simple Gen1
{
    parameters:
        double time = 10d 4h 34mi 23s 123ms 675us 999ns; // minutes is "mi" not "m"!!!! change strToSimtime()!!!!
        double datarate = 10Tbps 10Gbps 234Mbps 123Kbps 23bps;
        double filelengthBytes = 10TB 10GB 100MB 654KB 34B;
        double distance = 100km 33m 230mm;

        double time2 @unit(sec);
        double datarate2 @unit(bps);
        double filelengthBytes2 @unit(bytes);
        double distance2 @unit(meter);

        string className = default("FIFO") @enum("FIFO","LIFO");

        double time3 = 10h 30s @unit(sec);
}

