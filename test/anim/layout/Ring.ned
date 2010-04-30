network Ring extends Base
{
    parameters:
        int numNodes = default(8) @prompt("Number of nodes");
    submodules:
        comp[numNodes]: Node {
            parameters:
                @display("i=pc,cyan");
            gates:
                in[1];
                out[1];
        };
    connections allowunconnected:
        comp[numNodes-1].out[0] --> comp[0].in[0];
        for i=0..numNodes-2 {
            comp[i].out[0] --> comp[i+1].in[0];
        };
}

