//
// A generated network with k-ary tree topology.
//
network RegularTree extends Base
{
    parameters:
        int levels = default(4);  // 1 = root only
        int k = default(4); // tree branching factor
    types:
        module Node
        {
            parameters:
                @display("i=abstract/router_vs");
            gates:
                inout g[];
            connections allowunconnected:
        }
    submodules:
        node[(k^levels-1) / (k-1)]: Node;
    connections allowunconnected:
        for i=1..sizeof(node)-1 {
            node[i].g++ <--> node[floor((i-1)/k)].g++;
        }
}
