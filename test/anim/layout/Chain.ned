network Chain extends Base
{
    parameters:
        int numNodes = default(20) @prompt("Number of nodes");
    submodules:
        comp[numNodes]: Node {
            parameters:
                @display("i=ball2");
            gates:
                in[1];
                out[1];
        };
    connections allowunconnected:
        for i=0..numNodes-2 {
            comp[i].out[0] --> comp[i+1].in[0];
        };
}

