network Dyna extends Base
{
    parameters:
        int numNodes = default(10) @prompt("Number of nodes");
    submodules:
        server: Node {
            parameters:
                @display("i=server1,gold");
            gates:
                in[1];
                out[1];
        };
        switch: Node {
            parameters:
                @display("i=switch2,cyan");
            gates:
                in[numNodes+1];
                out[numNodes+1];
        };
        client[numNodes]: Node {
            parameters:
                @display("i=pc,green;p=,,m,10");
            gates:
                in[1];
                out[1];
        };
    connections allowunconnected:
        server.out[0] --> switch.in[0];
        for i=0..numNodes-1 {
            client[i].out[0] --> switch.in[i+1];
        };
}

