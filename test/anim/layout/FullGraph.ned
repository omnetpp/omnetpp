module FullGraphNode
{
    parameters:
        @display("i=misc/node_vs");
    gates:
        input in[];
        output out[];
    connections allowunconnected:
}


network FullGraph extends Base
{
    parameters:
        int n = default(10) @prompt("Number of nodes");
    submodules:
        node[n]: FullGraphNode {
            gates:
                in[n];
                out[n];
        };
    connections allowunconnected:
        for i=0..n-1, for j=0..n-1 {
            node[i].out[j] --> node[j].in[i] if i!=j;
        };
}
