// a chain attached to the border at some nodes
network ChainToBorder extends Base
{
    parameters:
        int numNodes = default(20) @prompt("Number of nodes");
    gates:
        input in[1];
        output out[1];
    submodules:
        comp[numNodes]: Node {
            parameters:
                @display("i=ball2");
        };
    connections allowunconnected:
        for i=0..numNodes-2 {
            comp[i].out++ --> comp[i+1].in++;
        };
        comp[0].out++ --> out++;
        comp[numNodes/2].in++ <-- in++;
}

