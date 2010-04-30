module NetworkLayers extends Base
{
    parameters:
        int numPorts;
    gates:
        input TCPIn;
        output TCPOut;
        input physIn[];
        output physOut[];
    submodules:
        proc: Node {
            parameters:
                @display("i=cogwheel");
            gates:
                in[numPorts+2];
                out[numPorts+2];
        };
        outputQueue[numPorts]: Node {
            parameters:
                @display("p=,,row;i=queue");
            gates:
                in[2];
                out[2];
        };
        inputQueue: Node {
            parameters:
                @display("i=queue");
            gates:
                in[numPorts];
                out[1];
        };
        networkInterface[numPorts]: Node {
            parameters:
                @display("p=,,row;i=bwxconn");
        //routingTable: Node;
        //    display: "p=,,row;i=box3";

            gates:
                in[3];
                out[3];
        };
    connections allowunconnected:
        proc.out[numPorts] --> TCPOut;
        proc.in[numPorts] <-- TCPIn;
        proc.in[numPorts+1] <-- inputQueue.out[0];

        for i=0..numPorts-1 {
            proc.out[i] --> outputQueue[i].in[0];
            networkInterface[i].out[0] --> inputQueue.in[i];
            networkInterface[i].out[2] --> outputQueue[i].in[1];
            networkInterface[i].in[2] <-- outputQueue[i].out[0];
            networkInterface[i].out[1] --> physOut[i];
            networkInterface[i].in[1] <-- physIn[i];
        };
}

network NetworkLayersWrapper
{
    parameters:
        int numPorts = default(3) @prompt("Number of ports");
    submodules:
        doubleClickHere: NetworkLayers {
            parameters:
                numPorts = numPorts;
                @display("i=cloud_l");
            gates:
                physIn[numPorts];
                physOut[numPorts];
        };
    connections allowunconnected:
}

