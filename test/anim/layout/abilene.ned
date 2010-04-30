//
// Converted from abilene.gml by gml2ned.pl
//

module abileneNode
{
    parameters:
        @display("i=misc/node_s");
    gates:
        input in[];
        output out[];
    connections allowunconnected:
}

network abilene extends Base
{
    submodules:
        node0: abileneNode {
            parameters:
                @display("t=Sunnyvale");
        };
        node1: abileneNode {
            parameters:
                @display("t=Denver");
        };
        node2: abileneNode {
            parameters:
                @display("t=Los Angeles");
        };
        node3: abileneNode {
            parameters:
                @display("t=Seattle");
        };
        node4: abileneNode {
            parameters:
                @display("t=Kansas City");
        };
        node5: abileneNode {
            parameters:
                @display("t=Houston");
        };
        node6: abileneNode {
            parameters:
                @display("t=Atlanta");
        };
        node7: abileneNode {
            parameters:
                @display("t=Chicago");
        };
        node8: abileneNode {
            parameters:
                @display("t=Indianapolis");
        };
        node9: abileneNode {
            parameters:
                @display("t=Washington");
        };
        node10: abileneNode {
            parameters:
                @display("t=New York");
        };
    connections allowunconnected:
        node3.out++ --> node0.in++;
        node3.out++ --> node1.in++;
        node0.out++ --> node1.in++;
        node0.out++ --> node2.in++;
        node2.out++ --> node5.in++;
        node1.out++ --> node4.in++;
        node5.out++ --> node4.in++;
        node5.out++ --> node6.in++;
        node4.out++ --> node8.in++;
        node6.out++ --> node8.in++;
        node6.out++ --> node9.in++;
        node9.out++ --> node10.in++;
        node10.out++ --> node7.in++;
        node7.out++ --> node8.in++;
}
