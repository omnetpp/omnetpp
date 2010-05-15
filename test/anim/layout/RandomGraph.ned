network RandomGraph extends Base
{
    parameters:
        int numNodes = default(60) @prompt("Number of nodes");
        double connectedness = default(0.9);
    submodules:
        vec[numNodes]: Node {
            parameters:
                @display("i=ball2,gold,100;is=vs");
            gates:
                in[numNodes];
                out[numNodes];
        };
    connections allowunconnected:
        for i=0..numNodes-1, for j=0..numNodes-1 {
            vec[i].out[j] --> vec[j].in[i] if uniform(0, 1)<connectedness/numNodes && i!=j;
        };
}

