
// File comment (copyright statement, whatever...)
//

package generic.queueing;  // default package for following components

@someproperty(somekey); // file-level property

//
// Simple FIFO module with a msg/sec processing rate and a finite buffer
// capacity. Messages which arrive when the queue is full are dropped.
//
simple Queue
{
    parameters: // this keyword is optional
        @display("i=queue");   // default presentation
        double msgPerSec = default(100); // maybe overridden by another default()
        int capacity = 100;  // fixed value, cannot be overridden
        int processingTime = exponential(10); // const value returns always the same number
        int novalue;  // no value assigned here
        int function waitTime = exponential(20); // evaluates exp(20) each
                                                 // time the parameter accessed
    gates:
        input in[];  // input gates
        input in2[5];  // input gates with fixed size
        output out;  // output gate
        inout thirdGate;
};

simple VeryFastQueue extends Queue { }  // "simple": VeryFastQueue C++ class required!!!
module VeryFastQueue extends Queue { }  // "module": no VeryFastQueue C++ class! (Queue C++ class used)

simple VeryFastQueue extends Queue  // "extends" inherits gates as well (they don't have to be repeated here)
{
    parameters: // this keyword is optional
        msgPerSec = 1000000;  // no type -> sets base class' parameter
        bool isVeryFast = true;  // add new parameter
};

simple QueueWithFourInputs extends Queue
{
    gates:
        in[4];  // sets vector gate size to a fixed value
        input plusOneInput;  // adds plus one gate
};

simple QueueWithNInputs extends Queue
{
    parameters:
        int numInputs @prompt("Number of input ports"); // add new parameter
    gates:
        input in[numInputs];  // sets vector gate size to a fixed value
};

// property syntax:
//   @propertyname(key="value", key="value", key=5, key=false, key=true,
//                 "value", value, this value, 5, true, false, 10s 531ms, etc)
//   @propertyname;             // propertyname.default="true"
//   @propertyname(false);      // propertyname.default="false"
//   @propertyname(Hello);      // propertyname.default="Hello"
//   @propertyname(Hello,World);// propertyname.default={"Hello","World"}
//   @propertyname("egy","ketto",3,more=true);  // propertyname.default={"egy","ketto","3","..."}, ~.more="true"
//   @propertyname(10s 531ms);  // propertyname.default="10s 531ms"
//   @propertyname(starttime=10s 531ms);  // propertyname.starttime="10s 531ms"
//
//   value in itself is interpreted as belonging to key "default"
//   for other key, one can only specify with "="
//
// property representation:
//   list of key-value pairs; either key or value can be empty
// actual storage may be:
//   hashtable based on key; content is a list (vector) of values
//
// one property name may occur more than once !!! (properties themselves cannot be
// stored in hashtable! or: store in hastable of property vectors)
// in this case the tag 'id' must be used as a primary key to identidy the proper property eg.
//
simple TCP
{
    parameters:
        int mss; // maximum segmentsize
        int advertisedWindow; // in bytes (Note: normally, NIC queues should be at least this size)
        string tcpAlgorithmClass @choice("TCPTahoe", "TCPReno", "TCPNoCongestionControl", "DumbTCP"); // "..."
        string tcpAlgorithmClass @classname("ITCPAlgorithm"); // alternative syntax:
                                                             // specify it accepts a class name, and specify base class
        @kernel(starttime=49);
        @recordstats(id="stat1", storeintervall=5,averaging=true); // recording seqNum etc. into output vectors on/off, 'id' is the primary key (must be unique)
        @recordstats(id="stat2", storeintervall=9,averaging=false); // recording seqNum etc. into output vectors on/off
        @display("i=block/transport");
    gates:
        inout toApp[] @labels(TCPControlInfo);  // inout; label means "sends/expects msg with TCPControlInfo"
        inout toNetw @labels(TCPSegment);       // inout; label means "sends/expects TCPSegment"
};

simple MobileHost
{
    parameters:
        double x;
        double y;
        @display("p=$x,$y"); // $paramname is only resolved at runtime
};

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

simple TCP {
    gates:
        // receiver's labels must be superset of sender's labels (must be able to receive anything sent to it)
        // inout gates: labels must be exactly the same (superset relation must hold in both directions)
        inout toApp[] @inlabels(tcp2app) @outlabels(app2tcp) @mustbeconnected;
        inout toNetw @inlabels(tcp2net) @outlabels(net2tcp);
};

simple Queue {
    gates:
        input in[]; // means @labels("*");
        output out; // means @labels("*");
};


//
// modules that can be connected to TCP
//
interface TCPApp   // no C++ impl; cannot be instantiated
{
    gates:
        inout toTCP @inlabels(tcp2app) @outlabels(app2tcp);
}

//
// modules that can be connected to UDP
//
interface UDPApp
{
    gates:
        inout toUDP @inlabels(udp2app) @outlabels(app2udp);
}

simple GenericFTPApp
{
    parameters:
        double connectInterval;
        int function fileLength;
    gates:
        inout toTCP @inlabels(tcp2app) @outlabels(app2tcp);
}


module FTPApp extends GenericFTPApp like TCPApp   // or: like TCPApp
{
    parameters:
        connectInterval = 10s;
        fileLength = normal(10Mb, 1Mb);
    // gates & params come from the "extends" base classes;
    // then they have to match the "like" interfaces.
}

module FlexibleApp like TCPApp, UDPApp // may be connected to *either* TCP or UDP
{
    parameters:
        connectInterval = 10s;
        fileLength = normal(10Mb, 1Mb);
    gates:
        inout toTCP @inlabels(tcp2app) @outlabels(app2tcp);
        inout toUDP @inlabels(udp2app) @outlabels(app2udp);
}


interface TCPUDPApp extends TCPApp, UDPApp   // both TCP and UDP gates required!!! (e.g. OSPF)
{
   // if there're conflicting gates/params between TCPApp & UDPApp ==> error
}

module OSPFRouting like TCPUDPApp // connects to *both* TCP and UDP
{
}


property @host;  // properties must be declared (guard against typos); values for the default key are accepted
property @host2 (foo, bar);  // valid key names MUST be declared (only declared keys can be used)

module Host
{
    parameters:
        @host;  // custom properties can serve as "marker interfaces"
    gates:
        inout pppPort[] @labels(PPP);
        inout ethPort[] @labels(Ethernet);  // type label implies "EthernetFrame" and UTP
        inout ethBusPort[] @labels(EthernetBus);  // type label implies "EthernetFrame" and coaxial cable
    submodules:
        apps: <> like TCPApp;
        tcp: TCP {
            mss = 1024;
        };
        ip: IP;
        ppp[sizeof(pppPort)]: PPP;

    connections allowunconnected:
        app[i].toTcp <--> { @display("c=red"); delay=0; } <--> tcp.toApp;

        // connect both directions separately (impl: 1 bidir obj, plus 2 child objects on-demand)
        app[i].toTcp$out --> { @display("c=red"); delay=0; } --> tcp.toApp$in; // or .i, .o ?
        app[i].toTcp$in <-- { @display("c=red"); delay=0; } <-- tcp.toApp$out;

        tcp.toNetw <--> ip.toTcp;

        // IP to network interfaces
        ppp[i].toNetw <--> ip.toIf[i] where i=0..sizeof(ppp)-1;

        ppp[i].toNetw <--> ip.toIf++ where i=0..sizeof(ppp)-1;
        eth[i].toNetw <--> ip.toIf++ where i=0..sizeof(eth)-1;

        // network interfaces to parent
        ppp[i].phy <--> pppPort[i] where i=0..sizeof(ppp)-1;
        eth[i].phy <--> ethPort[i] where i=0..sizeof(eth)-1;
};

//
// parameter scopes
//
module Host
{
    parameters:
        int delay;
        int mss0 = 1000;
        int mss = 500;
    submodules:
        udp: UDP {
            maxpk = 2000;
        }
        tcp: TCP {
            // reserved keywords: "this"
            // scope on right side: "parent"; one can explicitly write "this."
            // left side scope is always "this."
            mss0 = 200;
            mss = mss0; // --> 1000 (parent's)
            mss = this.mss0; // --> 200
            mss = mss;  // --> 500 (parent's)
            mss = udp.maxpk;
        }
     connections:
        tcp.out --> {delay = delay;} --> ip.in;
};


channel Eth
{
    parameters:
        @display("c=red");
        @labels(PPP, Eth);
        delay = 10us;  // TODO distance-based delay spec?
        delay = dist2delay(15m, 200000);
        delay = dist2delay(15m); // default 200,000 km/s
        // channel parameters are implemented exactly like module parameters
};


network Network   // "network" is exactly as "module", but: not gates
{
    parameters:
        int a;
    types: // defines locally used channel and module types  FIXME allow types before gates/params as well?
        channel PPPLink {
            parameters:
                @display("c=red");
                delay = 10us;  // TODO distance-based delay spec?
        };
        module HostX extends Host {
            //...
        };
    submodules:
        hosts[100] : HostX;
    connections:
        host[0].pppPort++ <--> PPPLink <--> host[1].pppPort++;
        host[0].pppPort++ <--> PPPLink {@display("c=green");} <--> host[3].pppPort++;
        host[0].pppPort[host[0].destgate] <--> SomeLink {datarate = host[0].linkspeed;} <--> host[3].pppPort++;
};

module Network
{
    parameters:
        @isnetwork(true);  // alternative to the "network" keyword
    submodules:
        hosts[100] : Host {}
    connections:
        host[0].pppPort++ <--> host[1].pppPort++;
        host[0].pppPort++ <--> host[3].pppPort++;
};

network RandomGraph
{
    submodules:
        node[100]: Node {}
    connections:
        // brief syntax:
        node[i].port++ <--> node[j].port++ where i=0..98, j=i+1..99, uniform()<0.3;

        where i=0..98, j=i+1..99, uniform()<0.3 {
            node[i].port++ <--> node[j].port++;
            node2[i].port++ <--> node2[j].port++;
        };

};

//
// "like" syntax:
//
module Host
{
    parameters:
        int N;
        string appType @moduleWithInterface(TCPApp); // "like TCPApp"
    submodules:
        apps[N]: <appType> like TCPApp; // they're all of the same type
};

module Host
{
    parameters:
        int N;
    submodules:
        app: <> like TCPApp;
        apps[N]: <> like *; // uses "typename" virtual parameter, type unchecked
        apps[N]: <> like TCPApp;  // uses "typename" virtual parameter (idea of @type property was discarded)
        apps[N]: <appType> like TCPApp;   // all modules are the same type (appType); conceptually, <> operator converts string to type
        //apps[]: <appType[]> like TCPApp;  // with string arrays; not yet implemented
};

network Network
{
    submodules:
        host: Host {
            N = 5;
            /apps[0].typename/ = "FTPApp";  // "typename" is a keyword
            /apps[1..4].typename/ = "WebBrowserApp";

            // if Host had a tcp submodule:
            /tcp*.mss/ = 1024;
        }
};

network Network2
{
    submodules:
        host: <> like AbstractHost {
            typename = "FTPApp";
        }
        // would be the same as:
        host: FTPApp;
};



channelinterface PPPLikeChannel
{
};


// "like" syntax for channels
network Network
{
    parameters:
        string channelType;
    submodules:
        hosts[100] : HostX;
    connections:
        host[0].pppPort++ <--> <channelType> like PPPLikeChannel <--> host[1].pppPort++;
        host[0].pppPort++ <--> <> like PPPLikeChannel <--> host[1].pppPort++; // how to specify typename?
        host[0].pppPort++ <--> <channelType> like * <--> host[1].pppPort++;
        host[0].pppPort++ <--> <> like * <--> host[1].pppPort++; // how to specify typename?
};

// channel labels
channel PPPLine
{
   parameters:
      @labels(PPP); // must be a superset of sender gates' labels (must be able to handle at least those pks, and possible more)
      @display("o=blue,3");
};

// channel delivers (gate expects) message on reception of first bit or last bit?
module EtherMAC
{
    gates:
        inout gate @deliverOnFirstBit;
}
channel PPPLine
{
   parameters:
      @deliverOnFirstBit;
};

// channel with or without corresponding C++ class
// (@withcppclass property idea was discarded)
channel withcppclass PPPLine   // FIXME find better syntax...
{
};


// "volatile" parameters -- "function" keyword
simple Gen
{
    parameters:
        double function interArrivalTime;
}
network Net
{
    parameters:
        double function iatime;
    submodules:
        gen1: Gen {
            interArrivalTime = 50s 200ms;
        }
        gen2: Gen {
            interArrivalTime = normal(1,0.3) * const( exponential(30) );
        }
        gen3: Gen {
            interArrivalTime = exponential(30);
        }
        gen4: Gen {
            interArrivalTime = iatime;  // by ref
            interArrivalTime = const(iatime); // by value (evaluated once)
        }
}

// units
simple Gen
{
    parameters:
        double time = 10d 4h 34mi 23s 123ms 675us 999ns; // minutes is "mi" not "m"!!!! change strToSimtime()!!!!
        double datarate = 10Tbps 10Gbps 234Mbps 123Kbps 23bps;
        double filelengthBytes = 10TB 10GB 100MB 654KB 34B;
        double distance = 100km 33m 230mm;

        double time @unit(sec);
        double datarate @unit(bps);
        double filelengthBytes @unit(bytes);
        double distance @unit(meter);

        string className = default("FIFO")
            @choice("FIFO","LIFO");

        double time = 10h 30s @unit(sec);
}

// conditional parameters
network Network
{
    submodules:
        hosts[100] : Host {
            parameters:
                // first the special cases. Stops at first match. index is reserved word.
                @display("yrtyr") if index==3;
                @display("yrtyr");
                parX = 45 if index>9;
                if index>9 {
                    parY = 3;
                    parZ = 655;
                }
            gates:
                port[5] if index==3;
                port[8];
                if index>9 {
                    portA[34];
                    portB[3];
                }
        };

};

