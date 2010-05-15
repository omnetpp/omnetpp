//
// Generated network with random topology (50 nodes, 80 edges).
//
network RandomNet50 extends Base
{
    types:
        channel Link extends ned.DatarateChannel
        {
            parameters:
                int cost = default(0);
        }

        module Node
        {
            parameters:
                @display("i=abstract/router_vs");
            gates:
                inout g[];
            connections allowunconnected:
        }

    submodules:
        node0: Node;
        node1: Node;
        node2: Node;
        node3: Node;
        node4: Node;
        node5: Node;
        node6: Node;
        node7: Node;
        node8: Node;
        node9: Node;
        node10: Node;
        node11: Node;
        node12: Node;
        node13: Node;
        node14: Node;
        node15: Node;
        node16: Node;
        node17: Node;
        node18: Node;
        node19: Node;
        node20: Node;
        node21: Node;
        node22: Node;
        node23: Node;
        node24: Node;
        node25: Node;
        node26: Node;
        node27: Node;
        node28: Node;
        node29: Node;
        node30: Node;
        node31: Node;
        node32: Node;
        node33: Node;
        node34: Node;
        node35: Node;
        node36: Node;
        node37: Node;
        node38: Node;
        node39: Node;
        node40: Node;
        node41: Node;
        node42: Node;
        node43: Node;
        node44: Node;
        node45: Node;
        node46: Node;
        node47: Node;
        node48: Node;
        node49: Node;
    connections:
        node0.g++ <--> Link <--> node1.g++;
        node0.g++ <--> Link <--> node21.g++;
        node0.g++ <--> Link <--> node6.g++;
        node0.g++ <--> Link <--> node22.g++;
        node0.g++ <--> Link <--> node9.g++;
        node0.g++ <--> Link <--> node11.g++;
        node1.g++ <--> Link <--> node16.g++;
        node1.g++ <--> Link <--> node19.g++;
        node1.g++ <--> Link <--> node2.g++;
        node1.g++ <--> Link <--> node3.g++;
        node1.g++ <--> Link <--> node5.g++;
        node2.g++ <--> Link <--> node4.g++;
        node2.g++ <--> Link <--> node8.g++;
        node2.g++ <--> Link <--> node41.g++;
        node3.g++ <--> Link <--> node7.g++;
        node3.g++ <--> Link <--> node10.g++;
        node4.g++ <--> Link <--> node18.g++;
        node4.g++ <--> Link <--> node24.g++;
        node4.g++ <--> Link <--> node27.g++;
        node4.g++ <--> Link <--> node46.g++;
        node5.g++ <--> Link <--> node33.g++;
        node5.g++ <--> Link <--> node12.g++;
        node5.g++ <--> Link <--> node14.g++;
        node6.g++ <--> Link <--> node32.g++;
        node6.g++ <--> Link <--> node18.g++;
        node6.g++ <--> Link <--> node41.g++;
        node6.g++ <--> Link <--> node13.g++;
        node7.g++ <--> Link <--> node34.g++;
        node7.g++ <--> Link <--> node17.g++;
        node7.g++ <--> Link <--> node16.g++;
        node7.g++ <--> Link <--> node9.g++;
        node7.g++ <--> Link <--> node26.g++;
        node8.g++ <--> Link <--> node35.g++;
        node8.g++ <--> Link <--> node9.g++;
        node8.g++ <--> Link <--> node28.g++;
        node9.g++ <--> Link <--> node35.g++;
        node9.g++ <--> Link <--> node37.g++;
        node9.g++ <--> Link <--> node12.g++;
        node10.g++ <--> Link <--> node16.g++;
        node10.g++ <--> Link <--> node31.g++;
        node12.g++ <--> Link <--> node15.g++;
        node13.g++ <--> Link <--> node26.g++;
        node14.g++ <--> Link <--> node49.g++;
        node14.g++ <--> Link <--> node38.g++;
        node15.g++ <--> Link <--> node16.g++;
        node16.g++ <--> Link <--> node29.g++;
        node17.g++ <--> Link <--> node42.g++;
        node17.g++ <--> Link <--> node25.g++;
        node17.g++ <--> Link <--> node40.g++;
        node17.g++ <--> Link <--> node29.g++;
        node18.g++ <--> Link <--> node45.g++;
        node19.g++ <--> Link <--> node32.g++;
        node19.g++ <--> Link <--> node33.g++;
        node19.g++ <--> Link <--> node20.g++;
        node19.g++ <--> Link <--> node44.g++;
        node20.g++ <--> Link <--> node48.g++;
        node20.g++ <--> Link <--> node43.g++;
        node22.g++ <--> Link <--> node23.g++;
        node22.g++ <--> Link <--> node45.g++;
        node23.g++ <--> Link <--> node32.g++;
        node23.g++ <--> Link <--> node47.g++;
        node24.g++ <--> Link <--> node35.g++;
        node24.g++ <--> Link <--> node46.g++;
        node26.g++ <--> Link <--> node36.g++;
        node26.g++ <--> Link <--> node31.g++;
        node27.g++ <--> Link <--> node38.g++;
        node27.g++ <--> Link <--> node29.g++;
        node28.g++ <--> Link <--> node30.g++;
        node29.g++ <--> Link <--> node40.g++;
        node30.g++ <--> Link <--> node42.g++;
        node31.g++ <--> Link <--> node45.g++;
        node32.g++ <--> Link <--> node33.g++;
        node32.g++ <--> Link <--> node39.g++;
        node33.g++ <--> Link <--> node49.g++;
        node34.g++ <--> Link <--> node43.g++;
        node35.g++ <--> Link <--> node39.g++;
        node37.g++ <--> Link <--> node46.g++;
        node39.g++ <--> Link <--> node44.g++;
        node41.g++ <--> Link <--> node47.g++;
        node47.g++ <--> Link <--> node48.g++;
}
