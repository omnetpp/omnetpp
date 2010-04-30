//
// Converted from geant.gml by gml2ned.pl
//

module geantNode
{
    parameters:
        @display("i=misc/node_s");
    gates:
        input in[];
        output out[];
    connections allowunconnected:
}

network geant extends Base
{
    submodules:
        node0: geantNode {
            parameters:
                @display("t=se1");
        };
        node1: geantNode {
            parameters:
                @display("t=nl1");
        };
        node10: geantNode {
            parameters:
                @display("t=de2");
        };
        node11: geantNode {
            parameters:
                @display("t=pl1");
        };
        node12: geantNode {
            parameters:
                @display("t=fr1");
        };
        node13: geantNode {
            parameters:
                @display("t=uk1");
        };
        node14: geantNode {
            parameters:
                @display("t=lu1");
        };
        node15: geantNode {
            parameters:
                @display("t=at1");
        };
        node16: geantNode {
            parameters:
                @display("t=il1");
        };
        node17: geantNode {
            parameters:
                @display("t=cz1");
        };
        node18: geantNode {
            parameters:
                @display("t=gr1");
        };
        node19: geantNode {
            parameters:
                @display("t=hu1");
        };
        node2: geantNode {
            parameters:
                @display("t=be1");
        };
        node20: geantNode {
            parameters:
                @display("t=de1");
        };
        node21: geantNode {
            parameters:
                @display("t=es1");
        };
        node22: geantNode {
            parameters:
                @display("t=sk1");
        };
        node3: geantNode {
            parameters:
                @display("t=hr1");
        };
        node4: geantNode {
            parameters:
                @display("t=ie1");
        };
        node5: geantNode {
            parameters:
                @display("t=ny1");
        };
        node6: geantNode {
            parameters:
                @display("t=pt1");
        };
        node7: geantNode {
            parameters:
                @display("t=ch1");
        };
        node8: geantNode {
            parameters:
                @display("t=si1");
        };
        node9: geantNode {
            parameters:
                @display("t=it1");
        };
    connections allowunconnected:
        node0.out++ --> node10.in++;
        node0.out++ --> node11.in++;
        node0.out++ --> node13.in++;
        node1.out++ --> node13.in++;
        node1.out++ --> node16.in++;
        node1.out++ --> node2.in++;
        node1.out++ --> node20.in++;
        node10.out++ --> node15.in++;
        node10.out++ --> node18.in++;
        node10.out++ --> node20.in++;
        node11.out++ --> node17.in++;
        node12.out++ --> node13.in++;
        node12.out++ --> node14.in++;
        node12.out++ --> node20.in++;
        node12.out++ --> node21.in++;
        node15.out++ --> node19.in++;
        node17.out++ --> node20.in++;
        node17.out++ --> node22.in++;
        node19.out++ --> node22.in++;
        node2.out++ --> node12.in++;
        node2.out++ --> node14.in++;
        node3.out++ --> node19.in++;
        node3.out++ --> node8.in++;
        node4.out++ --> node13.in++;
        node4.out++ --> node20.in++;
        node5.out++ --> node13.in++;
        node5.out++ --> node15.in++;
        node6.out++ --> node13.in++;
        node6.out++ --> node21.in++;
        node7.out++ --> node12.in++;
        node7.out++ --> node15.in++;
        node7.out++ --> node9.in++;
        node8.out++ --> node15.in++;
        node9.out++ --> node10.in++;
        node9.out++ --> node16.in++;
        node9.out++ --> node18.in++;
        node9.out++ --> node21.in++;
}
