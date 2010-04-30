module StandardHostNode
{
    gates:
        input in[];
        output out[];
    connections allowunconnected:
}

network StandardHost extends Base
{
    parameters:
        int numTcpApps = default(2);
        int numUdpApps = default(2);
        int numPpp = default(2);
        int numEth = default(2);
    gates:
        input in[numPpp];
        output out[numPpp];
        input ethIn[numEth];
        output ethOut[numEth];
    submodules:
        namTrace: StandardHostNode {
            parameters:
                @display("i=block/sink");
        };
        notificationBoard: StandardHostNode {
            parameters:
                @display("i=block/control");
        };
        interfaceTable: StandardHostNode {
            parameters:
                @display("i=block/table");
        };
        routingTable: StandardHostNode {
            parameters:
                @display("i=block/table");
        };
        tcpApp[numTcpApps]: StandardHostNode {
            parameters:
                @display("i=block/app");
        };
        tcp: StandardHostNode {
            parameters:
                @display("i=block/wheelbarrow");
        };
        udpApp[numUdpApps]: StandardHostNode {
            parameters:
                @display("i=block/app");
        };
        udp: StandardHostNode {
            parameters:
                @display("i=block/transport");
        };
        pingApp: StandardHostNode {
            parameters:
                @display("i=block/app");
        };
        networkLayer: StandardHostNode {
            parameters:
                @display("i=block/fork;q=queue");
        };
        ppp[numPpp]: StandardHostNode {
            parameters:
                @display("p=,,row;q=txQueue;i=block/ifcard");
        };
        eth[numEth]: StandardHostNode {
            parameters:
                @display("p=,,row;q=txQueue;i=block/ifcard");
        };
    connections allowunconnected:
        for i=0..numTcpApps-1 {
            tcpApp[i].out++ --> tcp.in++;
            tcpApp[i].in++ <-- tcp.out++;
        };

        tcp.out++ --> networkLayer.in++;
        tcp.in++ <-- networkLayer.out++;

        for i=0..numUdpApps-1 {
            udpApp[i].out++ --> udp.in++;
            udpApp[i].in++ <-- udp.out++;
        };

        udp.out++ --> networkLayer.in++;
        udp.in++ <-- networkLayer.out++;

        networkLayer.out++ --> pingApp.in++;
        networkLayer.in++ <-- pingApp.out++;

        // connections to network outside
        for i=0..sizeof(out)-1 {
            in[i] --> ppp[i].in++;
            out[i] <-- ppp[i].out++;
            ppp[i].out++ --> networkLayer.in++;
            ppp[i].in++ <-- networkLayer.out++;
        };

        for i=0..sizeof(ethOut)-1 {
            ethIn[i] --> eth[i].in++;
            ethOut[i] <-- eth[i].out++;
            eth[i].out++ --> networkLayer.in++;
            eth[i].in++ <-- networkLayer.out++;
        };
}
