//
// OMNeT++ network description -- exported from Brite
// Generator Model Used:
//   Model (3 - ASWaxman):  50 1000 100 1  2  0.15000000596046448 0.20000000298023224 1 1 10.0 1024.0
//


//
// Prototype for modules that can be used as node in a Brite50Network
//
module Brite50Node
{
    gates:
        input in[];
        output out[];
    connections allowunconnected:
}

//
// Topology exported from Brite
//
network Brite50 extends Base
{
    submodules:
        node0: Brite50Node {
            parameters:
                @display("i=router_s;ppp=880,197");
            gates:
                in[2];
                out[2];
        };
        node1: Brite50Node {
            parameters:
                @display("i=router_s;ppp=218,551");
            gates:
                in[8];
                out[8];
        };
        node2: Brite50Node {
            parameters:
                @display("i=router_s;ppp=540,977");
            gates:
                in[4];
                out[4];
        };
        node3: Brite50Node {
            parameters:
                @display("i=router_s;ppp=735,641");
            gates:
                in[4];
                out[4];
        };
        node4: Brite50Node {
            parameters:
                @display("i=router_s;ppp=732,761");
            gates:
                in[4];
                out[4];
        };
        node5: Brite50Node {
            parameters:
                @display("i=router_s;ppp=995,197");
            gates:
                in[2];
                out[2];
        };
        node6: Brite50Node {
            parameters:
                @display("i=router_s;ppp=29,603");
            gates:
                in[3];
                out[3];
        };
        node7: Brite50Node {
            parameters:
                @display("i=router_s;ppp=514,7");
            gates:
                in[3];
                out[3];
        };
        node8: Brite50Node {
            parameters:
                @display("i=router_s;ppp=917,470");
            gates:
                in[5];
                out[5];
        };
        node9: Brite50Node {
            parameters:
                @display("i=router_s;ppp=203,455");
            gates:
                in[3];
                out[3];
        };
        node10: Brite50Node {
            parameters:
                @display("i=router_s;ppp=386,219");
            gates:
                in[2];
                out[2];
        };
        node11: Brite50Node {
            parameters:
                @display("i=router_s;ppp=879,392");
            gates:
                in[3];
                out[3];
        };
        node12: Brite50Node {
            parameters:
                @display("i=router_s;ppp=749,746");
            gates:
                in[3];
                out[3];
        };
        node13: Brite50Node {
            parameters:
                @display("i=router_s;ppp=277,169");
            gates:
                in[4];
                out[4];
        };
        node14: Brite50Node {
            parameters:
                @display("i=router_s;ppp=613,284");
            gates:
                in[3];
                out[3];
        };
        node15: Brite50Node {
            parameters:
                @display("i=router_s;ppp=509,731");
            gates:
                in[10];
                out[10];
        };
        node16: Brite50Node {
            parameters:
                @display("i=router_s;ppp=68,632");
            gates:
                in[3];
                out[3];
        };
        node17: Brite50Node {
            parameters:
                @display("i=router_s;ppp=437,529");
            gates:
                in[6];
                out[6];
        };
        node18: Brite50Node {
            parameters:
                @display("i=router_s;ppp=387,221");
            gates:
                in[9];
                out[9];
        };
        node19: Brite50Node {
            parameters:
                @display("i=router_s;ppp=551,624");
            gates:
                in[3];
                out[3];
        };
        node20: Brite50Node {
            parameters:
                @display("i=router_s;ppp=731,305");
            gates:
                in[4];
                out[4];
        };
        node21: Brite50Node {
            parameters:
                @display("i=router_s;ppp=539,752");
            gates:
                in[5];
                out[5];
        };
        node22: Brite50Node {
            parameters:
                @display("i=router_s;ppp=625,251");
            gates:
                in[4];
                out[4];
        };
        node23: Brite50Node {
            parameters:
                @display("i=router_s;ppp=885,335");
            gates:
                in[5];
                out[5];
        };
        node24: Brite50Node {
            parameters:
                @display("i=router_s;ppp=256,832");
            gates:
                in[2];
                out[2];
        };
        node25: Brite50Node {
            parameters:
                @display("i=router_s;ppp=524,372");
            gates:
                in[2];
                out[2];
        };
        node26: Brite50Node {
            parameters:
                @display("i=router_s;ppp=977,702");
            gates:
                in[3];
                out[3];
        };
        node27: Brite50Node {
            parameters:
                @display("i=router_s;ppp=793,895");
            gates:
                in[2];
                out[2];
        };
        node28: Brite50Node {
            parameters:
                @display("i=router_s;ppp=730,697");
            gates:
                in[4];
                out[4];
        };
        node29: Brite50Node {
            parameters:
                @display("i=router_s;ppp=639,226");
            gates:
                in[6];
                out[6];
        };
        node30: Brite50Node {
            parameters:
                @display("i=router_s;ppp=508,458");
            gates:
                in[8];
                out[8];
        };
        node31: Brite50Node {
            parameters:
                @display("i=router_s;ppp=357,556");
            gates:
                in[6];
                out[6];
        };
        node32: Brite50Node {
            parameters:
                @display("i=router_s;ppp=381,899");
            gates:
                in[5];
                out[5];
        };
        node33: Brite50Node {
            parameters:
                @display("i=router_s;ppp=894,492");
            gates:
                in[2];
                out[2];
        };
        node34: Brite50Node {
            parameters:
                @display("i=router_s;ppp=57,457");
            gates:
                in[8];
                out[8];
        };
        node35: Brite50Node {
            parameters:
                @display("i=router_s;ppp=138,62");
            gates:
                in[3];
                out[3];
        };
        node36: Brite50Node {
            parameters:
                @display("i=router_s;ppp=425,644");
            gates:
                in[2];
                out[2];
        };
        node37: Brite50Node {
            parameters:
                @display("i=router_s;ppp=428,365");
            gates:
                in[5];
                out[5];
        };
        node38: Brite50Node {
            parameters:
                @display("i=router_s;ppp=47,402");
            gates:
                in[2];
                out[2];
        };
        node39: Brite50Node {
            parameters:
                @display("i=router_s;ppp=971,448");
            gates:
                in[2];
                out[2];
        };
        node40: Brite50Node {
            parameters:
                @display("i=router_s;ppp=84,919");
            gates:
                in[2];
                out[2];
        };
        node41: Brite50Node {
            parameters:
                @display("i=router_s;ppp=740,238");
            gates:
                in[4];
                out[4];
        };
        node42: Brite50Node {
            parameters:
                @display("i=router_s;ppp=738,81");
            gates:
                in[2];
                out[2];
        };
        node43: Brite50Node {
            parameters:
                @display("i=router_s;ppp=922,364");
            gates:
                in[3];
                out[3];
        };
        node44: Brite50Node {
            parameters:
                @display("i=router_s;ppp=881,483");
            gates:
                in[5];
                out[5];
        };
        node45: Brite50Node {
            parameters:
                @display("i=router_s;ppp=959,460");
            gates:
                in[2];
                out[2];
        };
        node46: Brite50Node {
            parameters:
                @display("i=router_s;ppp=168,977");
            gates:
                in[2];
                out[2];
        };
        node47: Brite50Node {
            parameters:
                @display("i=router_s;ppp=744,122");
            gates:
                in[3];
                out[3];
        };
        node48: Brite50Node {
            parameters:
                @display("i=router_s;ppp=618,68");
            gates:
                in[9];
                out[9];
        };
        node49: Brite50Node {
            parameters:
                @display("i=router_s;ppp=620,44");
            gates:
                in[4];
                out[4];
        };
    connections:
        node0.out[0] --> {  delay = 1.6084846664921484ms; datarate = 10Mbps;} --> node37.in[0];
        node0.in[0] <-- {  delay = 1.6084846664921484ms; datarate = 10Mbps;} <-- node37.out[0];
        node0.out[1] --> {  delay = 2.1689285505735354ms; datarate = 10Mbps;} --> node15.in[0];
        node0.in[1] <-- {  delay = 2.1689285505735354ms; datarate = 10Mbps;} <-- node15.out[0];
        node1.out[0] --> {  delay = 2.429723091200331ms; datarate = 10Mbps;} --> node43.in[0];
        node1.in[0] <-- {  delay = 2.429723091200331ms; datarate = 10Mbps;} <-- node43.out[0];
        node1.out[1] --> {  delay = 0.4639539630654457ms; datarate = 10Mbps;} --> node31.in[0];
        node1.in[1] <-- {  delay = 0.4639539630654457ms; datarate = 10Mbps;} <-- node31.out[0];
        node2.out[0] --> {  delay = 2.572869191386962ms; datarate = 10Mbps;} --> node18.in[0];
        node2.in[0] <-- {  delay = 2.572869191386962ms; datarate = 10Mbps;} <-- node18.out[0];
        node2.out[1] --> {  delay = 1.7812447613515943ms; datarate = 10Mbps;} --> node1.in[2];
        node2.in[1] <-- {  delay = 1.7812447613515943ms; datarate = 10Mbps;} <-- node1.out[2];
        node3.out[0] --> {  delay = 0.8114319424143279ms; datarate = 10Mbps;} --> node15.in[1];
        node3.in[0] <-- {  delay = 0.8114319424143279ms; datarate = 10Mbps;} <-- node15.out[1];
        node3.out[1] --> {  delay = 1.4611478116726189ms; datarate = 10Mbps;} --> node32.in[0];
        node3.in[1] <-- {  delay = 1.4611478116726189ms; datarate = 10Mbps;} <-- node32.out[0];
        node4.out[0] --> {  delay = 0.6444782897234133ms; datarate = 10Mbps;} --> node21.in[0];
        node4.in[0] <-- {  delay = 0.6444782897234133ms; datarate = 10Mbps;} <-- node21.out[0];
        node4.out[1] --> {  delay = 2.038502403689378ms; datarate = 10Mbps;} --> node9.in[0];
        node4.in[1] <-- {  delay = 2.038502403689378ms; datarate = 10Mbps;} <-- node9.out[0];
        node5.out[0] --> {  delay = 1.9725825434000954ms; datarate = 10Mbps;} --> node37.in[1];
        node5.in[0] <-- {  delay = 1.9725825434000954ms; datarate = 10Mbps;} <-- node37.out[1];
        node5.out[1] --> {  delay = 0.9514473635494629ms; datarate = 10Mbps;} --> node20.in[0];
        node5.in[1] <-- {  delay = 0.9514473635494629ms; datarate = 10Mbps;} <-- node20.out[0];
        node6.out[0] --> {  delay = 2.1122720812606812ms; datarate = 10Mbps;} --> node2.in[2];
        node6.in[0] <-- {  delay = 2.1122720812606812ms; datarate = 10Mbps;} <-- node2.out[2];
        node6.out[1] --> {  delay = 1.3831451501482377ms; datarate = 10Mbps;} --> node17.in[0];
        node6.in[1] <-- {  delay = 1.3831451501482377ms; datarate = 10Mbps;} <-- node17.out[0];
        node7.out[0] --> {  delay = 0.8300653873106643ms; datarate = 10Mbps;} --> node18.in[1];
        node7.in[0] <-- {  delay = 0.8300653873106643ms; datarate = 10Mbps;} <-- node18.out[1];
        node7.out[1] --> {  delay = 0.40217650186020876ms; datarate = 10Mbps;} --> node48.in[0];
        node7.in[1] <-- {  delay = 0.40217650186020876ms; datarate = 10Mbps;} <-- node48.out[0];
        node8.out[0] --> {  delay = 0.12767271956524429ms; datarate = 10Mbps;} --> node44.in[0];
        node8.in[0] <-- {  delay = 0.12767271956524429ms; datarate = 10Mbps;} <-- node44.out[0];
        node8.out[1] --> {  delay = 0.9810300940772709ms; datarate = 10Mbps;} --> node28.in[0];
        node8.in[1] <-- {  delay = 0.9810300940772709ms; datarate = 10Mbps;} <-- node28.out[0];
        node9.out[1] --> {  delay = 0.48704927055753966ms; datarate = 10Mbps;} --> node34.in[0];
        node9.in[1] <-- {  delay = 0.48704927055753966ms; datarate = 10Mbps;} <-- node34.out[0];
        node9.out[2] --> {  delay = 0.32410691413794634ms; datarate = 10Mbps;} --> node1.in[3];
        node9.in[2] <-- {  delay = 0.32410691413794634ms; datarate = 10Mbps;} <-- node1.out[3];
        node10.out[0] --> {  delay = 0.506754016202314ms; datarate = 10Mbps;} --> node37.in[2];
        node10.in[0] <-- {  delay = 0.506754016202314ms; datarate = 10Mbps;} <-- node37.out[2];
        node10.out[1] --> {  delay = 1.1860115207549817ms; datarate = 10Mbps;} --> node20.in[1];
        node10.in[1] <-- {  delay = 1.1860115207549817ms; datarate = 10Mbps;} <-- node20.out[1];
        node11.out[0] --> {  delay = 1.7374335194695276ms; datarate = 10Mbps;} --> node18.in[2];
        node11.in[0] <-- {  delay = 1.7374335194695276ms; datarate = 10Mbps;} <-- node18.out[2];
        node11.out[1] --> {  delay = 1.0069271888169768ms; datarate = 10Mbps;} --> node47.in[0];
        node11.in[1] <-- {  delay = 1.0069271888169768ms; datarate = 10Mbps;} <-- node47.out[0];
        node12.out[0] --> {  delay = 1.772917484924629ms; datarate = 10Mbps;} --> node29.in[0];
        node12.in[0] <-- {  delay = 1.772917484924629ms; datarate = 10Mbps;} <-- node29.out[0];
        node12.out[1] --> {  delay = 0.8021158862115902ms; datarate = 10Mbps;} --> node15.in[2];
        node12.in[1] <-- {  delay = 0.8021158862115902ms; datarate = 10Mbps;} <-- node15.out[2];
        node13.out[0] --> {  delay = 0.4058530698395389ms; datarate = 10Mbps;} --> node18.in[3];
        node13.in[0] <-- {  delay = 0.4058530698395389ms; datarate = 10Mbps;} <-- node18.out[3];
        node13.out[1] --> {  delay = 1.2088834061327276ms; datarate = 10Mbps;} --> node34.in[1];
        node13.in[1] <-- {  delay = 1.2088834061327276ms; datarate = 10Mbps;} <-- node34.out[1];
        node14.out[0] --> {  delay = 0.7206914545534654ms; datarate = 10Mbps;} --> node48.in[1];
        node14.in[0] <-- {  delay = 0.7206914545534654ms; datarate = 10Mbps;} <-- node48.out[1];
        node14.out[1] --> {  delay = 2.1925312861346122ms; datarate = 10Mbps;} --> node32.in[1];
        node14.in[1] <-- {  delay = 2.1925312861346122ms; datarate = 10Mbps;} <-- node32.out[1];
        node15.out[3] --> {  delay = 1.8214920958234782ms; datarate = 10Mbps;} --> node23.in[0];
        node15.in[3] <-- {  delay = 1.8214920958234782ms; datarate = 10Mbps;} <-- node23.out[0];
        node15.out[4] --> {  delay = 0.7458496152208752ms; datarate = 10Mbps;} --> node28.in[1];
        node15.in[4] <-- {  delay = 0.7458496152208752ms; datarate = 10Mbps;} <-- node28.out[1];
        node16.out[0] --> {  delay = 1.5782764141590022ms; datarate = 10Mbps;} --> node30.in[0];
        node16.in[0] <-- {  delay = 1.5782764141590022ms; datarate = 10Mbps;} <-- node30.out[0];
        node16.out[1] --> {  delay = 1.507628531007374ms; datarate = 10Mbps;} --> node15.in[5];
        node16.in[1] <-- {  delay = 1.507628531007374ms; datarate = 10Mbps;} <-- node15.out[5];
        node17.out[1] --> {  delay = 1.0062418583568036ms; datarate = 10Mbps;} --> node14.in[2];
        node17.in[1] <-- {  delay = 1.0062418583568036ms; datarate = 10Mbps;} <-- node14.out[2];
        node17.out[2] --> {  delay = 0.7341820752750475ms; datarate = 10Mbps;} --> node1.in[4];
        node17.in[2] <-- {  delay = 0.7341820752750475ms; datarate = 10Mbps;} <-- node1.out[4];
        node18.out[4] --> {  delay = 1.2367133433224793ms; datarate = 10Mbps;} --> node1.in[5];
        node18.in[4] <-- {  delay = 1.2367133433224793ms; datarate = 10Mbps;} <-- node1.out[5];
        node18.out[5] --> {  delay = 1.3532839636462164ms; datarate = 10Mbps;} --> node34.in[2];
        node18.in[5] <-- {  delay = 1.3532839636462164ms; datarate = 10Mbps;} <-- node34.out[2];
        node19.out[0] --> {  delay = 1.2684429879014412ms; datarate = 10Mbps;} --> node22.in[0];
        node19.in[0] <-- {  delay = 1.2684429879014412ms; datarate = 10Mbps;} <-- node22.out[0];
        node19.out[1] --> {  delay = 0.4949915846189381ms; datarate = 10Mbps;} --> node17.in[3];
        node19.in[1] <-- {  delay = 0.4949915846189381ms; datarate = 10Mbps;} <-- node17.out[3];
        node20.out[2] --> {  delay = 0.8758074535206031ms; datarate = 10Mbps;} --> node48.in[2];
        node20.in[2] <-- {  delay = 0.8758074535206031ms; datarate = 10Mbps;} <-- node48.out[2];
        node20.out[3] --> {  delay = 0.4044937462343325ms; datarate = 10Mbps;} --> node29.in[1];
        node20.in[3] <-- {  delay = 0.4044937462343325ms; datarate = 10Mbps;} <-- node29.out[1];
        node21.out[1] --> {  delay = 2.662044202019579ms; datarate = 10Mbps;} --> node35.in[0];
        node21.in[1] <-- {  delay = 2.662044202019579ms; datarate = 10Mbps;} <-- node35.out[0];
        node21.out[2] --> {  delay = 2.4864512938044716ms; datarate = 10Mbps;} --> node7.in[2];
        node21.in[2] <-- {  delay = 2.4864512938044716ms; datarate = 10Mbps;} <-- node7.out[2];
        node22.out[1] --> {  delay = 0.6906790752820215ms; datarate = 10Mbps;} --> node49.in[0];
        node22.in[1] <-- {  delay = 0.6906790752820215ms; datarate = 10Mbps;} <-- node49.out[0];
        node22.out[2] --> {  delay = 1.3543236285703022ms; datarate = 10Mbps;} --> node31.in[1];
        node22.in[2] <-- {  delay = 1.3543236285703022ms; datarate = 10Mbps;} <-- node31.out[1];
        node23.out[1] --> {  delay = 0.5819140125992485ms; datarate = 10Mbps;} --> node41.in[0];
        node23.in[1] <-- {  delay = 0.5819140125992485ms; datarate = 10Mbps;} <-- node41.out[0];
        node23.out[2] --> {  delay = 1.3227740642864145ms; datarate = 10Mbps;} --> node30.in[1];
        node23.in[2] <-- {  delay = 1.3227740642864145ms; datarate = 10Mbps;} <-- node30.out[1];
        node24.out[0] --> {  delay = 1.0755567224832667ms; datarate = 10Mbps;} --> node6.in[2];
        node24.in[0] <-- {  delay = 1.0755567224832667ms; datarate = 10Mbps;} <-- node6.out[2];
        node24.out[1] --> {  delay = 0.9809790553218829ms; datarate = 10Mbps;} --> node21.in[3];
        node24.in[1] <-- {  delay = 0.9809790553218829ms; datarate = 10Mbps;} <-- node21.out[3];
        node25.out[0] --> {  delay = 0.6800945730569309ms; datarate = 10Mbps;} --> node18.in[6];
        node25.in[0] <-- {  delay = 0.6800945730569309ms; datarate = 10Mbps;} <-- node18.out[6];
        node25.out[1] --> {  delay = 1.210474641955382ms; datarate = 10Mbps;} --> node23.in[3];
        node25.in[1] <-- {  delay = 1.210474641955382ms; datarate = 10Mbps;} <-- node23.out[3];
        node26.out[0] --> {  delay = 0.7993298244080661ms; datarate = 10Mbps;} --> node8.in[2];
        node26.in[0] <-- {  delay = 0.7993298244080661ms; datarate = 10Mbps;} <-- node8.out[2];
        node26.out[1] --> {  delay = 2.0938288811087857ms; datarate = 10Mbps;} --> node32.in[2];
        node26.in[1] <-- {  delay = 2.0938288811087857ms; datarate = 10Mbps;} <-- node32.out[2];
        node27.out[0] --> {  delay = 2.28990475886396ms; datarate = 10Mbps;} --> node29.in[2];
        node27.in[0] <-- {  delay = 2.28990475886396ms; datarate = 10Mbps;} <-- node29.out[2];
        node27.out[1] --> {  delay = 0.518228081089458ms; datarate = 10Mbps;} --> node12.in[2];
        node27.in[1] <-- {  delay = 0.518228081089458ms; datarate = 10Mbps;} <-- node12.out[2];
        node28.out[2] --> {  delay = 0.18753897800685296ms; datarate = 10Mbps;} --> node3.in[2];
        node28.in[2] <-- {  delay = 0.18753897800685296ms; datarate = 10Mbps;} <-- node3.out[2];
        node28.out[3] --> {  delay = 0.873638689019151ms; datarate = 10Mbps;} --> node44.in[1];
        node28.in[3] <-- {  delay = 0.873638689019151ms; datarate = 10Mbps;} <-- node44.out[1];
        node29.out[3] --> {  delay = 0.3392692858169552ms; datarate = 10Mbps;} --> node41.in[1];
        node29.in[3] <-- {  delay = 0.3392692858169552ms; datarate = 10Mbps;} <-- node41.out[1];
        node29.out[4] --> {  delay = 1.7740655871631774ms; datarate = 10Mbps;} --> node1.in[6];
        node29.in[4] <-- {  delay = 1.7740655871631774ms; datarate = 10Mbps;} <-- node1.out[6];
        node30.out[2] --> {  delay = 1.256952544885628ms; datarate = 10Mbps;} --> node11.in[2];
        node30.in[2] <-- {  delay = 1.256952544885628ms; datarate = 10Mbps;} <-- node11.out[2];
        node30.out[3] --> {  delay = 1.256899431970486ms; datarate = 10Mbps;} --> node4.in[2];
        node30.in[3] <-- {  delay = 1.256899431970486ms; datarate = 10Mbps;} <-- node4.out[2];
        node31.out[2] --> {  delay = 0.7731854555131314ms; datarate = 10Mbps;} --> node15.in[6];
        node31.in[2] <-- {  delay = 0.7731854555131314ms; datarate = 10Mbps;} <-- node15.out[6];
        node31.out[3] --> {  delay = 0.6004616979160079ms; datarate = 10Mbps;} --> node30.in[4];
        node31.in[3] <-- {  delay = 0.6004616979160079ms; datarate = 10Mbps;} <-- node30.out[4];
        node32.out[3] --> {  delay = 0.7045075847742153ms; datarate = 10Mbps;} --> node15.in[7];
        node32.in[3] <-- {  delay = 0.7045075847742153ms; datarate = 10Mbps;} <-- node15.out[7];
        node32.out[4] --> {  delay = 2.8824454484984074ms; datarate = 10Mbps;} --> node48.in[3];
        node32.in[4] <-- {  delay = 2.8824454484984074ms; datarate = 10Mbps;} <-- node48.out[3];
        node33.out[0] --> {  delay = 0.10616564918947727ms; datarate = 10Mbps;} --> node8.in[3];
        node33.in[0] <-- {  delay = 0.10616564918947727ms; datarate = 10Mbps;} <-- node8.out[3];
        node33.out[1] --> {  delay = 0.05274111432396974ms; datarate = 10Mbps;} --> node44.in[2];
        node33.in[1] <-- {  delay = 0.05274111432396974ms; datarate = 10Mbps;} <-- node44.out[2];
        node34.out[3] --> {  delay = 0.5848892085123862ms; datarate = 10Mbps;} --> node16.in[2];
        node34.in[3] <-- {  delay = 0.5848892085123862ms; datarate = 10Mbps;} <-- node16.out[2];
        node34.out[4] --> {  delay = 1.7631000330674944ms; datarate = 10Mbps;} --> node15.in[8];
        node34.in[4] <-- {  delay = 1.7631000330674944ms; datarate = 10Mbps;} <-- node15.out[8];
        node35.out[1] --> {  delay = 0.5851174431237701ms; datarate = 10Mbps;} --> node13.in[2];
        node35.in[1] <-- {  delay = 0.5851174431237701ms; datarate = 10Mbps;} <-- node13.out[2];
        node35.out[2] --> {  delay = 0.9854659922720415ms; datarate = 10Mbps;} --> node18.in[7];
        node35.in[2] <-- {  delay = 0.9854659922720415ms; datarate = 10Mbps;} <-- node18.out[7];
        node36.out[0] --> {  delay = 0.4255524916191737ms; datarate = 10Mbps;} --> node19.in[2];
        node36.in[0] <-- {  delay = 0.4255524916191737ms; datarate = 10Mbps;} <-- node19.out[2];
        node36.out[1] --> {  delay = 1.0958887797140524ms; datarate = 10Mbps;} --> node4.in[3];
        node36.in[1] <-- {  delay = 1.0958887797140524ms; datarate = 10Mbps;} <-- node4.out[3];
        node37.out[3] --> {  delay = 1.275004848773972ms; datarate = 10Mbps;} --> node34.in[5];
        node37.in[3] <-- {  delay = 1.275004848773972ms; datarate = 10Mbps;} <-- node34.out[5];
        node37.out[4] --> {  delay = 0.6797018142351714ms; datarate = 10Mbps;} --> node31.in[4];
        node37.in[4] <-- {  delay = 0.6797018142351714ms; datarate = 10Mbps;} <-- node31.out[4];
        node38.out[0] --> {  delay = 1.0920798819607076ms; datarate = 10Mbps;} --> node13.in[3];
        node38.in[0] <-- {  delay = 1.0920798819607076ms; datarate = 10Mbps;} <-- node13.out[3];
        node38.out[1] --> {  delay = 0.1864679979290698ms; datarate = 10Mbps;} --> node34.in[6];
        node38.in[1] <-- {  delay = 0.1864679979290698ms; datarate = 10Mbps;} <-- node34.out[6];
        node39.out[0] --> {  delay = 1.801607438932781ms; datarate = 10Mbps;} --> node17.in[4];
        node39.in[0] <-- {  delay = 1.801607438932781ms; datarate = 10Mbps;} <-- node17.out[4];
        node39.out[1] --> {  delay = 0.8474891528413397ms; datarate = 10Mbps;} --> node26.in[2];
        node39.in[1] <-- {  delay = 0.8474891528413397ms; datarate = 10Mbps;} <-- node26.out[2];
        node40.out[0] --> {  delay = 1.754651772747347ms; datarate = 10Mbps;} --> node17.in[5];
        node40.in[0] <-- {  delay = 1.754651772747347ms; datarate = 10Mbps;} <-- node17.out[5];
        node40.out[1] --> {  delay = 1.53330674310175ms; datarate = 10Mbps;} --> node2.in[3];
        node40.in[1] <-- {  delay = 1.53330674310175ms; datarate = 10Mbps;} <-- node2.out[3];
        node41.out[2] --> {  delay = 0.6979704152344255ms; datarate = 10Mbps;} --> node48.in[4];
        node41.in[2] <-- {  delay = 0.6979704152344255ms; datarate = 10Mbps;} <-- node48.out[4];
        node41.out[3] --> {  delay = 2.392494140717557ms; datarate = 10Mbps;} --> node34.in[7];
        node41.in[3] <-- {  delay = 2.392494140717557ms; datarate = 10Mbps;} <-- node34.out[7];
        node42.out[0] --> {  delay = 0.4125016037208581ms; datarate = 10Mbps;} --> node49.in[1];
        node42.in[0] <-- {  delay = 0.4125016037208581ms; datarate = 10Mbps;} <-- node49.out[1];
        node42.out[1] --> {  delay = 0.6809039226329823ms; datarate = 10Mbps;} --> node22.in[3];
        node42.in[1] <-- {  delay = 0.6809039226329823ms; datarate = 10Mbps;} <-- node22.out[3];
        node43.out[1] --> {  delay = 1.8429469086515091ms; datarate = 10Mbps;} --> node15.in[9];
        node43.in[1] <-- {  delay = 1.8429469086515091ms; datarate = 10Mbps;} <-- node15.out[9];
        node43.out[2] --> {  delay = 1.4161043213783926ms; datarate = 10Mbps;} --> node30.in[5];
        node43.in[2] <-- {  delay = 1.4161043213783926ms; datarate = 10Mbps;} <-- node30.out[5];
        node44.out[3] --> {  delay = 1.8652169298149892ms; datarate = 10Mbps;} --> node18.in[8];
        node44.in[3] <-- {  delay = 1.8652169298149892ms; datarate = 10Mbps;} <-- node18.out[8];
        node44.out[4] --> {  delay = 1.6388625556495187ms; datarate = 10Mbps;} --> node48.in[5];
        node44.in[4] <-- {  delay = 1.6388625556495187ms; datarate = 10Mbps;} <-- node48.out[5];
        node45.out[0] --> {  delay = 0.9606240560121269ms; datarate = 10Mbps;} --> node3.in[3];
        node45.in[0] <-- {  delay = 0.9606240560121269ms; datarate = 10Mbps;} <-- node3.out[3];
        node45.out[1] --> {  delay = 0.14401318357997453ms; datarate = 10Mbps;} --> node8.in[4];
        node45.in[1] <-- {  delay = 0.14401318357997453ms; datarate = 10Mbps;} <-- node8.out[4];
        node46.out[0] --> {  delay = 1.447322270446325ms; datarate = 10Mbps;} --> node21.in[4];
        node46.in[0] <-- {  delay = 1.447322270446325ms; datarate = 10Mbps;} <-- node21.out[4];
        node46.out[1] --> {  delay = 1.4307372460117214ms; datarate = 10Mbps;} --> node1.in[7];
        node46.in[1] <-- {  delay = 1.4307372460117214ms; datarate = 10Mbps;} <-- node1.out[7];
        node47.out[1] --> {  delay = 1.36961271259503ms; datarate = 10Mbps;} --> node30.in[6];
        node47.in[1] <-- {  delay = 1.36961271259503ms; datarate = 10Mbps;} <-- node30.out[6];
        node47.out[2] --> {  delay = 0.49296439906211353ms; datarate = 10Mbps;} --> node29.in[5];
        node47.in[2] <-- {  delay = 0.49296439906211353ms; datarate = 10Mbps;} <-- node29.out[5];
        node48.out[6] --> {  delay = 1.3516550566028456ms; datarate = 10Mbps;} --> node30.in[7];
        node48.in[6] <-- {  delay = 1.3516550566028456ms; datarate = 10Mbps;} <-- node30.out[7];
        node48.out[7] --> {  delay = 1.845984207454824ms; datarate = 10Mbps;} --> node31.in[5];
        node48.in[7] <-- {  delay = 1.845984207454824ms; datarate = 10Mbps;} <-- node31.out[5];
        node49.out[2] --> {  delay = 1.3128448864319113ms; datarate = 10Mbps;} --> node23.in[4];
        node49.in[2] <-- {  delay = 1.3128448864319113ms; datarate = 10Mbps;} <-- node23.out[4];
        node49.out[3] --> {  delay = 0.08033287200835651ms; datarate = 10Mbps;} --> node48.in[8];
        node49.in[3] <-- {  delay = 0.08033287200835651ms; datarate = 10Mbps;} <-- node48.out[8];
}

