import "foo.ned";
@someproperty(somekey);
simple Queue
{
    parameters:
        @display(i="queue");
        double msgPerSec = default(100);
        int capacity = 100;
        int processingTime = exponential(10);
        int novalue;
        int function waitTime = exponential(20);
    gates:
        input in[];
        input in2[5];
        output out;
        inout thirdGate;
}

simple VeryFastQueue extends Queue
{
}

module VeryFastQueue extends Queue
{
}

simple VeryFastQueue extends Queue
{
    parameters:
        msgPerSec = 1000000;
        bool isVeryFast = true;
        novalue @addedproperty;
}

simple QueueWithFourInputs extends Queue
{
    gates:
        in[4];
        input plusOneInput;
}

simple QueueWithNInputs extends Queue
{
    parameters:
        int numInputs @prompt("Number of input ports");
    gates:
        input in[numInputs];
}

simple TCP
{
    parameters:
        int mss;
        int advertisedWindow;
        string tcpAlgorithmClass @enum("TCPTahoe", "TCPReno", "TCPNoCongestionControl", "DumbTCP");
        string tcpAlgorithmClass @classname("ITCPAlgorithm");
        @kernel(starttime=49);
        @recordstats[stat1](storeintervall=5; averaging=true);
        @recordstats[stat2](storeintervall=9; averaging=false);
        @display(i="block/transport");
    gates:
        inout toApp[] @labels(TCPControlInfo);
        inout toNetw @labels(TCPSegment);
}

simple MobileHost
{
    parameters:
        double x;
        double y;
        @display(p=$x,$y);
}

simple TCP
{
    gates:
        inout toApp[] @inlabels(tcp2app) @outlabels(app2tcp) @mustbeconnected;
        inout toNetw @inlabels(tcp2net) @outlabels(net2tcp);
}

simple Queue
{
    gates:
        input in[];
        output out;
}

moduleinterface TCPApp
{
    gates:
        inout toTCP @inlabels(tcp2app) @outlabels(app2tcp);
}

moduleinterface UDPApp
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

module FTPApp extends GenericFTPApp like TCPApp
{
    parameters:
        connectInterval = 10s;
        fileLength = normal(10Mb, 1Mb);
}

module FlexibleApp like TCPApp, UDPApp
{
    parameters:
        connectInterval = 10s;
        fileLength = normal(10Mb, 1Mb);
    gates:
        inout toTCP @inlabels(tcp2app) @outlabels(app2tcp);
        inout toUDP @inlabels(udp2app) @outlabels(app2udp);
}

moduleinterface TCPUDPApp extends TCPApp, UDPApp
{
}

module OSPFRouting like TCPUDPApp
{
}

property @host;

property @host2(foo; bar);

module Host
{
    parameters:
        @host;
    gates:
        inout pppPort[] @labels(PPP);
        inout ethPort[] @labels(Ethernet);
        inout ethBusPort[] @labels(EthernetBus);
    submodules:
        apps: <> like TCPApp;
        tcp: TCP {
            mss = 1024;
        }
        ip: IP;
        ppp[sizeof(pppPort)]: PPP;
    connections allowunconnected:
        app[i].toTcp <--> { @display(c=red); delay=0; } <--> tcp.toApp;
        app[i].toTcp$o --> { @display(c=red); delay=0; } --> tcp.toApp$i;
        app[i].toTcp$i <-- { @display(c=red); delay=0; } <-- tcp.toApp$o;
        tcp.toNetw <--> ip.toTcp;
        ppp[i].toNetw <--> ip.toIf[i] where i=0..sizeof(ppp)-1;
        ppp[i].toNetw <--> ip.toIf++ where i=0..sizeof(ppp)-1;
        eth[i].toNetw <--> ip.toIf++ where i=0..sizeof(eth)-1;
        ppp[i].phy <--> pppPort[i] where i=0..sizeof(ppp)-1;
        eth[i].phy <--> ethPort[i] where i=0..sizeof(eth)-1;
}

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
            mss0 = 200;
            mss = mss0;
            mss = this.mss0;
            mss = mss;
            mss = udp.maxpk;
        }
     connections:
        tcp.out --> {delay = delay;} --> ip.in;
}

channel Eth
{
    parameters:
        @display(c=red);
        @labels(PPP, Eth);
        delay = 10us;
        delay = dist2delay(15m, 200000);
        delay = dist2delay(15m);
}

network Network
{
    parameters:
        int a;
    types:
        channel PPPLink
        {
            parameters:
                @display(c=red);
                delay = 10us;
        }

        module HostX extends Host
        {
        }

    submodules:
        hosts[100]: HostX;
    connections:
        host[0].pppPort++ <--> PPPLink <--> host[1].pppPort++;
        host[0].pppPort++ <--> PPPLink {@display(c=green);} <--> host[3].pppPort++;
        host[0].pppPort[host[0].destgate] <--> SomeLink {datarate = host[0].linkspeed;} <--> host[3].pppPort++;
}

module Network
{
    parameters:
        @isnetwork(true);
    submodules:
        hosts[100]: Host {
        }
    connections:
        host[0].pppPort++ <--> host[1].pppPort++;
        host[0].pppPort++ <--> host[3].pppPort++;
}

network RandomGraph
{
    submodules:
        node[100]: Node {
        }
    connections:
        node[i].port++ <--> node[j].port++ where i=0..98, j=i+1..99, uniform()<0.3;
        where i=0..98, j=i+1..99, uniform()<0.3 {
            node[i].port++ <--> node[j].port++;
            node2[i].port++ <--> node2[j].port++;
        };
}

module Host
{
    parameters:
        int N;
        string appType @moduleWithInterface(TCPApp);
    submodules:
        apps[N]: <appType> like TCPApp;
}

module Host
{
    parameters:
        int N;
    submodules:
        app: <> like TCPApp;
        apps[N]: <> like *;
        apps[N]: <> like TCPApp;
        apps[N]: <appType> like TCPApp;
        tcp: TCP;
}

network Network
{
    submodules:
        host: Host {
            N = 5;
            /apps[0].typename/ = "FTPApp";
            /apps[1..4].typename/ = "WebBrowserApp";
            /tcp*.mss/ = 1024;
        }
}

network Network2
{
    submodules:
        host: <> like AbstractHost {
            typename = "FTPApp";
        }
        host: FTPApp;
}

channelinterface PPPLikeChannel
{
}

network Network
{
    parameters:
        string channelType;
    submodules:
        hosts[100]: HostX;
    connections:
        host[0].pppPort++ <--> <channelType> like PPPLikeChannel <--> host[1].pppPort++;
        host[0].pppPort++ <--> <> like PPPLikeChannel <--> host[1].pppPort++;
        host[0].pppPort++ <--> <channelType> like * <--> host[1].pppPort++;
        host[0].pppPort++ <--> <> like * <--> host[1].pppPort++;
}

channel PPPLine
{
    parameters:
        @labels(PPP);
        @display(o=blue,3);
}

module EtherMAC
{
    gates:
        inout gate @deliverOnFirstBit;
}

channel PPPLine
{
    parameters:
        @deliverOnFirstBit;
}

channel withcppclass PPPLine
{
}

moduleinterface IBidirectionalChannel
{
    gates:
        inout a;
        inout b;
}

moduleinterface IUnidirectionalChannel
{
    gates:
        input i;
        output o;
}

module PPPLineModule like IBidirectionalChannel
{
    gates:
        inout a;
        inout b;
}

network PPPNet
{
    submodules:
        node[10]: Node;
    connections:
        node[0].g++ <--> PPPLineModule <--> node[1].g++;
        node[0].g++ <--> PPPLineModule <--> node[3].g++;
}

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
            interArrivalTime = iatime;
            interArrivalTime = const(iatime);
        }
}

simple Gen
{
    parameters:
        double time = 10d 4h 34mi 23s 123ms 675us 999ns;
        double datarate = 10Tbps 10Gbps 234Mbps 123Kbps 23bps;
        double filelengthBytes = 10TB 10GB 100MB 654KB 34B;
        double distance = 100km 33m 230mm;
        double time @unit(sec);
        double datarate @unit(bps);
        double filelengthBytes @unit(bytes);
        double distance @unit(meter);
        string className = default("FIFO") @enum("FIFO", "LIFO");
        double time = 10h 30s @unit(sec);
}

network Network
{
    submodules:
        hosts[100]: Host {
            parameters:
                @display(c=green) if index==3;
                @display(c=red);
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
        }
}

