//
// Converted from belnet.gml by gml2ned.pl
//

module belnetNode
{
    parameters:
        @display("i=misc/node_s");
    gates:
        input in[];
        output out[];
    connections allowunconnected:
}

network belnet extends Base
{
    submodules:
        node0: belnetNode {
            parameters:
                @display("t=Brussels1");
        };
        node1: belnetNode {
            parameters:
                @display("t=Brussels2");
        };
        node2: belnetNode {
            parameters:
                @display("t=Antwerpen");
        };
        node3: belnetNode {
            parameters:
                @display("t=Arlon");
        };
        node4: belnetNode {
            parameters:
                @display("t=Brugge");
        };
        node5: belnetNode {
            parameters:
                @display("t=Charleroi");
        };
        node6: belnetNode {
            parameters:
                @display("t=Geel");
        };
        node7: belnetNode {
            parameters:
                @display("t=Gent");
        };
        node8: belnetNode {
            parameters:
                @display("t=Hasselt");
        };
        node9: belnetNode {
            parameters:
                @display("t=Kortrijk");
        };
        node10: belnetNode {
            parameters:
                @display("t=Leuven");
        };
        node11: belnetNode {
            parameters:
                @display("t=Liege");
        };
        node12: belnetNode {
            parameters:
                @display("t=Louvain-la-Neuve");
        };
        node13: belnetNode {
            parameters:
                @display("t=Mons");
        };
        node14: belnetNode {
            parameters:
                @display("t=Namur");
        };
    connections allowunconnected:
        node0.out++ --> node1.in++;
        node0.out++ --> node2.in++;
        node1.out++ --> node2.in++;
        node0.out++ --> node3.in++;
        node1.out++ --> node3.in++;
        node0.out++ --> node4.in++;
        node1.out++ --> node4.in++;
        node0.out++ --> node5.in++;
        node1.out++ --> node5.in++;
        node0.out++ --> node6.in++;
        node1.out++ --> node6.in++;
        node0.out++ --> node7.in++;
        node1.out++ --> node7.in++;
        node0.out++ --> node8.in++;
        node1.out++ --> node8.in++;
        node0.out++ --> node9.in++;
        node1.out++ --> node9.in++;
        node0.out++ --> node10.in++;
        node1.out++ --> node10.in++;
        node0.out++ --> node11.in++;
        node1.out++ --> node11.in++;
        node0.out++ --> node12.in++;
        node1.out++ --> node12.in++;
        node0.out++ --> node13.in++;
        node1.out++ --> node13.in++;
        node0.out++ --> node14.in++;
        node1.out++ --> node14.in++;
}
