//
// OMNeT++ network description -- exported from BRITE
// Generator Model Used:
//   Model (3 - ASWaxman):  100 1000 100 1  2  0.15000000596046448 0.20000000298023224 1 1 10.0 1024.0
//


//
// Prototype for modules that can be used as node in a Brite100Network
//
module Brite100Node
{
    gates:
        input in[];
        output out[];
    connections allowunconnected:
}

//
// Topology exported from BRITE.
//
network Brite100 extends Base
{
    submodules:
        node0: Brite100Node {
            parameters:
                @display("i=router_s;ppp=936,643");
            gates:
                in[2];
                out[2];
        };
        node1: Brite100Node {
            parameters:
                @display("i=router_s;ppp=107,674");
            gates:
                in[8];
                out[8];
        };
        node2: Brite100Node {
            parameters:
                @display("i=router_s;ppp=280,427");
            gates:
                in[5];
                out[5];
        };
        node3: Brite100Node {
            parameters:
                @display("i=router_s;ppp=538,968");
            gates:
                in[5];
                out[5];
        };
        node4: Brite100Node {
            parameters:
                @display("i=router_s;ppp=470,805");
            gates:
                in[6];
                out[6];
        };
        node5: Brite100Node {
            parameters:
                @display("i=router_s;ppp=670,889");
            gates:
                in[5];
                out[5];
        };
        node6: Brite100Node {
            parameters:
                @display("i=router_s;ppp=159,358");
            gates:
                in[4];
                out[4];
        };
        node7: Brite100Node {
            parameters:
                @display("i=router_s;ppp=888,324");
            gates:
                in[3];
                out[3];
        };
        node8: Brite100Node {
            parameters:
                @display("i=router_s;ppp=629,170");
            gates:
                in[2];
                out[2];
        };
        node9: Brite100Node {
            parameters:
                @display("i=router_s;ppp=909,367");
            gates:
                in[2];
                out[2];
        };
        node10: Brite100Node {
            parameters:
                @display("i=router_s;ppp=568,489");
            gates:
                in[2];
                out[2];
        };
        node11: Brite100Node {
            parameters:
                @display("i=router_s;ppp=825,346");
            gates:
                in[2];
                out[2];
        };
        node12: Brite100Node {
            parameters:
                @display("i=router_s;ppp=382,464");
            gates:
                in[3];
                out[3];
        };
        node13: Brite100Node {
            parameters:
                @display("i=router_s;ppp=63,617");
            gates:
                in[3];
                out[3];
        };
        node14: Brite100Node {
            parameters:
                @display("i=router_s;ppp=418,949");
            gates:
                in[4];
                out[4];
        };
        node15: Brite100Node {
            parameters:
                @display("i=router_s;ppp=721,996");
            gates:
                in[4];
                out[4];
        };
        node16: Brite100Node {
            parameters:
                @display("i=router_s;ppp=167,772");
            gates:
                in[4];
                out[4];
        };
        node17: Brite100Node {
            parameters:
                @display("i=router_s;ppp=117,953");
            gates:
                in[4];
                out[4];
        };
        node18: Brite100Node {
            parameters:
                @display("i=router_s;ppp=847,103");
            gates:
                in[4];
                out[4];
        };
        node19: Brite100Node {
            parameters:
                @display("i=router_s;ppp=943,249");
            gates:
                in[2];
                out[2];
        };
        node20: Brite100Node {
            parameters:
                @display("i=router_s;ppp=785,815");
            gates:
                in[2];
                out[2];
        };
        node21: Brite100Node {
            parameters:
                @display("i=router_s;ppp=103,778");
            gates:
                in[3];
                out[3];
        };
        node22: Brite100Node {
            parameters:
                @display("i=router_s;ppp=638,763");
            gates:
                in[3];
                out[3];
        };
        node23: Brite100Node {
            parameters:
                @display("i=router_s;ppp=189,739");
            gates:
                in[3];
                out[3];
        };
        node24: Brite100Node {
            parameters:
                @display("i=router_s;ppp=836,964");
            gates:
                in[2];
                out[2];
        };
        node25: Brite100Node {
            parameters:
                @display("i=router_s;ppp=538,161");
            gates:
                in[2];
                out[2];
        };
        node26: Brite100Node {
            parameters:
                @display("i=router_s;ppp=571,541");
            gates:
                in[3];
                out[3];
        };
        node27: Brite100Node {
            parameters:
                @display("i=router_s;ppp=5,985");
            gates:
                in[4];
                out[4];
        };
        node28: Brite100Node {
            parameters:
                @display("i=router_s;ppp=290,117");
            gates:
                in[4];
                out[4];
        };
        node29: Brite100Node {
            parameters:
                @display("i=router_s;ppp=896,601");
            gates:
                in[6];
                out[6];
        };
        node30: Brite100Node {
            parameters:
                @display("i=router_s;ppp=924,83");
            gates:
                in[16];
                out[16];
        };
        node31: Brite100Node {
            parameters:
                @display("i=router_s;ppp=997,751");
            gates:
                in[8];
                out[8];
        };
        node32: Brite100Node {
            parameters:
                @display("i=router_s;ppp=25,780");
            gates:
                in[7];
                out[7];
        };
        node33: Brite100Node {
            parameters:
                @display("i=router_s;ppp=263,188");
            gates:
                in[3];
                out[3];
        };
        node34: Brite100Node {
            parameters:
                @display("i=router_s;ppp=562,165");
            gates:
                in[9];
                out[9];
        };
        node35: Brite100Node {
            parameters:
                @display("i=router_s;ppp=528,349");
            gates:
                in[5];
                out[5];
        };
        node36: Brite100Node {
            parameters:
                @display("i=router_s;ppp=400,996");
            gates:
                in[4];
                out[4];
        };
        node37: Brite100Node {
            parameters:
                @display("i=router_s;ppp=564,553");
            gates:
                in[3];
                out[3];
        };
        node38: Brite100Node {
            parameters:
                @display("i=router_s;ppp=338,902");
            gates:
                in[5];
                out[5];
        };
        node39: Brite100Node {
            parameters:
                @display("i=router_s;ppp=888,921");
            gates:
                in[2];
                out[2];
        };
        node40: Brite100Node {
            parameters:
                @display("i=router_s;ppp=219,957");
            gates:
                in[2];
                out[2];
        };
        node41: Brite100Node {
            parameters:
                @display("i=router_s;ppp=12,309");
            gates:
                in[2];
                out[2];
        };
        node42: Brite100Node {
            parameters:
                @display("i=router_s;ppp=754,553");
            gates:
                in[2];
                out[2];
        };
        node43: Brite100Node {
            parameters:
                @display("i=router_s;ppp=236,9");
            gates:
                in[6];
                out[6];
        };
        node44: Brite100Node {
            parameters:
                @display("i=router_s;ppp=648,426");
            gates:
                in[2];
                out[2];
        };
        node45: Brite100Node {
            parameters:
                @display("i=router_s;ppp=659,896");
            gates:
                in[4];
                out[4];
        };
        node46: Brite100Node {
            parameters:
                @display("i=router_s;ppp=94,653");
            gates:
                in[3];
                out[3];
        };
        node47: Brite100Node {
            parameters:
                @display("i=router_s;ppp=814,972");
            gates:
                in[2];
                out[2];
        };
        node48: Brite100Node {
            parameters:
                @display("i=router_s;ppp=920,336");
            gates:
                in[4];
                out[4];
        };
        node49: Brite100Node {
            parameters:
                @display("i=router_s;ppp=222,4");
            gates:
                in[2];
                out[2];
        };
        node50: Brite100Node {
            parameters:
                @display("i=router_s;ppp=481,469");
            gates:
                in[2];
                out[2];
        };
        node51: Brite100Node {
            parameters:
                @display("i=router_s;ppp=214,26");
            gates:
                in[3];
                out[3];
        };
        node52: Brite100Node {
            parameters:
                @display("i=router_s;ppp=923,164");
            gates:
                in[2];
                out[2];
        };
        node53: Brite100Node {
            parameters:
                @display("i=router_s;ppp=546,15");
            gates:
                in[3];
                out[3];
        };
        node54: Brite100Node {
            parameters:
                @display("i=router_s;ppp=568,24");
            gates:
                in[6];
                out[6];
        };
        node55: Brite100Node {
            parameters:
                @display("i=router_s;ppp=713,993");
            gates:
                in[3];
                out[3];
        };
        node56: Brite100Node {
            parameters:
                @display("i=router_s;ppp=988,962");
            gates:
                in[2];
                out[2];
        };
        node57: Brite100Node {
            parameters:
                @display("i=router_s;ppp=371,111");
            gates:
                in[3];
                out[3];
        };
        node58: Brite100Node {
            parameters:
                @display("i=router_s;ppp=820,152");
            gates:
                in[5];
                out[5];
        };
        node59: Brite100Node {
            parameters:
                @display("i=router_s;ppp=939,22");
            gates:
                in[7];
                out[7];
        };
        node60: Brite100Node {
            parameters:
                @display("i=router_s;ppp=463,143");
            gates:
                in[5];
                out[5];
        };
        node61: Brite100Node {
            parameters:
                @display("i=router_s;ppp=290,12");
            gates:
                in[3];
                out[3];
        };
        node62: Brite100Node {
            parameters:
                @display("i=router_s;ppp=598,360");
            gates:
                in[16];
                out[16];
        };
        node63: Brite100Node {
            parameters:
                @display("i=router_s;ppp=406,868");
            gates:
                in[6];
                out[6];
        };
        node64: Brite100Node {
            parameters:
                @display("i=router_s;ppp=914,394");
            gates:
                in[5];
                out[5];
        };
        node65: Brite100Node {
            parameters:
                @display("i=router_s;ppp=38,485");
            gates:
                in[2];
                out[2];
        };
        node66: Brite100Node {
            parameters:
                @display("i=router_s;ppp=486,447");
            gates:
                in[3];
                out[3];
        };
        node67: Brite100Node {
            parameters:
                @display("i=router_s;ppp=663,498");
            gates:
                in[6];
                out[6];
        };
        node68: Brite100Node {
            parameters:
                @display("i=router_s;ppp=23,325");
            gates:
                in[3];
                out[3];
        };
        node69: Brite100Node {
            parameters:
                @display("i=router_s;ppp=521,71");
            gates:
                in[2];
                out[2];
        };
        node70: Brite100Node {
            parameters:
                @display("i=router_s;ppp=595,145");
            gates:
                in[3];
                out[3];
        };
        node71: Brite100Node {
            parameters:
                @display("i=router_s;ppp=100,101");
            gates:
                in[2];
                out[2];
        };
        node72: Brite100Node {
            parameters:
                @display("i=router_s;ppp=518,723");
            gates:
                in[4];
                out[4];
        };
        node73: Brite100Node {
            parameters:
                @display("i=router_s;ppp=206,696");
            gates:
                in[4];
                out[4];
        };
        node74: Brite100Node {
            parameters:
                @display("i=router_s;ppp=772,262");
            gates:
                in[4];
                out[4];
        };
        node75: Brite100Node {
            parameters:
                @display("i=router_s;ppp=663,787");
            gates:
                in[3];
                out[3];
        };
        node76: Brite100Node {
            parameters:
                @display("i=router_s;ppp=726,512");
            gates:
                in[2];
                out[2];
        };
        node77: Brite100Node {
            parameters:
                @display("i=router_s;ppp=793,96");
            gates:
                in[2];
                out[2];
        };
        node78: Brite100Node {
            parameters:
                @display("i=router_s;ppp=945,60");
            gates:
                in[2];
                out[2];
        };
        node79: Brite100Node {
            parameters:
                @display("i=router_s;ppp=9,530");
            gates:
                in[2];
                out[2];
        };
        node80: Brite100Node {
            parameters:
                @display("i=router_s;ppp=567,798");
            gates:
                in[3];
                out[3];
        };
        node81: Brite100Node {
            parameters:
                @display("i=router_s;ppp=297,229");
            gates:
                in[5];
                out[5];
        };
        node82: Brite100Node {
            parameters:
                @display("i=router_s;ppp=45,863");
            gates:
                in[5];
                out[5];
        };
        node83: Brite100Node {
            parameters:
                @display("i=router_s;ppp=860,158");
            gates:
                in[6];
                out[6];
        };
        node84: Brite100Node {
            parameters:
                @display("i=router_s;ppp=84,602");
            gates:
                in[2];
                out[2];
        };
        node85: Brite100Node {
            parameters:
                @display("i=router_s;ppp=931,108");
            gates:
                in[5];
                out[5];
        };
        node86: Brite100Node {
            parameters:
                @display("i=router_s;ppp=219,133");
            gates:
                in[6];
                out[6];
        };
        node87: Brite100Node {
            parameters:
                @display("i=router_s;ppp=588,441");
            gates:
                in[4];
                out[4];
        };
        node88: Brite100Node {
            parameters:
                @display("i=router_s;ppp=827,150");
            gates:
                in[3];
                out[3];
        };
        node89: Brite100Node {
            parameters:
                @display("i=router_s;ppp=454,713");
            gates:
                in[2];
                out[2];
        };
        node90: Brite100Node {
            parameters:
                @display("i=router_s;ppp=292,494");
            gates:
                in[2];
                out[2];
        };
        node91: Brite100Node {
            parameters:
                @display("i=router_s;ppp=291,567");
            gates:
                in[3];
                out[3];
        };
        node92: Brite100Node {
            parameters:
                @display("i=router_s;ppp=695,822");
            gates:
                in[2];
                out[2];
        };
        node93: Brite100Node {
            parameters:
                @display("i=router_s;ppp=859,927");
            gates:
                in[6];
                out[6];
        };
        node94: Brite100Node {
            parameters:
                @display("i=router_s;ppp=678,461");
            gates:
                in[5];
                out[5];
        };
        node95: Brite100Node {
            parameters:
                @display("i=router_s;ppp=878,577");
            gates:
                in[11];
                out[11];
        };
        node96: Brite100Node {
            parameters:
                @display("i=router_s;ppp=36,140");
            gates:
                in[8];
                out[8];
        };
        node97: Brite100Node {
            parameters:
                @display("i=router_s;ppp=553,664");
            gates:
                in[4];
                out[4];
        };
        node98: Brite100Node {
            parameters:
                @display("i=router_s;ppp=212,833");
            gates:
                in[3];
                out[3];
        };
        node99: Brite100Node {
            parameters:
                @display("i=router_s;ppp=950,540");
            gates:
                in[3];
                out[3];
        };
    connections:
        node0.out[0] --> {  delay = 2.1663723278639626ms; datarate = 10Mbps;} --> node91.in[0];
        node0.in[0] <-- {  delay = 2.1663723278639626ms; datarate = 10Mbps;} <-- node91.out[0];
        node0.out[1] --> {  delay = 1.8255466508397855ms; datarate = 10Mbps;} --> node18.in[0];
        node0.in[1] <-- {  delay = 1.8255466508397855ms; datarate = 10Mbps;} <-- node18.out[0];
        node1.out[0] --> {  delay = 1.9440721002322032ms; datarate = 10Mbps;} --> node62.in[0];
        node1.in[0] <-- {  delay = 1.9440721002322032ms; datarate = 10Mbps;} <-- node62.out[0];
        node1.out[1] --> {  delay = 0.447025670479034ms; datarate = 10Mbps;} --> node32.in[0];
        node1.in[1] <-- {  delay = 0.447025670479034ms; datarate = 10Mbps;} <-- node32.out[0];
        node2.out[0] --> {  delay = 1.0058934873902103ms; datarate = 10Mbps;} --> node1.in[2];
        node2.in[0] <-- {  delay = 1.0058934873902103ms; datarate = 10Mbps;} <-- node1.out[2];
        node2.out[1] --> {  delay = 1.0840217265811178ms; datarate = 10Mbps;} --> node62.in[1];
        node2.in[1] <-- {  delay = 1.0840217265811178ms; datarate = 10Mbps;} <-- node62.out[1];
        node3.out[0] --> {  delay = 1.8224722389730212ms; datarate = 10Mbps;} --> node32.in[1];
        node3.in[0] <-- {  delay = 1.8224722389730212ms; datarate = 10Mbps;} <-- node32.out[1];
        node3.out[1] --> {  delay = 1.7101561483790353ms; datarate = 10Mbps;} --> node29.in[0];
        node3.in[1] <-- {  delay = 1.7101561483790353ms; datarate = 10Mbps;} <-- node29.out[0];
        node4.out[0] --> {  delay = 1.5445458422086418ms; datarate = 10Mbps;} --> node62.in[2];
        node4.in[0] <-- {  delay = 1.5445458422086418ms; datarate = 10Mbps;} <-- node62.out[2];
        node4.out[1] --> {  delay = 1.430787794060493ms; datarate = 10Mbps;} --> node82.in[0];
        node4.in[1] <-- {  delay = 1.430787794060493ms; datarate = 10Mbps;} <-- node82.out[0];
        node5.out[0] --> {  delay = 1.5191931273774575ms; datarate = 10Mbps;} --> node87.in[0];
        node5.in[0] <-- {  delay = 1.5191931273774575ms; datarate = 10Mbps;} <-- node87.out[0];
        node5.out[1] --> {  delay = 2.0167363538401935ms; datarate = 10Mbps;} --> node2.in[2];
        node5.in[1] <-- {  delay = 2.0167363538401935ms; datarate = 10Mbps;} <-- node2.out[2];
        node6.out[0] --> {  delay = 1.9102974308580787ms; datarate = 10Mbps;} --> node38.in[0];
        node6.in[0] <-- {  delay = 1.9102974308580787ms; datarate = 10Mbps;} <-- node38.out[0];
        node6.out[1] --> {  delay = 0.834930320217965ms; datarate = 10Mbps;} --> node96.in[0];
        node6.in[1] <-- {  delay = 0.834930320217965ms; datarate = 10Mbps;} <-- node96.out[0];
        node7.out[0] --> {  delay = 0.844576121161032ms; datarate = 10Mbps;} --> node95.in[0];
        node7.in[0] <-- {  delay = 0.844576121161032ms; datarate = 10Mbps;} <-- node95.out[0];
        node7.out[1] --> {  delay = 1.209863229560457ms; datarate = 10Mbps;} --> node34.in[0];
        node7.in[1] <-- {  delay = 1.209863229560457ms; datarate = 10Mbps;} <-- node34.out[0];
        node8.out[0] --> {  delay = 0.5278012831872848ms; datarate = 10Mbps;} --> node54.in[0];
        node8.in[0] <-- {  delay = 0.5278012831872848ms; datarate = 10Mbps;} <-- node54.out[0];
        node8.out[1] --> {  delay = 1.1458497162130434ms; datarate = 10Mbps;} --> node59.in[0];
        node8.in[1] <-- {  delay = 1.1458497162130434ms; datarate = 10Mbps;} <-- node59.out[0];
        node9.out[0] --> {  delay = 0.8318863836869006ms; datarate = 10Mbps;} --> node94.in[0];
        node9.in[0] <-- {  delay = 0.8318863836869006ms; datarate = 10Mbps;} <-- node94.out[0];
        node9.out[1] --> {  delay = 0.7160526508788502ms; datarate = 10Mbps;} --> node83.in[0];
        node9.in[1] <-- {  delay = 0.7160526508788502ms; datarate = 10Mbps;} <-- node83.out[0];
        node10.out[0] --> {  delay = 0.4417804277379943ms; datarate = 10Mbps;} --> node62.in[3];
        node10.in[0] <-- {  delay = 0.4417804277379943ms; datarate = 10Mbps;} <-- node62.out[3];
        node10.out[1] --> {  delay = 0.2138975696564241ms; datarate = 10Mbps;} --> node37.in[0];
        node10.in[1] <-- {  delay = 0.2138975696564241ms; datarate = 10Mbps;} <-- node37.out[0];
        node11.out[0] --> {  delay = 0.7905539428054973ms; datarate = 10Mbps;} --> node95.in[1];
        node11.in[0] <-- {  delay = 0.7905539428054973ms; datarate = 10Mbps;} <-- node95.out[1];
        node11.out[1] --> {  delay = 1.0188786133777596ms; datarate = 10Mbps;} --> node70.in[0];
        node11.in[1] <-- {  delay = 1.0188786133777596ms; datarate = 10Mbps;} <-- node70.out[0];
        node12.out[0] --> {  delay = 0.6914116694006814ms; datarate = 10Mbps;} --> node87.in[1];
        node12.in[0] <-- {  delay = 0.6914116694006814ms; datarate = 10Mbps;} <-- node87.out[1];
        node12.out[1] --> {  delay = 0.6199358481822779ms; datarate = 10Mbps;} --> node35.in[0];
        node12.in[1] <-- {  delay = 0.6199358481822779ms; datarate = 10Mbps;} <-- node35.out[0];
        node13.out[0] --> {  delay = 1.2426684029924058ms; datarate = 10Mbps;} --> node27.in[0];
        node13.in[0] <-- {  delay = 1.2426684029924058ms; datarate = 10Mbps;} <-- node27.out[0];
        node13.out[1] --> {  delay = 0.5449461440919743ms; datarate = 10Mbps;} --> node73.in[0];
        node13.in[1] <-- {  delay = 0.5449461440919743ms; datarate = 10Mbps;} <-- node73.out[0];
        node14.out[0] --> {  delay = 1.801023724277943ms; datarate = 10Mbps;} --> node2.in[3];
        node14.in[0] <-- {  delay = 1.801023724277943ms; datarate = 10Mbps;} <-- node2.out[3];
        node14.out[1] --> {  delay = 0.16787920037883836ms; datarate = 10Mbps;} --> node36.in[0];
        node14.in[1] <-- {  delay = 0.16787920037883836ms; datarate = 10Mbps;} <-- node36.out[0];
        node15.out[0] --> {  delay = 0.39247327776558616ms; datarate = 10Mbps;} --> node45.in[0];
        node15.in[0] <-- {  delay = 0.39247327776558616ms; datarate = 10Mbps;} <-- node45.out[0];
        node15.out[1] --> {  delay = 1.2310322915527394ms; datarate = 10Mbps;} --> node31.in[0];
        node15.in[1] <-- {  delay = 1.2310322915527394ms; datarate = 10Mbps;} <-- node31.out[0];
        node16.out[0] --> {  delay = 2.5554797933060445ms; datarate = 10Mbps;} --> node43.in[0];
        node16.in[0] <-- {  delay = 2.5554797933060445ms; datarate = 10Mbps;} <-- node43.out[0];
        node16.out[1] --> {  delay = 1.0166757836560731ms; datarate = 10Mbps;} --> node4.in[2];
        node16.in[1] <-- {  delay = 1.0166757836560731ms; datarate = 10Mbps;} <-- node4.out[2];
        node17.out[0] --> {  delay = 1.817887595806334ms; datarate = 10Mbps;} --> node45.in[1];
        node17.in[0] <-- {  delay = 1.817887595806334ms; datarate = 10Mbps;} <-- node45.out[1];
        node17.out[1] --> {  delay = 3.4446723372157373ms; datarate = 10Mbps;} --> node54.in[1];
        node17.in[1] <-- {  delay = 3.4446723372157373ms; datarate = 10Mbps;} <-- node54.out[1];
        node18.out[1] --> {  delay = 0.17037910659009486ms; datarate = 10Mbps;} --> node88.in[0];
        node18.in[1] <-- {  delay = 0.17037910659009486ms; datarate = 10Mbps;} <-- node88.out[0];
        node18.out[2] --> {  delay = 0.1885153648654469ms; datarate = 10Mbps;} --> node83.in[1];
        node18.in[2] <-- {  delay = 0.1885153648654469ms; datarate = 10Mbps;} <-- node83.out[1];
        node19.out[0] --> {  delay = 0.557331603400066ms; datarate = 10Mbps;} --> node30.in[0];
        node19.in[0] <-- {  delay = 0.557331603400066ms; datarate = 10Mbps;} <-- node30.out[0];
        node19.out[1] --> {  delay = 0.410839401108674ms; datarate = 10Mbps;} --> node83.in[2];
        node19.in[1] <-- {  delay = 0.410839401108674ms; datarate = 10Mbps;} <-- node83.out[2];
        node20.out[0] --> {  delay = 0.8523394869998041ms; datarate = 10Mbps;} --> node95.in[2];
        node20.in[0] <-- {  delay = 0.8523394869998041ms; datarate = 10Mbps;} <-- node95.out[2];
        node20.out[1] --> {  delay = 2.2752592517775967ms; datarate = 10Mbps;} --> node17.in[2];
        node20.in[1] <-- {  delay = 2.2752592517775967ms; datarate = 10Mbps;} <-- node17.out[2];
        node21.out[0] --> {  delay = 0.34716315194981157ms; datarate = 10Mbps;} --> node1.in[3];
        node21.in[0] <-- {  delay = 0.34716315194981157ms; datarate = 10Mbps;} <-- node1.out[3];
        node21.out[1] --> {  delay = 0.4391533151597215ms; datarate = 10Mbps;} --> node73.in[1];
        node21.in[1] <-- {  delay = 0.4391533151597215ms; datarate = 10Mbps;} <-- node73.out[1];
        node22.out[0] --> {  delay = 0.4352468248519328ms; datarate = 10Mbps;} --> node97.in[0];
        node22.in[0] <-- {  delay = 0.4352468248519328ms; datarate = 10Mbps;} <-- node97.out[0];
        node22.out[1] --> {  delay = 0.4219289145917579ms; datarate = 10Mbps;} --> node72.in[0];
        node22.in[1] <-- {  delay = 0.4219289145917579ms; datarate = 10Mbps;} <-- node72.out[0];
        node23.out[0] --> {  delay = 0.5638811989403819ms; datarate = 10Mbps;} --> node32.in[2];
        node23.in[0] <-- {  delay = 0.5638811989403819ms; datarate = 10Mbps;} <-- node32.out[2];
        node23.out[1] --> {  delay = 0.6338771081543194ms; datarate = 10Mbps;} --> node82.in[1];
        node23.in[1] <-- {  delay = 0.6338771081543194ms; datarate = 10Mbps;} <-- node82.out[1];
        node24.out[0] --> {  delay = 2.294302731481507ms; datarate = 10Mbps;} --> node35.in[1];
        node24.in[0] <-- {  delay = 2.294302731481507ms; datarate = 10Mbps;} <-- node35.out[1];
        node24.out[1] --> {  delay = 0.14532067321581493ms; datarate = 10Mbps;} --> node93.in[0];
        node24.in[1] <-- {  delay = 0.14532067321581493ms; datarate = 10Mbps;} <-- node93.out[0];
        node25.out[0] --> {  delay = 1.2878727856657926ms; datarate = 10Mbps;} --> node7.in[2];
        node25.in[0] <-- {  delay = 1.2878727856657926ms; datarate = 10Mbps;} <-- node7.out[2];
        node25.out[1] --> {  delay = 1.6759562744906433ms; datarate = 10Mbps;} --> node96.in[1];
        node25.in[1] <-- {  delay = 1.6759562744906433ms; datarate = 10Mbps;} <-- node96.out[1];
        node26.out[0] --> {  delay = 0.9428541855903062ms; datarate = 10Mbps;} --> node4.in[3];
        node26.in[0] <-- {  delay = 0.9428541855903062ms; datarate = 10Mbps;} <-- node4.out[3];
        node26.out[1] --> {  delay = 1.0310584348408338ms; datarate = 10Mbps;} --> node95.in[3];
        node26.in[1] <-- {  delay = 1.0310584348408338ms; datarate = 10Mbps;} <-- node95.out[3];
        node27.out[1] --> {  delay = 1.7788007129535783ms; datarate = 10Mbps;} --> node3.in[2];
        node27.in[1] <-- {  delay = 1.7788007129535783ms; datarate = 10Mbps;} <-- node3.out[2];
        node27.out[2] --> {  delay = 3.306575970594101ms; datarate = 10Mbps;} --> node34.in[1];
        node27.in[2] <-- {  delay = 3.306575970594101ms; datarate = 10Mbps;} <-- node34.out[1];
        node28.out[0] --> {  delay = 1.9556587252579596ms; datarate = 10Mbps;} --> node1.in[4];
        node28.in[0] <-- {  delay = 1.9556587252579596ms; datarate = 10Mbps;} <-- node1.out[4];
        node28.out[1] --> {  delay = 0.3743207468987421ms; datarate = 10Mbps;} --> node81.in[0];
        node28.in[1] <-- {  delay = 0.3743207468987421ms; datarate = 10Mbps;} <-- node81.out[0];
        node29.out[1] --> {  delay = 0.10006922855944561ms; datarate = 10Mbps;} --> node95.in[4];
        node29.in[1] <-- {  delay = 0.10006922855944561ms; datarate = 10Mbps;} <-- node95.out[4];
        node29.out[2] --> {  delay = 1.278403705733974ms; datarate = 10Mbps;} --> node62.in[4];
        node29.in[2] <-- {  delay = 1.278403705733974ms; datarate = 10Mbps;} <-- node62.out[4];
        node30.out[1] --> {  delay = 0.8440226290736104ms; datarate = 10Mbps;} --> node48.in[0];
        node30.in[1] <-- {  delay = 0.8440226290736104ms; datarate = 10Mbps;} <-- node48.out[0];
        node30.out[2] --> {  delay = 1.8996760754940052ms; datarate = 10Mbps;} --> node66.in[0];
        node30.in[2] <-- {  delay = 1.8996760754940052ms; datarate = 10Mbps;} <-- node66.out[0];
        node31.out[1] --> {  delay = 2.2414737802641804ms; datarate = 10Mbps;} --> node30.in[3];
        node31.in[1] <-- {  delay = 2.2414737802641804ms; datarate = 10Mbps;} <-- node30.out[3];
        node31.out[2] --> {  delay = 1.8634324640118443ms; datarate = 10Mbps;} --> node62.in[5];
        node31.in[2] <-- {  delay = 1.8634324640118443ms; datarate = 10Mbps;} <-- node62.out[5];
        node32.out[3] --> {  delay = 3.2436857265396455ms; datarate = 10Mbps;} --> node31.in[3];
        node32.in[3] <-- {  delay = 3.2436857265396455ms; datarate = 10Mbps;} <-- node31.out[3];
        node32.out[4] --> {  delay = 2.3697821632839733ms; datarate = 10Mbps;} --> node62.in[6];
        node32.in[4] <-- {  delay = 2.3697821632839733ms; datarate = 10Mbps;} <-- node62.out[6];
        node33.out[0] --> {  delay = 1.1063974596269959ms; datarate = 10Mbps;} --> node53.in[0];
        node33.in[0] <-- {  delay = 1.1063974596269959ms; datarate = 10Mbps;} <-- node53.out[0];
        node33.out[1] --> {  delay = 2.2441303860765602ms; datarate = 10Mbps;} --> node85.in[0];
        node33.in[1] <-- {  delay = 2.2441303860765602ms; datarate = 10Mbps;} <-- node85.out[0];
        node34.out[2] --> {  delay = 1.2380935866177276ms; datarate = 10Mbps;} --> node30.in[4];
        node34.in[2] <-- {  delay = 1.2380935866177276ms; datarate = 10Mbps;} <-- node30.out[4];
        node34.out[3] --> {  delay = 1.3449625303817396ms; datarate = 10Mbps;} --> node59.in[1];
        node34.in[3] <-- {  delay = 1.3449625303817396ms; datarate = 10Mbps;} <-- node59.out[1];
        node35.out[2] --> {  delay = 1.566295664487591ms; datarate = 10Mbps;} --> node85.in[1];
        node35.in[2] <-- {  delay = 1.566295664487591ms; datarate = 10Mbps;} <-- node85.out[1];
        node35.out[3] --> {  delay = 1.5912510755887457ms; datarate = 10Mbps;} --> node30.in[5];
        node35.in[3] <-- {  delay = 1.5912510755887457ms; datarate = 10Mbps;} <-- node30.out[5];
        node36.out[1] --> {  delay = 0.3756115375059701ms; datarate = 10Mbps;} --> node38.in[1];
        node36.in[1] <-- {  delay = 0.3756115375059701ms; datarate = 10Mbps;} <-- node38.out[1];
        node36.out[2] --> {  delay = 1.5482621011195217ms; datarate = 10Mbps;} --> node93.in[1];
        node36.in[2] <-- {  delay = 1.5482621011195217ms; datarate = 10Mbps;} <-- node93.out[1];
        node37.out[1] --> {  delay = 1.4078301736535666ms; datarate = 10Mbps;} --> node14.in[2];
        node37.in[1] <-- {  delay = 1.4078301736535666ms; datarate = 10Mbps;} <-- node14.out[2];
        node37.out[2] --> {  delay = 0.37206977836716487ms; datarate = 10Mbps;} --> node97.in[1];
        node37.in[2] <-- {  delay = 0.37206977836716487ms; datarate = 10Mbps;} <-- node97.out[1];
        node38.out[2] --> {  delay = 0.2535964771835349ms; datarate = 10Mbps;} --> node63.in[0];
        node38.in[2] <-- {  delay = 0.2535964771835349ms; datarate = 10Mbps;} <-- node63.out[0];
        node38.out[3] --> {  delay = 1.596197430412861ms; datarate = 10Mbps;} --> node2.in[4];
        node38.in[3] <-- {  delay = 1.596197430412861ms; datarate = 10Mbps;} <-- node2.out[4];
        node39.out[0] --> {  delay = 1.0677386166261758ms; datarate = 10Mbps;} --> node29.in[3];
        node39.in[0] <-- {  delay = 1.0677386166261758ms; datarate = 10Mbps;} <-- node29.out[3];
        node39.out[1] --> {  delay = 0.09878229088045201ms; datarate = 10Mbps;} --> node93.in[2];
        node39.in[1] <-- {  delay = 0.09878229088045201ms; datarate = 10Mbps;} <-- node93.out[2];
        node40.out[0] --> {  delay = 1.2477972521364162ms; datarate = 10Mbps;} --> node13.in[2];
        node40.in[0] <-- {  delay = 1.2477972521364162ms; datarate = 10Mbps;} <-- node13.out[2];
        node40.out[1] --> {  delay = 0.340496895493611ms; datarate = 10Mbps;} --> node17.in[3];
        node40.in[1] <-- {  delay = 0.340496895493611ms; datarate = 10Mbps;} <-- node17.out[3];
        node41.out[0] --> {  delay = 1.1269728417242735ms; datarate = 10Mbps;} --> node28.in[2];
        node41.in[0] <-- {  delay = 1.1269728417242735ms; datarate = 10Mbps;} <-- node28.out[2];
        node41.out[1] --> {  delay = 2.5399301493355924ms; datarate = 10Mbps;} --> node74.in[0];
        node41.in[1] <-- {  delay = 2.5399301493355924ms; datarate = 10Mbps;} <-- node74.out[0];
        node42.out[0] --> {  delay = 0.8002480029729663ms; datarate = 10Mbps;} --> node22.in[2];
        node42.in[0] <-- {  delay = 0.8002480029729663ms; datarate = 10Mbps;} <-- node22.out[2];
        node42.out[1] --> {  delay = 1.532758776473537ms; datarate = 10Mbps;} --> node18.in[3];
        node42.in[1] <-- {  delay = 1.532758776473537ms; datarate = 10Mbps;} <-- node18.out[3];
        node43.out[1] --> {  delay = 3.6682861146466075ms; datarate = 10Mbps;} --> node15.in[2];
        node43.in[1] <-- {  delay = 3.6682861146466075ms; datarate = 10Mbps;} <-- node15.out[2];
        node43.out[2] --> {  delay = 1.192135863313107ms; datarate = 10Mbps;} --> node6.in[2];
        node43.in[2] <-- {  delay = 1.192135863313107ms; datarate = 10Mbps;} <-- node6.out[2];
        node44.out[0] --> {  delay = 1.853157952795577ms; datarate = 10Mbps;} --> node23.in[2];
        node44.in[0] <-- {  delay = 1.853157952795577ms; datarate = 10Mbps;} <-- node23.out[2];
        node44.out[1] --> {  delay = 0.9166460291901182ms; datarate = 10Mbps;} --> node34.in[4];
        node44.in[1] <-- {  delay = 0.9166460291901182ms; datarate = 10Mbps;} <-- node34.out[4];
        node45.out[2] --> {  delay = 1.327652145254977ms; datarate = 10Mbps;} --> node67.in[0];
        node45.in[2] <-- {  delay = 1.327652145254977ms; datarate = 10Mbps;} <-- node67.out[0];
        node45.out[3] --> {  delay = 2.8523026576541093ms; datarate = 10Mbps;} --> node30.in[6];
        node45.in[3] <-- {  delay = 2.8523026576541093ms; datarate = 10Mbps;} <-- node30.out[6];
        node46.out[0] --> {  delay = 1.5680528455285199ms; datarate = 10Mbps;} --> node81.in[1];
        node46.in[0] <-- {  delay = 1.5680528455285199ms; datarate = 10Mbps;} <-- node81.out[1];
        node46.out[1] --> {  delay = 1.263891039202201ms; datarate = 10Mbps;} --> node63.in[1];
        node46.in[1] <-- {  delay = 1.263891039202201ms; datarate = 10Mbps;} <-- node63.out[1];
        node47.out[0] --> {  delay = 1.6048419996219796ms; datarate = 10Mbps;} --> node38.in[4];
        node47.in[0] <-- {  delay = 1.6048419996219796ms; datarate = 10Mbps;} <-- node38.out[4];
        node47.out[1] --> {  delay = 1.7638476995578294ms; datarate = 10Mbps;} --> node94.in[1];
        node47.in[1] <-- {  delay = 1.7638476995578294ms; datarate = 10Mbps;} <-- node94.out[1];
        node48.out[1] --> {  delay = 2.528902059133131ms; datarate = 10Mbps;} --> node43.in[3];
        node48.in[1] <-- {  delay = 2.528902059133131ms; datarate = 10Mbps;} <-- node43.out[3];
        node48.out[2] --> {  delay = 1.0133598157779724ms; datarate = 10Mbps;} --> node67.in[1];
        node48.in[2] <-- {  delay = 1.0133598157779724ms; datarate = 10Mbps;} <-- node67.out[1];
        node49.out[0] --> {  delay = 2.9053892248301576ms; datarate = 10Mbps;} --> node95.in[5];
        node49.in[0] <-- {  delay = 2.9053892248301576ms; datarate = 10Mbps;} <-- node95.out[5];
        node49.out[1] --> {  delay = 0.04958786770852823ms; datarate = 10Mbps;} --> node43.in[4];
        node49.in[1] <-- {  delay = 0.04958786770852823ms; datarate = 10Mbps;} <-- node43.out[4];
        node50.out[0] --> {  delay = 1.5822404618520418ms; datarate = 10Mbps;} --> node99.in[0];
        node50.in[0] <-- {  delay = 1.5822404618520418ms; datarate = 10Mbps;} <-- node99.out[0];
        node50.out[1] --> {  delay = 1.4526285501824363ms; datarate = 10Mbps;} --> node29.in[4];
        node50.in[1] <-- {  delay = 1.4526285501824363ms; datarate = 10Mbps;} <-- node29.out[4];
        node51.out[0] --> {  delay = 2.947321456954252ms; datarate = 10Mbps;} --> node75.in[0];
        node51.in[0] <-- {  delay = 2.947321456954252ms; datarate = 10Mbps;} <-- node75.out[0];
        node51.out[1] --> {  delay = 1.1808357422801845ms; datarate = 10Mbps;} --> node54.in[2];
        node51.in[1] <-- {  delay = 1.1808357422801845ms; datarate = 10Mbps;} <-- node54.out[2];
        node52.out[0] --> {  delay = 0.2702075067022351ms; datarate = 10Mbps;} --> node30.in[7];
        node52.in[0] <-- {  delay = 0.2702075067022351ms; datarate = 10Mbps;} <-- node30.out[7];
        node52.out[1] --> {  delay = 1.3857731552260573ms; datarate = 10Mbps;} --> node95.in[6];
        node52.in[1] <-- {  delay = 1.3857731552260573ms; datarate = 10Mbps;} <-- node95.out[6];
        node53.out[1] --> {  delay = 1.3111148247204436ms; datarate = 10Mbps;} --> node59.in[2];
        node53.in[1] <-- {  delay = 1.3111148247204436ms; datarate = 10Mbps;} <-- node59.out[2];
        node53.out[2] --> {  delay = 1.0951727630488803ms; datarate = 10Mbps;} --> node81.in[2];
        node53.in[2] <-- {  delay = 1.0951727630488803ms; datarate = 10Mbps;} <-- node81.out[2];
        node54.out[3] --> {  delay = 1.219595362377866ms; datarate = 10Mbps;} --> node86.in[0];
        node54.in[3] <-- {  delay = 1.219595362377866ms; datarate = 10Mbps;} <-- node86.out[0];
        node54.out[4] --> {  delay = 0.47075100813933396ms; datarate = 10Mbps;} --> node34.in[5];
        node54.in[4] <-- {  delay = 0.47075100813933396ms; datarate = 10Mbps;} <-- node34.out[5];
        node55.out[0] --> {  delay = 1.0441035733681858ms; datarate = 10Mbps;} --> node36.in[3];
        node55.in[0] <-- {  delay = 1.0441035733681858ms; datarate = 10Mbps;} <-- node36.out[3];
        node55.out[1] --> {  delay = 0.5344525445632053ms; datarate = 10Mbps;} --> node93.in[3];
        node55.in[1] <-- {  delay = 0.5344525445632053ms; datarate = 10Mbps;} <-- node93.out[3];
        node56.out[0] --> {  delay = 1.9106556031665998ms; datarate = 10Mbps;} --> node64.in[0];
        node56.in[0] <-- {  delay = 1.9106556031665998ms; datarate = 10Mbps;} <-- node64.out[0];
        node56.out[1] --> {  delay = 0.7044602032769736ms; datarate = 10Mbps;} --> node31.in[4];
        node56.in[1] <-- {  delay = 0.7044602032769736ms; datarate = 10Mbps;} <-- node31.out[4];
        node57.out[0] --> {  delay = 2.5123311601935137ms; datarate = 10Mbps;} --> node32.in[5];
        node57.in[0] <-- {  delay = 2.5123311601935137ms; datarate = 10Mbps;} <-- node32.out[5];
        node57.out[1] --> {  delay = 1.1216188801806006ms; datarate = 10Mbps;} --> node96.in[2];
        node57.in[1] <-- {  delay = 1.1216188801806006ms; datarate = 10Mbps;} <-- node96.out[2];
        node58.out[0] --> {  delay = 0.3982844693075094ms; datarate = 10Mbps;} --> node85.in[2];
        node58.in[0] <-- {  delay = 0.3982844693075094ms; datarate = 10Mbps;} <-- node85.out[2];
        node58.out[1] --> {  delay = 0.4163141833176863ms; datarate = 10Mbps;} --> node30.in[8];
        node58.in[1] <-- {  delay = 0.4163141833176863ms; datarate = 10Mbps;} <-- node30.out[8];
        node59.out[3] --> {  delay = 1.6015419263065216ms; datarate = 10Mbps;} --> node62.in[7];
        node59.in[3] <-- {  delay = 1.6015419263065216ms; datarate = 10Mbps;} <-- node62.out[7];
        node59.out[4] --> {  delay = 0.20953560845802743ms; datarate = 10Mbps;} --> node30.in[9];
        node59.in[4] <-- {  delay = 0.20953560845802743ms; datarate = 10Mbps;} <-- node30.out[9];
        node60.out[0] --> {  delay = 1.5506999798941254ms; datarate = 10Mbps;} --> node30.in[10];
        node60.in[0] <-- {  delay = 1.5506999798941254ms; datarate = 10Mbps;} <-- node30.out[10];
        node60.out[1] --> {  delay = 0.8145796384811836ms; datarate = 10Mbps;} --> node86.in[1];
        node60.in[1] <-- {  delay = 0.8145796384811836ms; datarate = 10Mbps;} <-- node86.out[1];
        node61.out[0] --> {  delay = 0.7238417723478308ms; datarate = 10Mbps;} --> node60.in[2];
        node61.in[0] <-- {  delay = 0.7238417723478308ms; datarate = 10Mbps;} <-- node60.out[2];
        node61.out[1] --> {  delay = 1.5501509839553949ms; datarate = 10Mbps;} --> node62.in[8];
        node61.in[1] <-- {  delay = 1.5501509839553949ms; datarate = 10Mbps;} <-- node62.out[8];
        node62.out[9] --> {  delay = 1.7808230743863431ms; datarate = 10Mbps;} --> node5.in[2];
        node62.in[9] <-- {  delay = 1.7808230743863431ms; datarate = 10Mbps;} <-- node5.out[2];
        node62.out[10] --> {  delay = 1.0770556849306832ms; datarate = 10Mbps;} --> node48.in[3];
        node62.in[10] <-- {  delay = 1.0770556849306832ms; datarate = 10Mbps;} <-- node48.out[3];
        node63.out[2] --> {  delay = 2.0096233250183526ms; datarate = 10Mbps;} --> node31.in[5];
        node63.in[2] <-- {  delay = 2.0096233250183526ms; datarate = 10Mbps;} <-- node31.out[5];
        node63.out[3] --> {  delay = 1.1888974941973909ms; datarate = 10Mbps;} --> node1.in[5];
        node63.in[3] <-- {  delay = 1.1888974941973909ms; datarate = 10Mbps;} <-- node1.out[5];
        node64.out[1] --> {  delay = 0.8659827583619311ms; datarate = 10Mbps;} --> node58.in[2];
        node64.in[1] <-- {  delay = 0.8659827583619311ms; datarate = 10Mbps;} <-- node58.out[2];
        node64.out[2] --> {  delay = 1.037920474203912ms; datarate = 10Mbps;} --> node30.in[11];
        node64.in[2] <-- {  delay = 1.037920474203912ms; datarate = 10Mbps;} <-- node30.out[11];
        node65.out[0] --> {  delay = 2.9377454017126494ms; datarate = 10Mbps;} --> node64.in[3];
        node65.in[0] <-- {  delay = 2.9377454017126494ms; datarate = 10Mbps;} <-- node64.out[3];
        node65.out[1] --> {  delay = 1.6714490417731767ms; datarate = 10Mbps;} --> node27.in[3];
        node65.in[1] <-- {  delay = 1.6714490417731767ms; datarate = 10Mbps;} <-- node27.out[3];
        node66.out[1] --> {  delay = 0.47306162999556856ms; datarate = 10Mbps;} --> node62.in[11];
        node66.in[1] <-- {  delay = 0.47306162999556856ms; datarate = 10Mbps;} <-- node62.out[11];
        node66.out[2] --> {  delay = 1.5970023351536167ms; datarate = 10Mbps;} --> node5.in[3];
        node66.in[2] <-- {  delay = 1.5970023351536167ms; datarate = 10Mbps;} <-- node5.out[3];
        node67.out[2] --> {  delay = 1.6595442299760255ms; datarate = 10Mbps;} --> node55.in[2];
        node67.in[2] <-- {  delay = 1.6595442299760255ms; datarate = 10Mbps;} <-- node55.out[2];
        node67.out[3] --> {  delay = 0.13317523002981702ms; datarate = 10Mbps;} --> node94.in[2];
        node67.in[3] <-- {  delay = 0.13317523002981702ms; datarate = 10Mbps;} <-- node94.out[2];
        node68.out[0] --> {  delay = 0.6186152764675971ms; datarate = 10Mbps;} --> node96.in[3];
        node68.in[0] <-- {  delay = 0.6186152764675971ms; datarate = 10Mbps;} <-- node96.out[3];
        node68.out[1] --> {  delay = 1.3723152815134112ms; datarate = 10Mbps;} --> node61.in[2];
        node68.in[1] <-- {  delay = 1.3723152815134112ms; datarate = 10Mbps;} <-- node61.out[2];
        node69.out[0] --> {  delay = 0.5178307286256022ms; datarate = 10Mbps;} --> node57.in[2];
        node69.in[0] <-- {  delay = 0.5178307286256022ms; datarate = 10Mbps;} <-- node57.out[2];
        node69.out[1] --> {  delay = 1.1674266802074027ms; datarate = 10Mbps;} --> node83.in[3];
        node69.in[1] <-- {  delay = 1.1674266802074027ms; datarate = 10Mbps;} <-- node83.out[3];
        node70.out[1] --> {  delay = 1.1167425018005261ms; datarate = 10Mbps;} --> node30.in[12];
        node70.in[1] <-- {  delay = 1.1167425018005261ms; datarate = 10Mbps;} <-- node30.out[12];
        node70.out[2] --> {  delay = 0.7508823405864667ms; datarate = 10Mbps;} --> node58.in[3];
        node70.in[2] <-- {  delay = 0.7508823405864667ms; datarate = 10Mbps;} <-- node58.out[3];
        node71.out[0] --> {  delay = 0.45517751145871027ms; datarate = 10Mbps;} --> node51.in[2];
        node71.in[0] <-- {  delay = 0.45517751145871027ms; datarate = 10Mbps;} <-- node51.out[2];
        node71.out[1] --> {  delay = 0.7900963953524168ms; datarate = 10Mbps;} --> node68.in[2];
        node71.in[1] <-- {  delay = 0.7900963953524168ms; datarate = 10Mbps;} <-- node68.out[2];
        node72.out[1] --> {  delay = 0.8243555957445234ms; datarate = 10Mbps;} --> node14.in[3];
        node72.in[1] <-- {  delay = 0.8243555957445234ms; datarate = 10Mbps;} <-- node14.out[3];
        node72.out[2] --> {  delay = 1.0846527846591525ms; datarate = 10Mbps;} --> node98.in[0];
        node72.in[2] <-- {  delay = 1.0846527846591525ms; datarate = 10Mbps;} <-- node98.out[0];
        node73.out[2] --> {  delay = 0.9527153490034996ms; datarate = 10Mbps;} --> node4.in[4];
        node73.in[2] <-- {  delay = 0.9527153490034996ms; datarate = 10Mbps;} <-- node4.out[4];
        node73.out[3] --> {  delay = 0.2849387207633117ms; datarate = 10Mbps;} --> node16.in[2];
        node73.in[3] <-- {  delay = 0.2849387207633117ms; datarate = 10Mbps;} <-- node16.out[2];
        node74.out[1] --> {  delay = 0.4003325043934894ms; datarate = 10Mbps;} --> node58.in[4];
        node74.in[1] <-- {  delay = 0.4003325043934894ms; datarate = 10Mbps;} <-- node58.out[4];
        node74.out[2] --> {  delay = 0.6661267464636014ms; datarate = 10Mbps;} --> node62.in[12];
        node74.in[2] <-- {  delay = 0.6661267464636014ms; datarate = 10Mbps;} <-- node62.out[12];
        node75.out[1] --> {  delay = 1.0024974629047352ms; datarate = 10Mbps;} --> node95.in[7];
        node75.in[1] <-- {  delay = 1.0024974629047352ms; datarate = 10Mbps;} <-- node95.out[7];
        node75.out[2] --> {  delay = 0.9640002351226594ms; datarate = 10Mbps;} --> node67.in[4];
        node75.in[2] <-- {  delay = 0.9640002351226594ms; datarate = 10Mbps;} <-- node67.out[4];
        node76.out[0] --> {  delay = 0.6400694314990388ms; datarate = 10Mbps;} --> node29.in[5];
        node76.in[0] <-- {  delay = 0.6400694314990388ms; datarate = 10Mbps;} <-- node29.out[5];
        node76.out[1] --> {  delay = 0.21527163035252278ms; datarate = 10Mbps;} --> node67.in[5];
        node76.in[1] <-- {  delay = 0.21527163035252278ms; datarate = 10Mbps;} <-- node67.out[5];
        node77.out[0] --> {  delay = 0.43911530913200014ms; datarate = 10Mbps;} --> node30.in[13];
        node77.in[0] <-- {  delay = 0.43911530913200014ms; datarate = 10Mbps;} <-- node30.out[13];
        node77.out[1] --> {  delay = 1.8804792355004851ms; datarate = 10Mbps;} --> node43.in[5];
        node77.in[1] <-- {  delay = 1.8804792355004851ms; datarate = 10Mbps;} <-- node43.out[5];
        node78.out[0] --> {  delay = 1.6923570868134241ms; datarate = 10Mbps;} --> node35.in[4];
        node78.in[0] <-- {  delay = 1.6923570868134241ms; datarate = 10Mbps;} <-- node35.out[4];
        node78.out[1] --> {  delay = 2.311445147880665ms; datarate = 10Mbps;} --> node31.in[6];
        node78.in[1] <-- {  delay = 2.311445147880665ms; datarate = 10Mbps;} <-- node31.out[6];
        node79.out[0] --> {  delay = 2.6975272901468004ms; datarate = 10Mbps;} --> node74.in[3];
        node79.in[0] <-- {  delay = 2.6975272901468004ms; datarate = 10Mbps;} <-- node74.out[3];
        node79.out[1] --> {  delay = 1.1172405584222906ms; datarate = 10Mbps;} --> node82.in[2];
        node79.in[1] <-- {  delay = 1.1172405584222906ms; datarate = 10Mbps;} <-- node82.out[2];
        node80.out[0] --> {  delay = 2.273419788419646ms; datarate = 10Mbps;} --> node33.in[2];
        node80.in[0] <-- {  delay = 2.273419788419646ms; datarate = 10Mbps;} <-- node33.out[2];
        node80.out[1] --> {  delay = 0.3243985855418473ms; datarate = 10Mbps;} --> node4.in[5];
        node80.in[1] <-- {  delay = 0.3243985855418473ms; datarate = 10Mbps;} <-- node4.out[5];
        node81.out[3] --> {  delay = 0.9198268074068577ms; datarate = 10Mbps;} --> node96.in[4];
        node81.in[3] <-- {  delay = 0.9198268074068577ms; datarate = 10Mbps;} <-- node96.out[4];
        node81.out[4] --> {  delay = 0.9093583716883074ms; datarate = 10Mbps;} --> node34.in[6];
        node81.in[4] <-- {  delay = 0.9093583716883074ms; datarate = 10Mbps;} <-- node34.out[6];
        node82.out[3] --> {  delay = 0.6634907645759767ms; datarate = 10Mbps;} --> node1.in[6];
        node82.in[3] <-- {  delay = 0.6634907645759767ms; datarate = 10Mbps;} <-- node1.out[6];
        node82.out[4] --> {  delay = 2.411855252301415ms; datarate = 10Mbps;} --> node96.in[5];
        node82.in[4] <-- {  delay = 2.411855252301415ms; datarate = 10Mbps;} <-- node96.out[5];
        node83.out[4] --> {  delay = 1.1035276314390883ms; datarate = 10Mbps;} --> node62.in[13];
        node83.in[4] <-- {  delay = 1.1035276314390883ms; datarate = 10Mbps;} <-- node62.out[13];
        node83.out[5] --> {  delay = 0.5246296258942909ms; datarate = 10Mbps;} --> node59.in[5];
        node83.in[5] <-- {  delay = 0.5246296258942909ms; datarate = 10Mbps;} <-- node59.out[5];
        node84.out[0] --> {  delay = 0.17335708238779352ms; datarate = 10Mbps;} --> node46.in[2];
        node84.in[0] <-- {  delay = 0.17335708238779352ms; datarate = 10Mbps;} <-- node46.out[2];
        node84.out[1] --> {  delay = 0.8514774823946305ms; datarate = 10Mbps;} --> node6.in[3];
        node84.in[1] <-- {  delay = 0.8514774823946305ms; datarate = 10Mbps;} <-- node6.out[3];
        node85.out[3] --> {  delay = 1.2454498958787341ms; datarate = 10Mbps;} --> node34.in[7];
        node85.in[3] <-- {  delay = 1.2454498958787341ms; datarate = 10Mbps;} <-- node34.out[7];
        node85.out[4] --> {  delay = 0.28810361709218424ms; datarate = 10Mbps;} --> node59.in[6];
        node85.in[4] <-- {  delay = 0.28810361709218424ms; datarate = 10Mbps;} <-- node59.out[6];
        node86.out[2] --> {  delay = 1.4736210891541446ms; datarate = 10Mbps;} --> node62.in[14];
        node86.in[2] <-- {  delay = 1.4736210891541446ms; datarate = 10Mbps;} <-- node62.out[14];
        node86.out[3] --> {  delay = 2.35753370972757ms; datarate = 10Mbps;} --> node30.in[14];
        node86.in[3] <-- {  delay = 2.35753370972757ms; datarate = 10Mbps;} <-- node30.out[14];
        node87.out[2] --> {  delay = 1.468364150550095ms; datarate = 10Mbps;} --> node28.in[3];
        node87.in[2] <-- {  delay = 1.468364150550095ms; datarate = 10Mbps;} <-- node28.out[3];
        node87.out[3] --> {  delay = 1.0779282569059192ms; datarate = 10Mbps;} --> node60.in[3];
        node87.in[3] <-- {  delay = 1.0779282569059192ms; datarate = 10Mbps;} <-- node60.out[3];
        node88.out[1] --> {  delay = 1.434441964200698ms; datarate = 10Mbps;} --> node95.in[8];
        node88.in[1] <-- {  delay = 1.434441964200698ms; datarate = 10Mbps;} <-- node95.out[8];
        node88.out[2] --> {  delay = 0.3932379747561671ms; datarate = 10Mbps;} --> node30.in[15];
        node88.in[2] <-- {  delay = 0.3932379747561671ms; datarate = 10Mbps;} <-- node30.out[15];
        node89.out[0] --> {  delay = 2.329502838350498ms; datarate = 10Mbps;} --> node54.in[5];
        node89.in[0] <-- {  delay = 2.329502838350498ms; datarate = 10Mbps;} <-- node54.out[5];
        node89.out[1] --> {  delay = 0.6938854795685542ms; datarate = 10Mbps;} --> node26.in[2];
        node89.in[1] <-- {  delay = 0.6938854795685542ms; datarate = 10Mbps;} <-- node26.out[2];
        node90.out[0] --> {  delay = 1.3041972219301374ms; datarate = 10Mbps;} --> node63.in[4];
        node90.in[0] <-- {  delay = 1.3041972219301374ms; datarate = 10Mbps;} <-- node63.out[4];
        node90.out[1] --> {  delay = 2.2026472565799096ms; datarate = 10Mbps;} --> node15.in[3];
        node90.in[1] <-- {  delay = 2.2026472565799096ms; datarate = 10Mbps;} <-- node15.out[3];
        node91.out[1] --> {  delay = 1.467454571183099ms; datarate = 10Mbps;} --> node86.in[4];
        node91.in[1] <-- {  delay = 1.467454571183099ms; datarate = 10Mbps;} <-- node86.out[4];
        node91.out[2] --> {  delay = 0.942665352745725ms; datarate = 10Mbps;} --> node21.in[2];
        node91.in[2] <-- {  delay = 0.942665352745725ms; datarate = 10Mbps;} <-- node21.out[2];
        node92.out[0] --> {  delay = 0.23853914535476572ms; datarate = 10Mbps;} --> node5.in[4];
        node92.in[0] <-- {  delay = 0.23853914535476572ms; datarate = 10Mbps;} <-- node5.out[4];
        node92.out[1] --> {  delay = 0.6764857485970073ms; datarate = 10Mbps;} --> node72.in[3];
        node92.in[1] <-- {  delay = 0.6764857485970073ms; datarate = 10Mbps;} <-- node72.out[3];
        node93.out[4] --> {  delay = 2.728001700772254ms; datarate = 10Mbps;} --> node34.in[8];
        node93.in[4] <-- {  delay = 2.728001700772254ms; datarate = 10Mbps;} <-- node34.out[8];
        node93.out[5] --> {  delay = 1.5238075344275919ms; datarate = 10Mbps;} --> node63.in[5];
        node93.in[5] <-- {  delay = 1.5238075344275919ms; datarate = 10Mbps;} <-- node63.out[5];
        node94.out[3] --> {  delay = 0.8183204972600101ms; datarate = 10Mbps;} --> node64.in[4];
        node94.in[3] <-- {  delay = 0.8183204972600101ms; datarate = 10Mbps;} <-- node64.out[4];
        node94.out[4] --> {  delay = 1.280421309997005ms; datarate = 10Mbps;} --> node60.in[4];
        node94.in[4] <-- {  delay = 1.280421309997005ms; datarate = 10Mbps;} <-- node60.out[4];
        node95.out[9] --> {  delay = 1.1816316807030542ms; datarate = 10Mbps;} --> node62.in[15];
        node95.in[9] <-- {  delay = 1.1816316807030542ms; datarate = 10Mbps;} <-- node62.out[15];
        node95.out[10] --> {  delay = 0.7031559609421985ms; datarate = 10Mbps;} --> node31.in[7];
        node95.in[10] <-- {  delay = 0.7031559609421985ms; datarate = 10Mbps;} <-- node31.out[7];
        node96.out[6] --> {  delay = 0.6108687058601566ms; datarate = 10Mbps;} --> node86.in[5];
        node96.in[6] <-- {  delay = 0.6108687058601566ms; datarate = 10Mbps;} <-- node86.out[5];
        node96.out[7] --> {  delay = 2.135125508293001ms; datarate = 10Mbps;} --> node32.in[6];
        node96.in[7] <-- {  delay = 2.135125508293001ms; datarate = 10Mbps;} <-- node32.out[6];
        node97.out[2] --> {  delay = 1.015268505582948ms; datarate = 10Mbps;} --> node3.in[3];
        node97.in[2] <-- {  delay = 1.015268505582948ms; datarate = 10Mbps;} <-- node3.out[3];
        node97.out[3] --> {  delay = 1.3370054525153379ms; datarate = 10Mbps;} --> node16.in[3];
        node97.in[3] <-- {  delay = 1.3370054525153379ms; datarate = 10Mbps;} <-- node16.out[3];
        node98.out[1] --> {  delay = 1.189893775228793ms; datarate = 10Mbps;} --> node80.in[2];
        node98.in[1] <-- {  delay = 1.189893775228793ms; datarate = 10Mbps;} <-- node80.out[2];
        node98.out[2] --> {  delay = 0.6355774770638326ms; datarate = 10Mbps;} --> node1.in[7];
        node98.in[2] <-- {  delay = 0.6355774770638326ms; datarate = 10Mbps;} <-- node1.out[7];
        node99.out[1] --> {  delay = 1.9115289126979176ms; datarate = 10Mbps;} --> node12.in[2];
        node99.in[1] <-- {  delay = 1.9115289126979176ms; datarate = 10Mbps;} <-- node12.out[2];
        node99.out[2] --> {  delay = 1.9816290242699095ms; datarate = 10Mbps;} --> node3.in[4];
        node99.in[2] <-- {  delay = 1.9816290242699095ms; datarate = 10Mbps;} <-- node3.out[4];
}

