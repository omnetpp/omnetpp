// These nodes should be scattered, not arranged in a ring

network FixedAndFreeNodes extends Base
{
    submodules:
        node0: Node {
            parameters:
                @display("p=100,100");
        };
        node1: Node {
            parameters:
                @display("p=200,100");
        };
        node2: Node;
        node3: Node;
    connections allowunconnected:
        node0.in++ <-- node1.out++;
        node1.in++ <-- node2.out++;
        node2.in++ <-- node3.out++;
        node3.in++ <-- node0.out++;
}

