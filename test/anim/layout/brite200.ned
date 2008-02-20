//
// OMNeT++ network description -- exported from BRITE200 topology
// Generator Model Used:
//   Model (3 - ASWaxman):  200 1000 100 1  2  0.15000000596046448 0.20000000298023224 1 1 10.0 1024.0 
//


//
// Prototype for modules that can be used as node in a BRITE200Network
//
module BRITE200Node
{
    gates:
        input in[];
        output out[];
    connections allowunconnected:
}

//
// Topology exported from BRITE200. Actual module type to be used as nodes
// in the network should be supplied in the 'nodetype' parameter.
//
module BRITE200Network
{
    parameters:
        string nodetype;
    submodules:
        node0: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=330,760");
            gates:
                in[3];
                out[3];
        };
        node1: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=47,336");
            gates:
                in[2];
                out[2];
        };
        node2: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=333,214");
            gates:
                in[2];
                out[2];
        };
        node3: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=593,938");
            gates:
                in[4];
                out[4];
        };
        node4: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=913,656");
            gates:
                in[4];
                out[4];
        };
        node5: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=313,537");
            gates:
                in[2];
                out[2];
        };
        node6: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=878,134");
            gates:
                in[3];
                out[3];
        };
        node7: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=595,532");
            gates:
                in[3];
                out[3];
        };
        node8: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=617,157");
            gates:
                in[2];
                out[2];
        };
        node9: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=21,375");
            gates:
                in[2];
                out[2];
        };
        node10: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=580,235");
            gates:
                in[2];
                out[2];
        };
        node11: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=584,471");
            gates:
                in[3];
                out[3];
        };
        node12: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=949,96");
            gates:
                in[2];
                out[2];
        };
        node13: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=67,677");
            gates:
                in[3];
                out[3];
        };
        node14: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=631,969");
            gates:
                in[2];
                out[2];
        };
        node15: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=732,547");
            gates:
                in[4];
                out[4];
        };
        node16: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=335,973");
            gates:
                in[3];
                out[3];
        };
        node17: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=923,42");
            gates:
                in[4];
                out[4];
        };
        node18: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=26,446");
            gates:
                in[3];
                out[3];
        };
        node19: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=43,684");
            gates:
                in[2];
                out[2];
        };
        node20: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=504,988");
            gates:
                in[3];
                out[3];
        };
        node21: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=665,27");
            gates:
                in[2];
                out[2];
        };
        node22: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=197,123");
            gates:
                in[2];
                out[2];
        };
        node23: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=169,895");
            gates:
                in[5];
                out[5];
        };
        node24: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=500,713");
            gates:
                in[7];
                out[7];
        };
        node25: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=420,23");
            gates:
                in[6];
                out[6];
        };
        node26: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=185,698");
            gates:
                in[6];
                out[6];
        };
        node27: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=466,639");
            gates:
                in[5];
                out[5];
        };
        node28: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=439,324");
            gates:
                in[9];
                out[9];
        };
        node29: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=152,41");
            gates:
                in[5];
                out[5];
        };
        node30: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=78,710");
            gates:
                in[7];
                out[7];
        };
        node31: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=813,400");
            gates:
                in[12];
                out[12];
        };
        node32: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=378,531");
            gates:
                in[10];
                out[10];
        };
        node33: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=970,868");
            gates:
                in[6];
                out[6];
        };
        node34: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=709,636");
            gates:
                in[8];
                out[8];
        };
        node35: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=9,583");
            gates:
                in[9];
                out[9];
        };
        node36: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=904,584");
            gates:
                in[9];
                out[9];
        };
        node37: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=385,280");
            gates:
                in[3];
                out[3];
        };
        node38: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=849,488");
            gates:
                in[7];
                out[7];
        };
        node39: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=807,57");
            gates:
                in[3];
                out[3];
        };
        node40: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=307,28");
            gates:
                in[5];
                out[5];
        };
        node41: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=184,302");
            gates:
                in[4];
                out[4];
        };
        node42: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=396,187");
            gates:
                in[2];
                out[2];
        };
        node43: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=866,910");
            gates:
                in[6];
                out[6];
        };
        node44: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=455,80");
            gates:
                in[3];
                out[3];
        };
        node45: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=851,685");
            gates:
                in[3];
                out[3];
        };
        node46: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=270,230");
            gates:
                in[6];
                out[6];
        };
        node47: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=183,278");
            gates:
                in[2];
                out[2];
        };
        node48: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=727,779");
            gates:
                in[2];
                out[2];
        };
        node49: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=716,87");
            gates:
                in[2];
                out[2];
        };
        node50: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=41,423");
            gates:
                in[2];
                out[2];
        };
        node51: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=810,300");
            gates:
                in[4];
                out[4];
        };
        node52: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=622,303");
            gates:
                in[3];
                out[3];
        };
        node53: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=247,651");
            gates:
                in[2];
                out[2];
        };
        node54: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=488,695");
            gates:
                in[2];
                out[2];
        };
        node55: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=896,552");
            gates:
                in[4];
                out[4];
        };
        node56: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=910,103");
            gates:
                in[3];
                out[3];
        };
        node57: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=169,934");
            gates:
                in[2];
                out[2];
        };
        node58: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=612,273");
            gates:
                in[4];
                out[4];
        };
        node59: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=363,535");
            gates:
                in[2];
                out[2];
        };
        node60: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=432,739");
            gates:
                in[3];
                out[3];
        };
        node61: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=333,405");
            gates:
                in[4];
                out[4];
        };
        node62: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=818,717");
            gates:
                in[2];
                out[2];
        };
        node63: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=765,40");
            gates:
                in[3];
                out[3];
        };
        node64: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=911,570");
            gates:
                in[2];
                out[2];
        };
        node65: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=516,962");
            gates:
                in[3];
                out[3];
        };
        node66: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=262,610");
            gates:
                in[3];
                out[3];
        };
        node67: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=123,493");
            gates:
                in[3];
                out[3];
        };
        node68: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=400,690");
            gates:
                in[3];
                out[3];
        };
        node69: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=560,177");
            gates:
                in[2];
                out[2];
        };
        node70: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=607,905");
            gates:
                in[2];
                out[2];
        };
        node71: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=803,588");
            gates:
                in[2];
                out[2];
        };
        node72: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=213,750");
            gates:
                in[3];
                out[3];
        };
        node73: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=553,220");
            gates:
                in[2];
                out[2];
        };
        node74: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=668,199");
            gates:
                in[4];
                out[4];
        };
        node75: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=724,810");
            gates:
                in[2];
                out[2];
        };
        node76: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=464,573");
            gates:
                in[3];
                out[3];
        };
        node77: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=955,682");
            gates:
                in[3];
                out[3];
        };
        node78: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=682,465");
            gates:
                in[6];
                out[6];
        };
        node79: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=445,670");
            gates:
                in[5];
                out[5];
        };
        node80: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=581,691");
            gates:
                in[4];
                out[4];
        };
        node81: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=857,743");
            gates:
                in[8];
                out[8];
        };
        node82: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=207,0");
            gates:
                in[8];
                out[8];
        };
        node83: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=950,729");
            gates:
                in[3];
                out[3];
        };
        node84: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=386,212");
            gates:
                in[5];
                out[5];
        };
        node85: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=791,395");
            gates:
                in[14];
                out[14];
        };
        node86: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=871,617");
            gates:
                in[8];
                out[8];
        };
        node87: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=161,760");
            gates:
                in[7];
                out[7];
        };
        node88: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=634,649");
            gates:
                in[4];
                out[4];
        };
        node89: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=775,675");
            gates:
                in[2];
                out[2];
        };
        node90: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=465,344");
            gates:
                in[2];
                out[2];
        };
        node91: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=490,613");
            gates:
                in[2];
                out[2];
        };
        node92: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=104,618");
            gates:
                in[2];
                out[2];
        };
        node93: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=513,127");
            gates:
                in[4];
                out[4];
        };
        node94: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=122,149");
            gates:
                in[6];
                out[6];
        };
        node95: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=979,623");
            gates:
                in[7];
                out[7];
        };
        node96: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=610,541");
            gates:
                in[13];
                out[13];
        };
        node97: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=925,928");
            gates:
                in[3];
                out[3];
        };
        node98: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=569,471");
            gates:
                in[6];
                out[6];
        };
        node99: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=834,643");
            gates:
                in[5];
                out[5];
        };
        node100: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=547,752");
            gates:
                in[11];
                out[11];
        };
        node101: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=439,408");
            gates:
                in[5];
                out[5];
        };
        node102: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=686,563");
            gates:
                in[9];
                out[9];
        };
        node103: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=389,34");
            gates:
                in[2];
                out[2];
        };
        node104: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=842,648");
            gates:
                in[5];
                out[5];
        };
        node105: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=718,904");
            gates:
                in[6];
                out[6];
        };
        node106: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=864,925");
            gates:
                in[2];
                out[2];
        };
        node107: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=820,188");
            gates:
                in[2];
                out[2];
        };
        node108: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=403,785");
            gates:
                in[5];
                out[5];
        };
        node109: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=736,166");
            gates:
                in[3];
                out[3];
        };
        node110: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=898,346");
            gates:
                in[2];
                out[2];
        };
        node111: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=147,427");
            gates:
                in[3];
                out[3];
        };
        node112: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=452,53");
            gates:
                in[2];
                out[2];
        };
        node113: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=406,762");
            gates:
                in[3];
                out[3];
        };
        node114: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=410,318");
            gates:
                in[4];
                out[4];
        };
        node115: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=323,349");
            gates:
                in[6];
                out[6];
        };
        node116: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=343,340");
            gates:
                in[3];
                out[3];
        };
        node117: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=154,205");
            gates:
                in[3];
                out[3];
        };
        node118: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=27,853");
            gates:
                in[4];
                out[4];
        };
        node119: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=475,963");
            gates:
                in[2];
                out[2];
        };
        node120: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=732,313");
            gates:
                in[2];
                out[2];
        };
        node121: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=141,735");
            gates:
                in[6];
                out[6];
        };
        node122: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=820,957");
            gates:
                in[2];
                out[2];
        };
        node123: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=583,184");
            gates:
                in[4];
                out[4];
        };
        node124: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=207,225");
            gates:
                in[3];
                out[3];
        };
        node125: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=39,990");
            gates:
                in[3];
                out[3];
        };
        node126: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=720,300");
            gates:
                in[3];
                out[3];
        };
        node127: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=152,531");
            gates:
                in[4];
                out[4];
        };
        node128: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=751,877");
            gates:
                in[2];
                out[2];
        };
        node129: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=555,999");
            gates:
                in[2];
                out[2];
        };
        node130: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=925,831");
            gates:
                in[2];
                out[2];
        };
        node131: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=687,103");
            gates:
                in[2];
                out[2];
        };
        node132: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=642,151");
            gates:
                in[6];
                out[6];
        };
        node133: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=111,225");
            gates:
                in[2];
                out[2];
        };
        node134: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=514,373");
            gates:
                in[2];
                out[2];
        };
        node135: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=4,167");
            gates:
                in[2];
                out[2];
        };
        node136: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=658,734");
            gates:
                in[2];
                out[2];
        };
        node137: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=367,595");
            gates:
                in[6];
                out[6];
        };
        node138: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=772,463");
            gates:
                in[3];
                out[3];
        };
        node139: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=692,941");
            gates:
                in[5];
                out[5];
        };
        node140: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=700,945");
            gates:
                in[4];
                out[4];
        };
        node141: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=512,742");
            gates:
                in[2];
                out[2];
        };
        node142: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=947,950");
            gates:
                in[3];
                out[3];
        };
        node143: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=280,404");
            gates:
                in[3];
                out[3];
        };
        node144: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=195,805");
            gates:
                in[5];
                out[5];
        };
        node145: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=497,900");
            gates:
                in[7];
                out[7];
        };
        node146: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=455,129");
            gates:
                in[9];
                out[9];
        };
        node147: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=886,210");
            gates:
                in[2];
                out[2];
        };
        node148: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=892,740");
            gates:
                in[2];
                out[2];
        };
        node149: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=629,177");
            gates:
                in[5];
                out[5];
        };
        node150: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=735,431");
            gates:
                in[6];
                out[6];
        };
        node151: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=434,48");
            gates:
                in[2];
                out[2];
        };
        node152: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=918,571");
            gates:
                in[2];
                out[2];
        };
        node153: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=215,145");
            gates:
                in[6];
                out[6];
        };
        node154: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=278,930");
            gates:
                in[2];
                out[2];
        };
        node155: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=704,22");
            gates:
                in[3];
                out[3];
        };
        node156: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=199,669");
            gates:
                in[5];
                out[5];
        };
        node157: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=175,963");
            gates:
                in[6];
                out[6];
        };
        node158: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=541,194");
            gates:
                in[10];
                out[10];
        };
        node159: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=795,879");
            gates:
                in[8];
                out[8];
        };
        node160: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=622,443");
            gates:
                in[14];
                out[14];
        };
        node161: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=793,492");
            gates:
                in[3];
                out[3];
        };
        node162: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=749,243");
            gates:
                in[6];
                out[6];
        };
        node163: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=781,189");
            gates:
                in[3];
                out[3];
        };
        node164: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=538,532");
            gates:
                in[2];
                out[2];
        };
        node165: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=50,106");
            gates:
                in[2];
                out[2];
        };
        node166: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=822,648");
            gates:
                in[6];
                out[6];
        };
        node167: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=628,774");
            gates:
                in[3];
                out[3];
        };
        node168: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=479,362");
            gates:
                in[7];
                out[7];
        };
        node169: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=189,154");
            gates:
                in[3];
                out[3];
        };
        node170: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=907,17");
            gates:
                in[2];
                out[2];
        };
        node171: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=654,3");
            gates:
                in[3];
                out[3];
        };
        node172: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=194,957");
            gates:
                in[2];
                out[2];
        };
        node173: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=468,282");
            gates:
                in[2];
                out[2];
        };
        node174: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=261,413");
            gates:
                in[2];
                out[2];
        };
        node175: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=100,761");
            gates:
                in[2];
                out[2];
        };
        node176: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=377,146");
            gates:
                in[2];
                out[2];
        };
        node177: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=245,72");
            gates:
                in[4];
                out[4];
        };
        node178: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=478,450");
            gates:
                in[4];
                out[4];
        };
        node179: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=603,184");
            gates:
                in[3];
                out[3];
        };
        node180: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=693,558");
            gates:
                in[3];
                out[3];
        };
        node181: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=852,712");
            gates:
                in[4];
                out[4];
        };
        node182: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=278,106");
            gates:
                in[4];
                out[4];
        };
        node183: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=628,495");
            gates:
                in[6];
                out[6];
        };
        node184: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=966,69");
            gates:
                in[4];
                out[4];
        };
        node185: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=906,635");
            gates:
                in[2];
                out[2];
        };
        node186: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=371,514");
            gates:
                in[2];
                out[2];
        };
        node187: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=147,511");
            gates:
                in[2];
                out[2];
        };
        node188: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=648,564");
            gates:
                in[2];
                out[2];
        };
        node189: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=3,359");
            gates:
                in[3];
                out[3];
        };
        node190: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=120,242");
            gates:
                in[3];
                out[3];
        };
        node191: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=136,665");
            gates:
                in[2];
                out[2];
        };
        node192: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=919,186");
            gates:
                in[3];
                out[3];
        };
        node193: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=958,961");
            gates:
                in[4];
                out[4];
        };
        node194: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=995,854");
            gates:
                in[8];
                out[8];
        };
        node195: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=670,623");
            gates:
                in[2];
                out[2];
        };
        node196: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=53,452");
            gates:
                in[5];
                out[5];
        };
        node197: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=29,913");
            gates:
                in[2];
                out[2];
        };
        node198: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=47,747");
            gates:
                in[8];
                out[8];
        };
        node199: BRITE200Node {
            parameters:
                @display("i=router_s;ppp=688,819");
            gates:
                in[4];
                out[4];
        };
    connections:
        node0.out[0] --> {  delay = 1.222729684075449ms; datarate = 1.0E7; } --> node35.in[0];
        node0.in[0] <-- {  delay = 1.222729684075449ms; datarate = 1.0E7; } <-- node35.out[0];
        node0.out[1] --> {  delay = 2.08697595683848ms; datarate = 1.0E7; } --> node153.in[0];
        node0.in[1] <-- {  delay = 2.08697595683848ms; datarate = 1.0E7; } <-- node153.out[0];
        node1.out[0] --> {  delay = 1.5260803425205078ms; datarate = 1.0E7; } --> node146.in[0];
        node1.in[0] <-- {  delay = 1.5260803425205078ms; datarate = 1.0E7; } <-- node146.out[0];
        node1.out[1] --> {  delay = 0.47084554120080985ms; datarate = 1.0E7; } --> node41.in[0];
        node1.in[1] <-- {  delay = 0.47084554120080985ms; datarate = 1.0E7; } <-- node41.out[0];
        node2.out[0] --> {  delay = 0.6579673322513073ms; datarate = 1.0E7; } --> node143.in[0];
        node2.in[0] <-- {  delay = 0.6579673322513073ms; datarate = 1.0E7; } <-- node143.out[0];
        node2.out[1] --> {  delay = 0.5572417587039253ms; datarate = 1.0E7; } --> node177.in[0];
        node2.in[1] <-- {  delay = 0.5572417587039253ms; datarate = 1.0E7; } <-- node177.out[0];
        node3.out[0] --> {  delay = 1.3798833432303461ms; datarate = 1.0E7; } --> node83.in[0];
        node3.in[0] <-- {  delay = 1.3798833432303461ms; datarate = 1.0E7; } <-- node83.out[0];
        node3.out[1] --> {  delay = 1.6539734540846747ms; datarate = 1.0E7; } --> node160.in[0];
        node3.in[1] <-- {  delay = 1.6539734540846747ms; datarate = 1.0E7; } <-- node160.out[0];
        node4.out[0] --> {  delay = 1.2029877064653236ms; datarate = 1.0E7; } --> node139.in[0];
        node4.in[0] <-- {  delay = 1.2029877064653236ms; datarate = 1.0E7; } <-- node139.out[0];
        node4.out[1] --> {  delay = 2.2185122033589586ms; datarate = 1.0E7; } --> node115.in[0];
        node4.in[1] <-- {  delay = 2.2185122033589586ms; datarate = 1.0E7; } <-- node115.out[0];
        node5.out[0] --> {  delay = 1.3304859486968006ms; datarate = 1.0E7; } --> node58.in[0];
        node5.in[0] <-- {  delay = 1.3304859486968006ms; datarate = 1.0E7; } <-- node58.out[0];
        node5.out[1] --> {  delay = 2.2399816250976063ms; datarate = 1.0E7; } --> node95.in[0];
        node5.in[1] <-- {  delay = 2.2399816250976063ms; datarate = 1.0E7; } <-- node95.out[0];
        node6.out[0] --> {  delay = 1.6112837701020861ms; datarate = 1.0E7; } --> node86.in[0];
        node6.in[0] <-- {  delay = 1.6112837701020861ms; datarate = 1.0E7; } <-- node86.out[0];
        node6.out[1] --> {  delay = 1.7246650828057195ms; datarate = 1.0E7; } --> node166.in[0];
        node6.in[1] <-- {  delay = 1.7246650828057195ms; datarate = 1.0E7; } <-- node166.out[0];
        node7.out[0] --> {  delay = 0.8598710457090356ms; datarate = 1.0E7; } --> node38.in[0];
        node7.in[0] <-- {  delay = 0.8598710457090356ms; datarate = 1.0E7; } <-- node38.out[0];
        node7.out[1] --> {  delay = 0.9102389050355555ms; datarate = 1.0E7; } --> node104.in[0];
        node7.in[1] <-- {  delay = 0.9102389050355555ms; datarate = 1.0E7; } <-- node104.out[0];
        node8.out[0] --> {  delay = 1.6822480966162559ms; datarate = 1.0E7; } --> node137.in[0];
        node8.in[0] <-- {  delay = 1.6822480966162559ms; datarate = 1.0E7; } <-- node137.out[0];
        node8.out[1] --> {  delay = 2.879776890627905ms; datarate = 1.0E7; } --> node23.in[0];
        node8.in[1] <-- {  delay = 2.879776890627905ms; datarate = 1.0E7; } <-- node23.out[0];
        node9.out[0] --> {  delay = 1.2083356471015663ms; datarate = 1.0E7; } --> node26.in[0];
        node9.in[0] <-- {  delay = 1.2083356471015663ms; datarate = 1.0E7; } <-- node26.out[0];
        node9.out[1] --> {  delay = 2.2252025539151203ms; datarate = 1.0E7; } --> node78.in[0];
        node9.in[1] <-- {  delay = 2.2252025539151203ms; datarate = 1.0E7; } <-- node78.out[0];
        node10.out[0] --> {  delay = 0.5689883889939419ms; datarate = 1.0E7; } --> node109.in[0];
        node10.in[0] <-- {  delay = 0.5689883889939419ms; datarate = 1.0E7; } <-- node109.out[0];
        node10.out[1] --> {  delay = 1.6400671888703406ms; datarate = 1.0E7; } --> node66.in[0];
        node10.in[1] <-- {  delay = 1.6400671888703406ms; datarate = 1.0E7; } <-- node66.out[0];
        node11.out[0] --> {  delay = 0.15744790646075488ms; datarate = 1.0E7; } --> node160.in[1];
        node11.in[0] <-- {  delay = 0.15744790646075488ms; datarate = 1.0E7; } <-- node160.out[1];
        node11.out[1] --> {  delay = 0.5745441833575443ms; datarate = 1.0E7; } --> node52.in[0];
        node11.in[1] <-- {  delay = 0.5745441833575443ms; datarate = 1.0E7; } <-- node52.out[0];
        node12.out[0] --> {  delay = 1.4197139956844305ms; datarate = 1.0E7; } --> node161.in[0];
        node12.in[0] <-- {  delay = 1.4197139956844305ms; datarate = 1.0E7; } <-- node161.out[0];
        node12.out[1] --> {  delay = 1.031754241391204ms; datarate = 1.0E7; } --> node171.in[0];
        node12.in[1] <-- {  delay = 1.031754241391204ms; datarate = 1.0E7; } <-- node171.out[0];
        node13.out[0] --> {  delay = 1.6562053565874568ms; datarate = 1.0E7; } --> node114.in[0];
        node13.in[0] <-- {  delay = 1.6562053565874568ms; datarate = 1.0E7; } <-- node114.out[0];
        node13.out[1] --> {  delay = 2.5202660313925294ms; datarate = 1.0E7; } --> node166.in[1];
        node13.in[1] <-- {  delay = 2.5202660313925294ms; datarate = 1.0E7; } <-- node166.out[1];
        node14.out[0] --> {  delay = 1.5270242222940307ms; datarate = 1.0E7; } --> node137.in[1];
        node14.in[0] <-- {  delay = 1.5270242222940307ms; datarate = 1.0E7; } <-- node137.out[1];
        node14.out[1] --> {  delay = 1.5538216144821346ms; datarate = 1.0E7; } --> node144.in[0];
        node14.in[1] <-- {  delay = 1.5538216144821346ms; datarate = 1.0E7; } <-- node144.out[0];
        node15.out[0] --> {  delay = 0.40744004266051204ms; datarate = 1.0E7; } --> node96.in[0];
        node15.in[0] <-- {  delay = 0.40744004266051204ms; datarate = 1.0E7; } <-- node96.out[0];
        node15.out[1] --> {  delay = 1.127194960292721ms; datarate = 1.0E7; } --> node159.in[0];
        node15.in[1] <-- {  delay = 1.127194960292721ms; datarate = 1.0E7; } <-- node159.out[0];
        node16.out[0] --> {  delay = 1.0752411574827083ms; datarate = 1.0E7; } --> node79.in[0];
        node16.in[0] <-- {  delay = 1.0752411574827083ms; datarate = 1.0E7; } <-- node79.out[0];
        node16.out[1] --> {  delay = 2.29913030063558ms; datarate = 1.0E7; } --> node36.in[0];
        node16.in[1] <-- {  delay = 2.29913030063558ms; datarate = 1.0E7; } <-- node36.out[0];
        node17.out[0] --> {  delay = 0.8867850247478336ms; datarate = 1.0E7; } --> node162.in[0];
        node17.in[0] <-- {  delay = 0.8867850247478336ms; datarate = 1.0E7; } <-- node162.out[0];
        node17.out[1] --> {  delay = 1.679023956655489ms; datarate = 1.0E7; } --> node25.in[0];
        node17.in[1] <-- {  delay = 1.679023956655489ms; datarate = 1.0E7; } <-- node25.out[0];
        node18.out[0] --> {  delay = 2.9646474216647127ms; datarate = 1.0E7; } --> node36.in[1];
        node18.in[0] <-- {  delay = 2.9646474216647127ms; datarate = 1.0E7; } <-- node36.out[1];
        node18.out[1] --> {  delay = 0.9939146604549131ms; datarate = 1.0E7; } --> node26.in[1];
        node18.in[1] <-- {  delay = 0.9939146604549131ms; datarate = 1.0E7; } <-- node26.out[1];
        node19.out[0] --> {  delay = 0.21056852740553772ms; datarate = 1.0E7; } --> node198.in[0];
        node19.in[0] <-- {  delay = 0.21056852740553772ms; datarate = 1.0E7; } <-- node198.out[0];
        node19.out[1] --> {  delay = 0.4681795179656655ms; datarate = 1.0E7; } --> node87.in[0];
        node19.in[1] <-- {  delay = 0.4681795179656655ms; datarate = 1.0E7; } <-- node87.out[0];
        node20.out[0] --> {  delay = 1.3738346587509846ms; datarate = 1.0E7; } --> node87.in[1];
        node20.in[0] <-- {  delay = 1.3738346587509846ms; datarate = 1.0E7; } <-- node87.out[1];
        node20.out[1] --> {  delay = 1.5924847378421592ms; datarate = 1.0E7; } --> node99.in[0];
        node20.in[1] <-- {  delay = 1.5924847378421592ms; datarate = 1.0E7; } <-- node99.out[0];
        node21.out[0] --> {  delay = 0.42067444825580746ms; datarate = 1.0E7; } --> node132.in[0];
        node21.in[0] <-- {  delay = 0.42067444825580746ms; datarate = 1.0E7; } <-- node132.out[0];
        node21.out[1] --> {  delay = 1.2587747289826712ms; datarate = 1.0E7; } --> node37.in[0];
        node21.in[1] <-- {  delay = 1.2587747289826712ms; datarate = 1.0E7; } <-- node37.out[0];
        node22.out[0] --> {  delay = 0.27607341204772484ms; datarate = 1.0E7; } --> node182.in[0];
        node22.in[0] <-- {  delay = 0.27607341204772484ms; datarate = 1.0E7; } <-- node182.out[0];
        node22.out[1] --> {  delay = 0.09481673087020685ms; datarate = 1.0E7; } --> node153.in[1];
        node22.in[1] <-- {  delay = 0.09481673087020685ms; datarate = 1.0E7; } <-- node153.out[1];
        node23.out[1] --> {  delay = 0.22770483660234056ms; datarate = 1.0E7; } --> node157.in[0];
        node23.in[1] <-- {  delay = 0.22770483660234056ms; datarate = 1.0E7; } <-- node157.out[0];
        node23.out[2] --> {  delay = 1.4000833865358104ms; datarate = 1.0E7; } --> node32.in[0];
        node23.in[2] <-- {  delay = 1.4000833865358104ms; datarate = 1.0E7; } <-- node32.out[0];
        node24.out[0] --> {  delay = 1.2599939414457935ms; datarate = 1.0E7; } --> node23.in[3];
        node24.in[0] <-- {  delay = 1.2599939414457935ms; datarate = 1.0E7; } <-- node23.out[3];
        node24.out[1] --> {  delay = 2.5242847210670085ms; datarate = 1.0E7; } --> node29.in[0];
        node24.in[1] <-- {  delay = 2.5242847210670085ms; datarate = 1.0E7; } <-- node29.out[0];
        node25.out[1] --> {  delay = 2.1103459132008613ms; datarate = 1.0E7; } --> node38.in[1];
        node25.in[1] <-- {  delay = 2.1103459132008613ms; datarate = 1.0E7; } <-- node38.out[1];
        node25.out[2] --> {  delay = 0.3723537612067876ms; datarate = 1.0E7; } --> node146.in[1];
        node25.in[2] <-- {  delay = 0.3723537612067876ms; datarate = 1.0E7; } <-- node146.out[1];
        node26.out[2] --> {  delay = 1.051917525638452ms; datarate = 1.0E7; } --> node24.in[2];
        node26.in[2] <-- {  delay = 1.051917525638452ms; datarate = 1.0E7; } <-- node24.out[2];
        node26.out[3] --> {  delay = 1.6876958803332727ms; datarate = 1.0E7; } --> node160.in[2];
        node26.in[3] <-- {  delay = 1.6876958803332727ms; datarate = 1.0E7; } <-- node160.out[2];
        node27.out[0] --> {  delay = 0.8767216843006455ms; datarate = 1.0E7; } --> node145.in[0];
        node27.in[0] <-- {  delay = 0.8767216843006455ms; datarate = 1.0E7; } <-- node145.out[0];
        node27.out[1] --> {  delay = 1.3556046470408387ms; datarate = 1.0E7; } --> node85.in[0];
        node27.in[1] <-- {  delay = 1.3556046470408387ms; datarate = 1.0E7; } <-- node85.out[0];
        node28.out[0] --> {  delay = 1.3444660754602877ms; datarate = 1.0E7; } --> node29.in[1];
        node28.in[0] <-- {  delay = 1.3444660754602877ms; datarate = 1.0E7; } <-- node29.out[1];
        node28.out[1] --> {  delay = 1.3763066378637874ms; datarate = 1.0E7; } --> node34.in[0];
        node28.in[1] <-- {  delay = 1.3763066378637874ms; datarate = 1.0E7; } <-- node34.out[0];
        node29.out[2] --> {  delay = 2.245153989026951ms; datarate = 1.0E7; } --> node30.in[0];
        node29.in[2] <-- {  delay = 2.245153989026951ms; datarate = 1.0E7; } <-- node30.out[0];
        node29.out[3] --> {  delay = 1.3943218577295875ms; datarate = 1.0E7; } --> node158.in[0];
        node29.in[3] <-- {  delay = 1.3943218577295875ms; datarate = 1.0E7; } <-- node158.out[0];
        node30.out[1] --> {  delay = 0.22608658424396444ms; datarate = 1.0E7; } --> node121.in[0];
        node30.in[1] <-- {  delay = 0.22608658424396444ms; datarate = 1.0E7; } <-- node121.out[0];
        node30.out[2] --> {  delay = 0.9429957854087218ms; datarate = 1.0E7; } --> node125.in[0];
        node30.in[2] <-- {  delay = 0.9429957854087218ms; datarate = 1.0E7; } <-- node125.out[0];
        node31.out[0] --> {  delay = 1.138133206288975ms; datarate = 1.0E7; } --> node158.in[1];
        node31.in[0] <-- {  delay = 1.138133206288975ms; datarate = 1.0E7; } <-- node158.out[1];
        node31.out[1] --> {  delay = 2.6608401810674738ms; datarate = 1.0E7; } --> node30.in[3];
        node31.in[1] <-- {  delay = 2.6608401810674738ms; datarate = 1.0E7; } <-- node30.out[3];
        node32.out[1] --> {  delay = 1.2486975356556207ms; datarate = 1.0E7; } --> node158.in[2];
        node32.in[1] <-- {  delay = 1.2486975356556207ms; datarate = 1.0E7; } <-- node158.out[2];
        node32.out[2] --> {  delay = 2.028072441928692ms; datarate = 1.0E7; } --> node95.in[1];
        node32.in[2] <-- {  delay = 2.028072441928692ms; datarate = 1.0E7; } <-- node95.out[1];
        node33.out[0] --> {  delay = 0.09557644563489588ms; datarate = 1.0E7; } --> node194.in[0];
        node33.in[0] <-- {  delay = 0.09557644563489588ms; datarate = 1.0E7; } <-- node194.out[0];
        node33.out[1] --> {  delay = 0.5848892085123862ms; datarate = 1.0E7; } --> node159.in[1];
        node33.in[1] <-- {  delay = 0.5848892085123862ms; datarate = 1.0E7; } <-- node159.out[1];
        node34.out[1] --> {  delay = 0.5440777034698859ms; datarate = 1.0E7; } --> node86.in[1];
        node34.in[1] <-- {  delay = 0.5440777034698859ms; datarate = 1.0E7; } <-- node86.out[1];
        node34.out[2] --> {  delay = 1.5772609192296512ms; datarate = 1.0E7; } --> node158.in[3];
        node34.in[2] <-- {  delay = 1.5772609192296512ms; datarate = 1.0E7; } <-- node158.out[3];
        node35.out[1] --> {  delay = 1.7324183639351063ms; datarate = 1.0E7; } --> node168.in[0];
        node35.in[1] <-- {  delay = 1.7324183639351063ms; datarate = 1.0E7; } <-- node168.out[0];
        node35.out[2] --> {  delay = 1.6744153420655425ms; datarate = 1.0E7; } --> node28.in[2];
        node35.in[2] <-- {  delay = 1.6744153420655425ms; datarate = 1.0E7; } <-- node28.out[2];
        node36.out[2] --> {  delay = 0.7345229827448557ms; datarate = 1.0E7; } --> node85.in[1];
        node36.in[2] <-- {  delay = 0.7345229827448557ms; datarate = 1.0E7; } <-- node85.out[1];
        node36.out[3] --> {  delay = 1.3160910765446097ms; datarate = 1.0E7; } --> node100.in[0];
        node36.in[3] <-- {  delay = 1.3160910765446097ms; datarate = 1.0E7; } <-- node100.out[0];
        node37.out[1] --> {  delay = 1.1067292748030408ms; datarate = 1.0E7; } --> node82.in[0];
        node37.in[1] <-- {  delay = 1.1067292748030408ms; datarate = 1.0E7; } <-- node82.out[0];
        node37.out[2] --> {  delay = 0.9800769323839271ms; datarate = 1.0E7; } --> node94.in[0];
        node37.in[2] <-- {  delay = 0.9800769323839271ms; datarate = 1.0E7; } <-- node94.out[0];
        node38.out[2] --> {  delay = 0.6795544711533387ms; datarate = 1.0E7; } --> node34.in[3];
        node38.in[2] <-- {  delay = 0.6795544711533387ms; datarate = 1.0E7; } <-- node34.out[3];
        node38.out[3] --> {  delay = 0.8165850712527957ms; datarate = 1.0E7; } --> node96.in[1];
        node38.in[3] <-- {  delay = 0.8165850712527957ms; datarate = 1.0E7; } <-- node96.out[1];
        node39.out[0] --> {  delay = 0.8015608366606755ms; datarate = 1.0E7; } --> node179.in[0];
        node39.in[0] <-- {  delay = 0.8015608366606755ms; datarate = 1.0E7; } <-- node179.out[0];
        node39.out[1] --> {  delay = 3.1846637849924986ms; datarate = 1.0E7; } --> node87.in[2];
        node39.in[1] <-- {  delay = 3.1846637849924986ms; datarate = 1.0E7; } <-- node87.out[2];
        node40.out[0] --> {  delay = 0.3772962339747111ms; datarate = 1.0E7; } --> node25.in[3];
        node40.in[0] <-- {  delay = 0.3772962339747111ms; datarate = 1.0E7; } <-- node25.out[3];
        node40.out[1] --> {  delay = 2.094878123205902ms; datarate = 1.0E7; } --> node31.in[2];
        node40.in[1] <-- {  delay = 2.094878123205902ms; datarate = 1.0E7; } <-- node31.out[2];
        node41.out[1] --> {  delay = 1.5418096006830921ms; datarate = 1.0E7; } --> node149.in[0];
        node41.in[1] <-- {  delay = 1.5418096006830921ms; datarate = 1.0E7; } <-- node149.out[0];
        node41.out[2] --> {  delay = 0.6642952195608709ms; datarate = 1.0E7; } --> node196.in[0];
        node41.in[2] <-- {  delay = 0.6642952195608709ms; datarate = 1.0E7; } <-- node196.out[0];
        node42.out[0] --> {  delay = 1.7885133209315687ms; datarate = 1.0E7; } --> node24.in[3];
        node42.in[0] <-- {  delay = 1.7885133209315687ms; datarate = 1.0E7; } <-- node24.out[3];
        node42.out[1] --> {  delay = 1.1922058606237829ms; datarate = 1.0E7; } --> node162.in[1];
        node42.in[1] <-- {  delay = 1.1922058606237829ms; datarate = 1.0E7; } <-- node162.out[1];
        node43.out[0] --> {  delay = 0.4690935957628543ms; datarate = 1.0E7; } --> node194.in[1];
        node43.in[0] <-- {  delay = 0.4690935957628543ms; datarate = 1.0E7; } <-- node194.out[1];
        node43.out[1] --> {  delay = 0.5578603953304062ms; datarate = 1.0E7; } --> node81.in[0];
        node43.in[1] <-- {  delay = 0.5578603953304062ms; datarate = 1.0E7; } <-- node81.out[0];
        node44.out[0] --> {  delay = 0.22311423869363178ms; datarate = 1.0E7; } --> node25.in[4];
        node44.in[0] <-- {  delay = 0.22311423869363178ms; datarate = 1.0E7; } <-- node25.out[4];
        node44.out[1] --> {  delay = 1.6016773942462523ms; datarate = 1.0E7; } --> node31.in[3];
        node44.in[1] <-- {  delay = 1.6016773942462523ms; datarate = 1.0E7; } <-- node31.out[3];
        node45.out[0] --> {  delay = 0.22831481582698923ms; datarate = 1.0E7; } --> node4.in[2];
        node45.in[0] <-- {  delay = 0.22831481582698923ms; datarate = 1.0E7; } <-- node4.out[2];
        node45.out[1] --> {  delay = 1.7557833022060318ms; datarate = 1.0E7; } --> node0.in[2];
        node45.in[1] <-- {  delay = 1.7557833022060318ms; datarate = 1.0E7; } <-- node0.out[2];
        node46.out[0] --> {  delay = 0.7954577049580526ms; datarate = 1.0E7; } --> node82.in[1];
        node46.in[0] <-- {  delay = 0.7954577049580526ms; datarate = 1.0E7; } <-- node82.out[1];
        node46.out[1] --> {  delay = 1.8229392245527827ms; datarate = 1.0E7; } --> node85.in[2];
        node46.in[1] <-- {  delay = 1.8229392245527827ms; datarate = 1.0E7; } <-- node85.out[2];
        node47.out[0] --> {  delay = 0.523610638081554ms; datarate = 1.0E7; } --> node115.in[1];
        node47.in[0] <-- {  delay = 0.523610638081554ms; datarate = 1.0E7; } <-- node115.out[1];
        node47.out[1] --> {  delay = 1.3053271300094968ms; datarate = 1.0E7; } --> node156.in[0];
        node47.in[1] <-- {  delay = 1.3053271300094968ms; datarate = 1.0E7; } <-- node156.out[0];
        node48.out[0] --> {  delay = 0.7229958480752084ms; datarate = 1.0E7; } --> node86.in[2];
        node48.in[0] <-- {  delay = 0.7229958480752084ms; datarate = 1.0E7; } <-- node86.out[2];
        node48.out[1] --> {  delay = 0.5331706695056837ms; datarate = 1.0E7; } --> node88.in[0];
        node48.in[1] <-- {  delay = 0.5331706695056837ms; datarate = 1.0E7; } <-- node88.out[0];
        node49.out[0] --> {  delay = 2.138835243645992ms; datarate = 1.0E7; } --> node77.in[0];
        node49.in[0] <-- {  delay = 2.138835243645992ms; datarate = 1.0E7; } <-- node77.out[0];
        node49.out[1] --> {  delay = 0.7766028347127403ms; datarate = 1.0E7; } --> node51.in[0];
        node49.in[1] <-- {  delay = 0.7766028347127403ms; datarate = 1.0E7; } <-- node51.out[0];
        node50.out[0] --> {  delay = 3.0889566114133054ms; datarate = 1.0E7; } --> node56.in[0];
        node50.in[0] <-- {  delay = 3.0889566114133054ms; datarate = 1.0E7; } <-- node56.out[0];
        node50.out[1] --> {  delay = 1.51573594937981ms; datarate = 1.0E7; } --> node82.in[2];
        node50.in[1] <-- {  delay = 1.51573594937981ms; datarate = 1.0E7; } <-- node82.out[2];
        node51.out[1] --> {  delay = 1.1469221982198228ms; datarate = 1.0E7; } --> node99.in[1];
        node51.in[1] <-- {  delay = 1.1469221982198228ms; datarate = 1.0E7; } <-- node99.out[1];
        node51.out[2] --> {  delay = 1.7443616275851497ms; datarate = 1.0E7; } --> node100.in[1];
        node51.in[2] <-- {  delay = 1.7443616275851497ms; datarate = 1.0E7; } <-- node100.out[1];
        node52.out[1] --> {  delay = 0.6271803363599945ms; datarate = 1.0E7; } --> node51.in[3];
        node52.in[1] <-- {  delay = 0.6271803363599945ms; datarate = 1.0E7; } <-- node51.out[3];
        node52.out[2] --> {  delay = 0.4669897332774129ms; datarate = 1.0E7; } --> node160.in[3];
        node52.in[2] <-- {  delay = 0.4669897332774129ms; datarate = 1.0E7; } <-- node160.out[3];
        node53.out[0] --> {  delay = 0.4511385022953903ms; datarate = 1.0E7; } --> node121.in[1];
        node53.in[0] <-- {  delay = 0.4511385022953903ms; datarate = 1.0E7; } <-- node121.out[1];
        node53.out[1] --> {  delay = 1.2652108556062032ms; datarate = 1.0E7; } --> node96.in[2];
        node53.in[1] <-- {  delay = 1.2652108556062032ms; datarate = 1.0E7; } <-- node96.out[2];
        node54.out[0] --> {  delay = 1.4505589938028844ms; datarate = 1.0E7; } --> node43.in[2];
        node54.in[0] <-- {  delay = 1.4505589938028844ms; datarate = 1.0E7; } <-- node43.out[2];
        node54.out[1] --> {  delay = 0.8143337358896269ms; datarate = 1.0E7; } --> node183.in[0];
        node54.in[1] <-- {  delay = 0.8143337358896269ms; datarate = 1.0E7; } <-- node183.out[0];
        node55.out[0] --> {  delay = 0.2648633346353939ms; datarate = 1.0E7; } --> node38.in[4];
        node55.in[0] <-- {  delay = 0.2648633346353939ms; datarate = 1.0E7; } <-- node38.out[4];
        node55.out[1] --> {  delay = 1.395590079543681ms; datarate = 1.0E7; } --> node6.in[2];
        node55.in[1] <-- {  delay = 1.395590079543681ms; datarate = 1.0E7; } <-- node6.out[2];
        node56.out[1] --> {  delay = 0.5171534530229961ms; datarate = 1.0E7; } --> node163.in[0];
        node56.in[1] <-- {  delay = 0.5171534530229961ms; datarate = 1.0E7; } <-- node163.out[0];
        node56.out[2] --> {  delay = 1.6045681195282397ms; datarate = 1.0E7; } --> node36.in[4];
        node56.in[2] <-- {  delay = 1.6045681195282397ms; datarate = 1.0E7; } <-- node36.out[4];
        node57.out[0] --> {  delay = 0.6703310563723374ms; datarate = 1.0E7; } --> node121.in[2];
        node57.in[0] <-- {  delay = 0.6703310563723374ms; datarate = 1.0E7; } <-- node121.out[2];
        node57.out[1] --> {  delay = 2.326319620158931ms; datarate = 1.0E7; } --> node43.in[3];
        node57.in[1] <-- {  delay = 2.326319620158931ms; datarate = 1.0E7; } <-- node43.out[3];
        node58.out[1] --> {  delay = 2.7478576096468075ms; datarate = 1.0E7; } --> node118.in[0];
        node58.in[1] <-- {  delay = 2.7478576096468075ms; datarate = 1.0E7; } <-- node118.out[0];
        node58.out[2] --> {  delay = 1.3049562869269082ms; datarate = 1.0E7; } --> node40.in[2];
        node58.in[2] <-- {  delay = 1.3049562869269082ms; datarate = 1.0E7; } <-- node40.out[2];
        node59.out[0] --> {  delay = 1.0636145062906748ms; datarate = 1.0E7; } --> node46.in[2];
        node59.in[0] <-- {  delay = 1.0636145062906748ms; datarate = 1.0E7; } <-- node46.out[2];
        node59.out[1] --> {  delay = 0.4936861298219194ms; datarate = 1.0E7; } --> node101.in[0];
        node59.in[1] <-- {  delay = 0.4936861298219194ms; datarate = 1.0E7; } <-- node101.out[0];
        node60.out[0] --> {  delay = 0.11579047100450927ms; datarate = 1.0E7; } --> node113.in[0];
        node60.in[0] <-- {  delay = 0.11579047100450927ms; datarate = 1.0E7; } <-- node113.out[0];
        node60.out[1] --> {  delay = 0.5269996019842365ms; datarate = 1.0E7; } --> node137.in[2];
        node60.in[1] <-- {  delay = 0.5269996019842365ms; datarate = 1.0E7; } <-- node137.out[2];
        node61.out[0] --> {  delay = 0.6204112832558647ms; datarate = 1.0E7; } --> node46.in[3];
        node61.in[0] <-- {  delay = 0.6204112832558647ms; datarate = 1.0E7; } <-- node46.out[3];
        node61.out[1] --> {  delay = 1.7433152312785132ms; datarate = 1.0E7; } --> node38.in[5];
        node61.in[1] <-- {  delay = 1.7433152312785132ms; datarate = 1.0E7; } <-- node38.out[5];
        node62.out[0] --> {  delay = 1.7988477976241835ms; datarate = 1.0E7; } --> node74.in[0];
        node62.in[0] <-- {  delay = 1.7988477976241835ms; datarate = 1.0E7; } <-- node74.out[0];
        node62.out[1] --> {  delay = 1.6311318361912177ms; datarate = 1.0E7; } --> node101.in[1];
        node62.in[1] <-- {  delay = 1.6311318361912177ms; datarate = 1.0E7; } <-- node101.out[1];
        node63.out[0] --> {  delay = 0.6774061825209697ms; datarate = 1.0E7; } --> node184.in[0];
        node63.in[0] <-- {  delay = 0.6774061825209697ms; datarate = 1.0E7; } <-- node184.out[0];
        node63.out[1] --> {  delay = 1.8864782208620745ms; datarate = 1.0E7; } --> node61.in[2];
        node63.in[1] <-- {  delay = 1.8864782208620745ms; datarate = 1.0E7; } <-- node61.out[2];
        node64.out[0] --> {  delay = 1.0529747294486371ms; datarate = 1.0E7; } --> node160.in[4];
        node64.in[0] <-- {  delay = 1.0529747294486371ms; datarate = 1.0E7; } <-- node160.out[4];
        node64.out[1] --> {  delay = 0.7480839516346334ms; datarate = 1.0E7; } --> node150.in[0];
        node64.in[1] <-- {  delay = 0.7480839516346334ms; datarate = 1.0E7; } <-- node150.out[0];
        node65.out[0] --> {  delay = 0.5912370659138052ms; datarate = 1.0E7; } --> node139.in[1];
        node65.in[0] <-- {  delay = 0.5912370659138052ms; datarate = 1.0E7; } <-- node139.out[1];
        node65.out[1] --> {  delay = 1.4398923089763316ms; datarate = 1.0E7; } --> node156.in[1];
        node65.in[1] <-- {  delay = 1.4398923089763316ms; datarate = 1.0E7; } <-- node156.out[1];
        node66.out[1] --> {  delay = 1.809787341928923ms; datarate = 1.0E7; } --> node105.in[0];
        node66.in[1] <-- {  delay = 1.809787341928923ms; datarate = 1.0E7; } <-- node105.out[0];
        node66.out[2] --> {  delay = 1.5817727569240823ms; datarate = 1.0E7; } --> node15.in[2];
        node66.in[2] <-- {  delay = 1.5817727569240823ms; datarate = 1.0E7; } <-- node15.out[2];
        node67.out[0] --> {  delay = 0.899590887720345ms; datarate = 1.0E7; } --> node87.in[3];
        node67.in[0] <-- {  delay = 0.899590887720345ms; datarate = 1.0E7; } <-- node87.out[3];
        node67.out[1] --> {  delay = 0.6688105677504473ms; datarate = 1.0E7; } --> node41.in[3];
        node67.in[1] <-- {  delay = 0.6688105677504473ms; datarate = 1.0E7; } <-- node41.out[3];
        node68.out[0] --> {  delay = 2.0288952116555374ms; datarate = 1.0E7; } --> node94.in[1];
        node68.in[0] <-- {  delay = 2.0288952116555374ms; datarate = 1.0E7; } <-- node94.out[1];
        node68.out[1] --> {  delay = 1.000742318957933ms; datarate = 1.0E7; } --> node183.in[1];
        node68.in[1] <-- {  delay = 1.000742318957933ms; datarate = 1.0E7; } <-- node183.out[1];
        node69.out[0] --> {  delay = 1.0847143316447794ms; datarate = 1.0E7; } --> node183.in[2];
        node69.in[0] <-- {  delay = 1.0847143316447794ms; datarate = 1.0E7; } <-- node183.out[2];
        node69.out[1] --> {  delay = 2.132111324841052ms; datarate = 1.0E7; } --> node81.in[1];
        node69.in[1] <-- {  delay = 2.132111324841052ms; datarate = 1.0E7; } <-- node81.out[1];
        node70.out[0] --> {  delay = 2.4262976283988924ms; datarate = 1.0E7; } --> node84.in[0];
        node70.in[0] <-- {  delay = 2.4262976283988924ms; datarate = 1.0E7; } <-- node84.out[0];
        node70.out[1] --> {  delay = 0.5481930806198869ms; datarate = 1.0E7; } --> node100.in[2];
        node70.in[1] <-- {  delay = 0.5481930806198869ms; datarate = 1.0E7; } <-- node100.out[2];
        node71.out[0] --> {  delay = 0.33264552916660756ms; datarate = 1.0E7; } --> node55.in[2];
        node71.in[0] <-- {  delay = 0.33264552916660756ms; datarate = 1.0E7; } <-- node55.out[2];
        node71.out[1] --> {  delay = 1.3176921740197112ms; datarate = 1.0E7; } --> node168.in[1];
        node71.in[1] <-- {  delay = 1.3176921740197112ms; datarate = 1.0E7; } <-- node168.out[1];
        node72.out[0] --> {  delay = 0.8794017406510334ms; datarate = 1.0E7; } --> node35.in[3];
        node72.in[0] <-- {  delay = 0.8794017406510334ms; datarate = 1.0E7; } <-- node35.out[3];
        node72.out[1] --> {  delay = 1.0244817215916437ms; datarate = 1.0E7; } --> node76.in[0];
        node72.in[1] <-- {  delay = 1.0244817215916437ms; datarate = 1.0E7; } <-- node76.out[0];
        node73.out[0] --> {  delay = 2.5273264629419065ms; datarate = 1.0E7; } --> node43.in[4];
        node73.in[0] <-- {  delay = 2.5273264629419065ms; datarate = 1.0E7; } <-- node43.out[4];
        node73.out[1] --> {  delay = 1.2203341104076395ms; datarate = 1.0E7; } --> node180.in[0];
        node73.in[1] <-- {  delay = 1.2203341104076395ms; datarate = 1.0E7; } <-- node180.out[0];
        node74.out[1] --> {  delay = 2.21822380495931ms; datarate = 1.0E7; } --> node196.in[1];
        node74.in[1] <-- {  delay = 2.21822380495931ms; datarate = 1.0E7; } <-- node196.out[1];
        node74.out[2] --> {  delay = 1.6748837494953208ms; datarate = 1.0E7; } --> node82.in[3];
        node74.in[2] <-- {  delay = 1.6748837494953208ms; datarate = 1.0E7; } <-- node82.out[3];
        node75.out[0] --> {  delay = 1.0343069060459922ms; datarate = 1.0E7; } --> node55.in[3];
        node75.in[0] <-- {  delay = 1.0343069060459922ms; datarate = 1.0E7; } <-- node55.out[3];
        node75.out[1] --> {  delay = 0.49675298633382314ms; datarate = 1.0E7; } --> node81.in[2];
        node75.in[1] <-- {  delay = 0.49675298633382314ms; datarate = 1.0E7; } <-- node81.out[2];
        node76.out[1] --> {  delay = 0.7358472326362948ms; datarate = 1.0E7; } --> node108.in[0];
        node76.in[1] <-- {  delay = 0.7358472326362948ms; datarate = 1.0E7; } <-- node108.out[0];
        node76.out[2] --> {  delay = 0.6581448676558024ms; datarate = 1.0E7; } --> node100.in[3];
        node76.in[2] <-- {  delay = 0.6581448676558024ms; datarate = 1.0E7; } <-- node100.out[3];
        node77.out[1] --> {  delay = 0.4579070899902123ms; datarate = 1.0E7; } --> node166.in[2];
        node77.in[1] <-- {  delay = 0.4579070899902123ms; datarate = 1.0E7; } <-- node166.out[2];
        node77.out[2] --> {  delay = 1.2219286461248842ms; datarate = 1.0E7; } --> node140.in[0];
        node77.in[2] <-- {  delay = 1.2219286461248842ms; datarate = 1.0E7; } <-- node140.out[0];
        node78.out[1] --> {  delay = 1.037657801129162ms; datarate = 1.0E7; } --> node32.in[3];
        node78.in[1] <-- {  delay = 1.037657801129162ms; datarate = 1.0E7; } <-- node32.out[3];
        node78.out[2] --> {  delay = 0.34920831339399205ms; datarate = 1.0E7; } --> node96.in[3];
        node78.in[2] <-- {  delay = 0.34920831339399205ms; datarate = 1.0E7; } <-- node96.out[3];
        node79.out[1] --> {  delay = 1.359784170245581ms; datarate = 1.0E7; } --> node159.in[2];
        node79.in[1] <-- {  delay = 1.359784170245581ms; datarate = 1.0E7; } <-- node159.out[2];
        node79.out[2] --> {  delay = 1.8517164139631195ms; datarate = 1.0E7; } --> node132.in[1];
        node79.in[2] <-- {  delay = 1.8517164139631195ms; datarate = 1.0E7; } <-- node132.out[1];
        node80.out[0] --> {  delay = 2.3691153571026624ms; datarate = 1.0E7; } --> node94.in[2];
        node80.in[0] <-- {  delay = 2.3691153571026624ms; datarate = 1.0E7; } <-- node94.out[2];
        node80.out[1] --> {  delay = 0.4647087791073532ms; datarate = 1.0E7; } --> node34.in[4];
        node80.in[1] <-- {  delay = 0.4647087791073532ms; datarate = 1.0E7; } <-- node34.out[4];
        node81.out[3] --> {  delay = 0.590747568832253ms; datarate = 1.0E7; } --> node194.in[2];
        node81.in[3] <-- {  delay = 0.590747568832253ms; datarate = 1.0E7; } <-- node194.out[2];
        node81.out[4] --> {  delay = 0.4985639603123939ms; datarate = 1.0E7; } --> node159.in[3];
        node81.in[4] <-- {  delay = 0.4985639603123939ms; datarate = 1.0E7; } <-- node159.out[3];
        node82.out[4] --> {  delay = 2.351759346772515ms; datarate = 1.0E7; } --> node85.in[3];
        node82.in[4] <-- {  delay = 2.351759346772515ms; datarate = 1.0E7; } <-- node85.out[3];
        node82.out[5] --> {  delay = 2.2502353521563947ms; datarate = 1.0E7; } --> node96.in[4];
        node82.in[5] <-- {  delay = 2.2502353521563947ms; datarate = 1.0E7; } <-- node96.out[4];
        node83.out[1] --> {  delay = 1.6151185889642ms; datarate = 1.0E7; } --> node145.in[1];
        node83.in[1] <-- {  delay = 1.6151185889642ms; datarate = 1.0E7; } <-- node145.out[1];
        node83.out[2] --> {  delay = 0.7372445675501398ms; datarate = 1.0E7; } --> node142.in[0];
        node83.in[2] <-- {  delay = 0.7372445675501398ms; datarate = 1.0E7; } <-- node142.out[0];
        node84.out[1] --> {  delay = 0.5097531697701958ms; datarate = 1.0E7; } --> node93.in[0];
        node84.in[1] <-- {  delay = 0.5097531697701958ms; datarate = 1.0E7; } <-- node93.out[0];
        node84.out[2] --> {  delay = 1.556257933799405ms; datarate = 1.0E7; } --> node31.in[4];
        node84.in[2] <-- {  delay = 1.556257933799405ms; datarate = 1.0E7; } <-- node31.out[4];
        node85.out[4] --> {  delay = 0.07525549006758854ms; datarate = 1.0E7; } --> node31.in[5];
        node85.in[4] <-- {  delay = 0.07525549006758854ms; datarate = 1.0E7; } <-- node31.out[5];
        node85.out[5] --> {  delay = 0.7756853555483146ms; datarate = 1.0E7; } --> node96.in[5];
        node85.in[5] <-- {  delay = 0.7756853555483146ms; datarate = 1.0E7; } <-- node96.out[5];
        node86.out[3] --> {  delay = 0.3608047346672066ms; datarate = 1.0E7; } --> node95.in[2];
        node86.in[3] <-- {  delay = 0.3608047346672066ms; datarate = 1.0E7; } <-- node95.out[2];
        node86.out[4] --> {  delay = 0.7492431733294153ms; datarate = 1.0E7; } --> node31.in[6];
        node86.in[4] <-- {  delay = 0.7492431733294153ms; datarate = 1.0E7; } <-- node31.out[6];
        node87.out[4] --> {  delay = 0.3232131088970159ms; datarate = 1.0E7; } --> node30.in[4];
        node87.in[4] <-- {  delay = 0.3232131088970159ms; datarate = 1.0E7; } <-- node30.out[4];
        node87.out[5] --> {  delay = 1.9008646850925197ms; datarate = 1.0E7; } --> node140.in[1];
        node87.in[5] <-- {  delay = 1.9008646850925197ms; datarate = 1.0E7; } <-- node140.out[1];
        node88.out[1] --> {  delay = 0.2539034151951223ms; datarate = 1.0E7; } --> node34.in[5];
        node88.in[1] <-- {  delay = 0.2539034151951223ms; datarate = 1.0E7; } <-- node34.out[5];
        node88.out[2] --> {  delay = 1.1540594627646972ms; datarate = 1.0E7; } --> node95.in[3];
        node88.in[2] <-- {  delay = 1.1540594627646972ms; datarate = 1.0E7; } <-- node95.out[3];
        node89.out[0] --> {  delay = 0.2556937959624962ms; datarate = 1.0E7; } --> node45.in[2];
        node89.in[0] <-- {  delay = 0.2556937959624962ms; datarate = 1.0E7; } <-- node45.out[2];
        node89.out[1] --> {  delay = 0.9270021670912595ms; datarate = 1.0E7; } --> node160.in[5];
        node89.in[1] <-- {  delay = 0.9270021670912595ms; datarate = 1.0E7; } <-- node160.out[5];
        node90.out[0] --> {  delay = 1.7813478252045545ms; datarate = 1.0E7; } --> node181.in[0];
        node90.in[0] <-- {  delay = 1.7813478252045545ms; datarate = 1.0E7; } <-- node181.out[0];
        node90.out[1] --> {  delay = 1.5055385043864502ms; datarate = 1.0E7; } --> node26.in[4];
        node90.in[1] <-- {  delay = 1.5055385043864502ms; datarate = 1.0E7; } <-- node26.out[4];
        node91.out[0] --> {  delay = 0.5451808961011043ms; datarate = 1.0E7; } --> node178.in[0];
        node91.in[0] <-- {  delay = 0.5451808961011043ms; datarate = 1.0E7; } <-- node178.out[0];
        node91.out[1] --> {  delay = 1.3015199106578552ms; datarate = 1.0E7; } --> node111.in[0];
        node91.in[1] <-- {  delay = 1.3015199106578552ms; datarate = 1.0E7; } <-- node111.out[0];
        node92.out[0] --> {  delay = 0.3314391950783337ms; datarate = 1.0E7; } --> node127.in[0];
        node92.in[0] <-- {  delay = 0.3314391950783337ms; datarate = 1.0E7; } <-- node127.out[0];
        node92.out[1] --> {  delay = 1.2553360217048903ms; datarate = 1.0E7; } --> node190.in[0];
        node92.in[1] <-- {  delay = 1.2553360217048903ms; datarate = 1.0E7; } <-- node190.out[0];
        node93.out[1] --> {  delay = 0.7019523326575281ms; datarate = 1.0E7; } --> node28.in[3];
        node93.in[1] <-- {  delay = 0.7019523326575281ms; datarate = 1.0E7; } <-- node28.out[3];
        node93.out[2] --> {  delay = 0.4376813454452354ms; datarate = 1.0E7; } --> node132.in[2];
        node93.in[2] <-- {  delay = 0.4376813454452354ms; datarate = 1.0E7; } <-- node132.out[2];
        node94.out[3] --> {  delay = 2.3776288661806624ms; datarate = 1.0E7; } --> node85.in[6];
        node94.in[3] <-- {  delay = 2.3776288661806624ms; datarate = 1.0E7; } <-- node85.out[6];
        node94.out[4] --> {  delay = 0.3738894930893039ms; datarate = 1.0E7; } --> node29.in[4];
        node94.in[4] <-- {  delay = 0.3738894930893039ms; datarate = 1.0E7; } <-- node29.out[4];
        node95.out[4] --> {  delay = 2.0450634863488086ms; datarate = 1.0E7; } --> node158.in[4];
        node95.in[4] <-- {  delay = 2.0450634863488086ms; datarate = 1.0E7; } <-- node158.out[4];
        node95.out[5] --> {  delay = 0.9273141839856861ms; datarate = 1.0E7; } --> node31.in[7];
        node95.in[5] <-- {  delay = 0.9273141839856861ms; datarate = 1.0E7; } <-- node31.out[7];
        node96.out[6] --> {  delay = 0.7745872553988693ms; datarate = 1.0E7; } --> node32.in[4];
        node96.in[6] <-- {  delay = 0.7745872553988693ms; datarate = 1.0E7; } <-- node32.out[4];
        node96.out[7] --> {  delay = 1.1801288383741473ms; datarate = 1.0E7; } --> node158.in[5];
        node96.in[7] <-- {  delay = 1.1801288383741473ms; datarate = 1.0E7; } <-- node158.out[5];
        node97.out[0] --> {  delay = 1.0331929104859372ms; datarate = 1.0E7; } --> node95.in[6];
        node97.in[0] <-- {  delay = 1.0331929104859372ms; datarate = 1.0E7; } <-- node95.out[6];
        node97.out[1] --> {  delay = 0.25017307139861406ms; datarate = 1.0E7; } --> node33.in[2];
        node97.in[1] <-- {  delay = 0.25017307139861406ms; datarate = 1.0E7; } <-- node33.out[2];
        node98.out[0] --> {  delay = 0.9401834060606459ms; datarate = 1.0E7; } --> node100.in[4];
        node98.in[0] <-- {  delay = 0.9401834060606459ms; datarate = 1.0E7; } <-- node100.out[4];
        node98.out[1] --> {  delay = 0.47150674194551395ms; datarate = 1.0E7; } --> node168.in[2];
        node98.in[1] <-- {  delay = 0.47150674194551395ms; datarate = 1.0E7; } <-- node168.out[2];
        node99.out[2] --> {  delay = 0.3053705964854079ms; datarate = 1.0E7; } --> node36.in[5];
        node99.in[2] <-- {  delay = 0.3053705964854079ms; datarate = 1.0E7; } <-- node36.out[5];
        node99.out[3] --> {  delay = 2.7591719493414812ms; datarate = 1.0E7; } --> node35.in[4];
        node99.in[3] <-- {  delay = 2.7591719493414812ms; datarate = 1.0E7; } <-- node35.out[4];
        node100.out[5] --> {  delay = 0.9280158384352186ms; datarate = 1.0E7; } --> node32.in[5];
        node100.in[5] <-- {  delay = 0.9280158384352186ms; datarate = 1.0E7; } <-- node32.out[5];
        node100.out[6] --> {  delay = 1.1708099741455138ms; datarate = 1.0E7; } --> node86.in[5];
        node100.in[6] <-- {  delay = 1.1708099741455138ms; datarate = 1.0E7; } <-- node86.out[5];
        node101.out[2] --> {  delay = 0.4818702524516632ms; datarate = 1.0E7; } --> node98.in[2];
        node101.in[2] <-- {  delay = 0.4818702524516632ms; datarate = 1.0E7; } <-- node98.out[2];
        node101.out[3] --> {  delay = 0.9726925766472333ms; datarate = 1.0E7; } --> node102.in[0];
        node101.in[3] <-- {  delay = 0.9726925766472333ms; datarate = 1.0E7; } <-- node102.out[0];
        node102.out[1] --> {  delay = 1.1997786283386815ms; datarate = 1.0E7; } --> node108.in[1];
        node102.in[1] <-- {  delay = 1.1997786283386815ms; datarate = 1.0E7; } <-- node108.out[1];
        node102.out[2] --> {  delay = 2.164625381573809ms; datarate = 1.0E7; } --> node157.in[1];
        node102.in[2] <-- {  delay = 2.164625381573809ms; datarate = 1.0E7; } <-- node157.out[1];
        node103.out[0] --> {  delay = 3.3904242919728294ms; datarate = 1.0E7; } --> node33.in[3];
        node103.in[0] <-- {  delay = 3.3904242919728294ms; datarate = 1.0E7; } <-- node33.out[3];
        node103.out[1] --> {  delay = 0.6884442928641067ms; datarate = 1.0E7; } --> node153.in[2];
        node103.in[1] <-- {  delay = 0.6884442928641067ms; datarate = 1.0E7; } <-- node153.out[2];
        node104.out[1] --> {  delay = 0.29722788303027237ms; datarate = 1.0E7; } --> node36.in[6];
        node104.in[1] <-- {  delay = 0.29722788303027237ms; datarate = 1.0E7; } <-- node36.out[6];
        node104.out[2] --> {  delay = 1.5961834890933995ms; datarate = 1.0E7; } --> node32.in[6];
        node104.in[2] <-- {  delay = 1.5961834890933995ms; datarate = 1.0E7; } <-- node32.out[6];
        node105.out[1] --> {  delay = 0.7668492730813914ms; datarate = 1.0E7; } --> node20.in[2];
        node105.in[1] <-- {  delay = 0.7668492730813914ms; datarate = 1.0E7; } <-- node20.out[2];
        node105.out[2] --> {  delay = 0.9667893667122338ms; datarate = 1.0E7; } --> node24.in[4];
        node105.in[2] <-- {  delay = 0.9667893667122338ms; datarate = 1.0E7; } <-- node24.out[4];
        node106.out[0] --> {  delay = 1.6361514382424276ms; datarate = 1.0E7; } --> node183.in[3];
        node106.in[0] <-- {  delay = 1.6361514382424276ms; datarate = 1.0E7; } <-- node183.out[3];
        node106.out[1] --> {  delay = 0.20372002133025602ms; datarate = 1.0E7; } --> node97.in[2];
        node106.in[1] <-- {  delay = 0.20372002133025602ms; datarate = 1.0E7; } <-- node97.out[2];
        node107.out[0] --> {  delay = 1.2333123633648775ms; datarate = 1.0E7; } --> node146.in[2];
        node107.in[0] <-- {  delay = 1.2333123633648775ms; datarate = 1.0E7; } <-- node146.out[2];
        node107.out[1] --> {  delay = 1.912451276440206ms; datarate = 1.0E7; } --> node27.in[2];
        node107.in[1] <-- {  delay = 1.912451276440206ms; datarate = 1.0E7; } <-- node27.out[2];
        node108.out[2] --> {  delay = 1.9421366626791041ms; datarate = 1.0E7; } --> node193.in[0];
        node108.in[2] <-- {  delay = 1.9421366626791041ms; datarate = 1.0E7; } <-- node193.out[0];
        node108.out[3] --> {  delay = 1.1352798973957172ms; datarate = 1.0E7; } --> node34.in[6];
        node108.in[3] <-- {  delay = 1.1352798973957172ms; datarate = 1.0E7; } <-- node34.out[6];
        node109.out[1] --> {  delay = 1.0134915649609615ms; datarate = 1.0E7; } --> node78.in[3];
        node109.in[1] <-- {  delay = 1.0134915649609615ms; datarate = 1.0E7; } <-- node78.out[3];
        node109.out[2] --> {  delay = 1.3195864747050394ms; datarate = 1.0E7; } --> node96.in[8];
        node109.in[2] <-- {  delay = 1.3195864747050394ms; datarate = 1.0E7; } <-- node96.out[8];
        node110.out[0] --> {  delay = 0.8695536778096895ms; datarate = 1.0E7; } --> node15.in[3];
        node110.in[0] <-- {  delay = 0.8695536778096895ms; datarate = 1.0E7; } <-- node15.out[3];
        node110.out[1] --> {  delay = 1.0174579788142837ms; datarate = 1.0E7; } --> node17.in[2];
        node110.in[1] <-- {  delay = 1.0174579788142837ms; datarate = 1.0E7; } <-- node17.out[2];
        node111.out[1] --> {  delay = 1.1183106290312486ms; datarate = 1.0E7; } --> node198.in[1];
        node111.in[1] <-- {  delay = 1.1183106290312486ms; datarate = 1.0E7; } <-- node198.out[1];
        node111.out[2] --> {  delay = 1.0996743669931908ms; datarate = 1.0E7; } --> node72.in[2];
        node111.in[2] <-- {  delay = 1.0996743669931908ms; datarate = 1.0E7; } <-- node72.out[2];
        node112.out[0] --> {  delay = 2.1318712586513984ms; datarate = 1.0E7; } --> node68.in[2];
        node112.in[0] <-- {  delay = 2.1318712586513984ms; datarate = 1.0E7; } <-- node68.out[2];
        node112.out[1] --> {  delay = 1.1158603966126244ms; datarate = 1.0E7; } --> node117.in[0];
        node112.in[1] <-- {  delay = 1.1158603966126244ms; datarate = 1.0E7; } <-- node117.out[0];
        node113.out[1] --> {  delay = 1.8316283599608492ms; datarate = 1.0E7; } --> node46.in[4];
        node113.in[1] <-- {  delay = 1.8316283599608492ms; datarate = 1.0E7; } <-- node46.out[4];
        node113.out[2] --> {  delay = 2.9719961883355586ms; datarate = 1.0E7; } --> node184.in[1];
        node113.in[2] <-- {  delay = 2.9719961883355586ms; datarate = 1.0E7; } <-- node184.out[1];
        node114.out[1] --> {  delay = 0.30807312408156223ms; datarate = 1.0E7; } --> node115.in[2];
        node114.in[1] <-- {  delay = 0.30807312408156223ms; datarate = 1.0E7; } <-- node115.out[2];
        node114.out[2] --> {  delay = 0.8695344841296843ms; datarate = 1.0E7; } --> node153.in[3];
        node114.in[2] <-- {  delay = 0.8695344841296843ms; datarate = 1.0E7; } <-- node153.out[3];
        node115.out[3] --> {  delay = 2.2722791609124124ms; datarate = 1.0E7; } --> node105.in[3];
        node115.in[3] <-- {  delay = 2.2722791609124124ms; datarate = 1.0E7; } <-- node105.out[3];
        node115.out[4] --> {  delay = 1.3062431297271364ms; datarate = 1.0E7; } --> node35.in[5];
        node115.in[4] <-- {  delay = 1.3062431297271364ms; datarate = 1.0E7; } <-- node35.out[5];
        node116.out[0] --> {  delay = 0.459544318471001ms; datarate = 1.0E7; } --> node168.in[3];
        node116.in[0] <-- {  delay = 0.459544318471001ms; datarate = 1.0E7; } <-- node168.out[3];
        node116.out[1] --> {  delay = 2.259194428027327ms; datarate = 1.0E7; } --> node105.in[4];
        node116.in[1] <-- {  delay = 2.259194428027327ms; datarate = 1.0E7; } <-- node105.out[4];
        node117.out[1] --> {  delay = 0.7742208759118846ms; datarate = 1.0E7; } --> node84.in[3];
        node117.in[1] <-- {  delay = 0.7742208759118846ms; datarate = 1.0E7; } <-- node84.out[3];
        node117.out[2] --> {  delay = 1.8428110634524992ms; datarate = 1.0E7; } --> node198.in[2];
        node117.in[2] <-- {  delay = 1.8428110634524992ms; datarate = 1.0E7; } <-- node198.out[2];
        node118.out[1] --> {  delay = 2.7493209894079547ms; datarate = 1.0E7; } --> node150.in[1];
        node118.in[1] <-- {  delay = 2.7493209894079547ms; datarate = 1.0E7; } <-- node150.out[1];
        node118.out[2] --> {  delay = 2.7047194348109627ms; datarate = 1.0E7; } --> node177.in[1];
        node118.in[2] <-- {  delay = 2.7047194348109627ms; datarate = 1.0E7; } <-- node177.out[1];
        node119.out[0] --> {  delay = 0.8380694171032891ms; datarate = 1.0E7; } --> node24.in[5];
        node119.in[0] <-- {  delay = 0.8380694171032891ms; datarate = 1.0E7; } <-- node24.out[5];
        node119.out[1] --> {  delay = 1.8025088910131928ms; datarate = 1.0E7; } --> node160.in[6];
        node119.in[1] <-- {  delay = 1.8025088910131928ms; datarate = 1.0E7; } <-- node160.out[6];
        node120.out[0] --> {  delay = 1.5279201859425056ms; datarate = 1.0E7; } --> node79.in[3];
        node120.in[0] <-- {  delay = 1.5279201859425056ms; datarate = 1.0E7; } <-- node79.out[3];
        node120.out[1] --> {  delay = 2.1357116843781996ms; datarate = 1.0E7; } --> node3.in[2];
        node120.in[1] <-- {  delay = 2.1357116843781996ms; datarate = 1.0E7; } <-- node3.out[2];
        node121.out[3] --> {  delay = 1.6929716964239063ms; datarate = 1.0E7; } --> node96.in[9];
        node121.in[3] <-- {  delay = 1.6929716964239063ms; datarate = 1.0E7; } <-- node96.out[9];
        node121.out[4] --> {  delay = 1.9446100156603714ms; datarate = 1.0E7; } --> node169.in[0];
        node121.in[4] <-- {  delay = 1.9446100156603714ms; datarate = 1.0E7; } <-- node169.out[0];
        node122.out[0] --> {  delay = 2.151581499668847ms; datarate = 1.0E7; } --> node157.in[2];
        node122.in[0] <-- {  delay = 2.151581499668847ms; datarate = 1.0E7; } <-- node157.out[2];
        node122.out[1] --> {  delay = 1.9342489997365837ms; datarate = 1.0E7; } --> node137.in[3];
        node122.in[1] <-- {  delay = 1.9342489997365837ms; datarate = 1.0E7; } <-- node137.out[3];
        node123.out[0] --> {  delay = 0.5501279989662262ms; datarate = 1.0E7; } --> node44.in[2];
        node123.in[0] <-- {  delay = 0.5501279989662262ms; datarate = 1.0E7; } <-- node44.out[2];
        node123.out[1] --> {  delay = 0.9539058351541885ms; datarate = 1.0E7; } --> node178.in[1];
        node123.in[1] <-- {  delay = 0.9539058351541885ms; datarate = 1.0E7; } <-- node178.out[1];
        node124.out[0] --> {  delay = 0.8413819573559411ms; datarate = 1.0E7; } --> node28.in[4];
        node124.in[0] <-- {  delay = 0.8413819573559411ms; datarate = 1.0E7; } <-- node28.out[4];
        node124.out[1] --> {  delay = 0.9791853413951235ms; datarate = 1.0E7; } --> node25.in[5];
        node124.in[1] <-- {  delay = 0.9791853413951235ms; datarate = 1.0E7; } <-- node25.out[5];
        node125.out[1] --> {  delay = 0.45873249841429475ms; datarate = 1.0E7; } --> node118.in[3];
        node125.in[1] <-- {  delay = 0.45873249841429475ms; datarate = 1.0E7; } <-- node118.out[3];
        node125.out[2] --> {  delay = 0.8072044346542946ms; datarate = 1.0E7; } --> node144.in[1];
        node125.in[2] <-- {  delay = 0.8072044346542946ms; datarate = 1.0E7; } <-- node144.out[1];
        node126.out[0] --> {  delay = 1.1213758129414588ms; datarate = 1.0E7; } --> node34.in[7];
        node126.in[0] <-- {  delay = 1.1213758129414588ms; datarate = 1.0E7; } <-- node34.out[7];
        node126.out[1] --> {  delay = 1.0041609001044562ms; datarate = 1.0E7; } --> node101.in[4];
        node126.in[1] <-- {  delay = 1.0041609001044562ms; datarate = 1.0E7; } <-- node101.out[4];
        node127.out[1] --> {  delay = 2.2477418952204884ms; datarate = 1.0E7; } --> node31.in[8];
        node127.in[1] <-- {  delay = 2.2477418952204884ms; datarate = 1.0E7; } <-- node31.out[8];
        node127.out[2] --> {  delay = 0.7538548551478236ms; datarate = 1.0E7; } --> node32.in[7];
        node127.in[2] <-- {  delay = 0.7538548551478236ms; datarate = 1.0E7; } <-- node32.out[7];
        node128.out[0] --> {  delay = 0.28564073345797836ms; datarate = 1.0E7; } --> node199.in[0];
        node128.in[0] <-- {  delay = 0.28564073345797836ms; datarate = 1.0E7; } <-- node199.out[0];
        node128.out[1] --> {  delay = 0.9140204016256199ms; datarate = 1.0E7; } --> node4.in[3];
        node128.in[1] <-- {  delay = 0.9140204016256199ms; datarate = 1.0E7; } <-- node4.out[3];
        node129.out[0] --> {  delay = 0.7465355226489205ms; datarate = 1.0E7; } --> node199.in[1];
        node129.in[0] <-- {  delay = 0.7465355226489205ms; datarate = 1.0E7; } <-- node199.out[1];
        node129.out[1] --> {  delay = 0.8243353496809575ms; datarate = 1.0E7; } --> node100.in[7];
        node129.in[1] <-- {  delay = 0.8243353496809575ms; datarate = 1.0E7; } <-- node100.out[7];
        node130.out[0] --> {  delay = 3.455034935189706ms; datarate = 1.0E7; } --> node189.in[0];
        node130.in[0] <-- {  delay = 3.455034935189706ms; datarate = 1.0E7; } <-- node189.out[0];
        node130.out[1] --> {  delay = 1.1635268379810235ms; datarate = 1.0E7; } --> node3.in[3];
        node130.in[1] <-- {  delay = 1.1635268379810235ms; datarate = 1.0E7; } <-- node3.out[3];
        node131.out[0] --> {  delay = 2.7953168732984794ms; datarate = 1.0E7; } --> node139.in[2];
        node131.in[0] <-- {  delay = 2.7953168732984794ms; datarate = 1.0E7; } <-- node139.out[2];
        node131.out[1] --> {  delay = 2.056370569117701ms; datarate = 1.0E7; } --> node79.in[4];
        node131.in[1] <-- {  delay = 2.056370569117701ms; datarate = 1.0E7; } <-- node79.out[4];
        node132.out[3] --> {  delay = 0.9536491893335547ms; datarate = 1.0E7; } --> node85.in[7];
        node132.in[3] <-- {  delay = 0.9536491893335547ms; datarate = 1.0E7; } <-- node85.out[7];
        node132.out[4] --> {  delay = 2.6300275631578867ms; datarate = 1.0E7; } --> node33.in[4];
        node132.in[4] <-- {  delay = 2.6300275631578867ms; datarate = 1.0E7; } <-- node33.out[4];
        node133.out[0] --> {  delay = 2.021401169077931ms; datarate = 1.0E7; } --> node60.in[2];
        node133.in[0] <-- {  delay = 2.021401169077931ms; datarate = 1.0E7; } <-- node60.out[2];
        node133.out[1] --> {  delay = 2.95773749100923ms; datarate = 1.0E7; } --> node181.in[1];
        node133.in[1] <-- {  delay = 2.95773749100923ms; datarate = 1.0E7; } <-- node181.out[1];
        node134.out[0] --> {  delay = 1.487987517603336ms; datarate = 1.0E7; } --> node192.in[0];
        node134.in[0] <-- {  delay = 1.487987517603336ms; datarate = 1.0E7; } <-- node192.out[0];
        node134.out[1] --> {  delay = 1.8243394640719655ms; datarate = 1.0E7; } --> node35.in[6];
        node134.in[1] <-- {  delay = 1.8243394640719655ms; datarate = 1.0E7; } <-- node35.out[6];
        node135.out[0] --> {  delay = 1.9399813628230567ms; datarate = 1.0E7; } --> node198.in[3];
        node135.in[0] <-- {  delay = 1.9399813628230567ms; datarate = 1.0E7; } <-- node198.out[3];
        node135.out[1] --> {  delay = 0.9363412116683585ms; datarate = 1.0E7; } --> node182.in[1];
        node135.in[1] <-- {  delay = 0.9363412116683585ms; datarate = 1.0E7; } <-- node182.out[1];
        node136.out[0] --> {  delay = 2.3166932948460652ms; datarate = 1.0E7; } --> node18.in[2];
        node136.in[0] <-- {  delay = 2.3166932948460652ms; datarate = 1.0E7; } <-- node18.out[2];
        node136.out[1] --> {  delay = 0.5779906394831139ms; datarate = 1.0E7; } --> node102.in[3];
        node136.in[1] <-- {  delay = 0.5779906394831139ms; datarate = 1.0E7; } <-- node102.out[3];
        node137.out[4] --> {  delay = 0.7906243112192559ms; datarate = 1.0E7; } --> node98.in[3];
        node137.in[4] <-- {  delay = 0.7906243112192559ms; datarate = 1.0E7; } <-- node98.out[3];
        node137.out[5] --> {  delay = 0.9353187192460962ms; datarate = 1.0E7; } --> node28.in[5];
        node137.in[5] <-- {  delay = 0.9353187192460962ms; datarate = 1.0E7; } <-- node28.out[5];
        node138.out[0] --> {  delay = 0.6597911293970071ms; datarate = 1.0E7; } --> node104.in[3];
        node138.in[0] <-- {  delay = 0.6597911293970071ms; datarate = 1.0E7; } <-- node104.out[3];
        node138.out[1] --> {  delay = 0.3002818018838059ms; datarate = 1.0E7; } --> node78.in[4];
        node138.in[1] <-- {  delay = 0.3002818018838059ms; datarate = 1.0E7; } <-- node78.out[4];
        node139.out[3] --> {  delay = 1.2610311098905655ms; datarate = 1.0E7; } --> node102.in[4];
        node139.in[3] <-- {  delay = 1.2610311098905655ms; datarate = 1.0E7; } <-- node102.out[4];
        node139.out[4] --> {  delay = 1.0515366720685329ms; datarate = 1.0E7; } --> node194.in[3];
        node139.in[4] <-- {  delay = 1.0515366720685329ms; datarate = 1.0E7; } <-- node194.out[3];
        node140.out[2] --> {  delay = 1.8565891076516488ms; datarate = 1.0E7; } --> node31.in[9];
        node140.in[2] <-- {  delay = 1.8565891076516488ms; datarate = 1.0E7; } <-- node31.out[9];
        node140.out[3] --> {  delay = 1.7522404809783958ms; datarate = 1.0E7; } --> node157.in[3];
        node140.in[3] <-- {  delay = 1.7522404809783958ms; datarate = 1.0E7; } <-- node157.out[3];
        node141.out[0] --> {  delay = 1.7596573078277526ms; datarate = 1.0E7; } --> node35.in[7];
        node141.in[0] <-- {  delay = 1.7596573078277526ms; datarate = 1.0E7; } <-- node35.out[7];
        node141.out[1] --> {  delay = 0.923737694363243ms; datarate = 1.0E7; } --> node98.in[4];
        node141.in[1] <-- {  delay = 0.923737694363243ms; datarate = 1.0E7; } <-- node98.out[4];
        node142.out[1] --> {  delay = 2.6891555875989934ms; datarate = 1.0E7; } --> node28.in[6];
        node142.in[1] <-- {  delay = 2.6891555875989934ms; datarate = 1.0E7; } <-- node28.out[6];
        node142.out[2] --> {  delay = 0.051890395408493044ms; datarate = 1.0E7; } --> node193.in[1];
        node142.in[2] <-- {  delay = 0.051890395408493044ms; datarate = 1.0E7; } <-- node193.out[1];
        node143.out[1] --> {  delay = 1.4630997308260425ms; datarate = 1.0E7; } --> node100.in[8];
        node143.in[1] <-- {  delay = 1.4630997308260425ms; datarate = 1.0E7; } <-- node100.out[8];
        node143.out[2] --> {  delay = 0.593715979453431ms; datarate = 1.0E7; } --> node28.in[7];
        node143.in[2] <-- {  delay = 0.593715979453431ms; datarate = 1.0E7; } <-- node28.out[7];
        node144.out[2] --> {  delay = 0.5302305311500163ms; datarate = 1.0E7; } --> node198.in[4];
        node144.in[2] <-- {  delay = 0.5302305311500163ms; datarate = 1.0E7; } <-- node198.out[4];
        node144.out[3] --> {  delay = 0.9660698044229389ms; datarate = 1.0E7; } --> node35.in[8];
        node144.in[3] <-- {  delay = 0.9660698044229389ms; datarate = 1.0E7; } <-- node35.out[8];
        node145.out[2] --> {  delay = 0.9964860975071614ms; datarate = 1.0E7; } --> node159.in[4];
        node145.in[2] <-- {  delay = 0.9964860975071614ms; datarate = 1.0E7; } <-- node159.out[4];
        node145.out[3] --> {  delay = 0.5210864800388944ms; datarate = 1.0E7; } --> node100.in[9];
        node145.in[3] <-- {  delay = 0.5210864800388944ms; datarate = 1.0E7; } <-- node100.out[9];
        node146.out[3] --> {  delay = 1.42947685568658ms; datarate = 1.0E7; } --> node85.in[8];
        node146.in[3] <-- {  delay = 1.42947685568658ms; datarate = 1.0E7; } <-- node85.out[8];
        node146.out[4] --> {  delay = 1.468322473816476ms; datarate = 1.0E7; } --> node96.in[10];
        node146.in[4] <-- {  delay = 1.468322473816476ms; datarate = 1.0E7; } <-- node96.out[10];
        node147.out[0] --> {  delay = 0.629862289700279ms; datarate = 1.0E7; } --> node126.in[2];
        node147.in[0] <-- {  delay = 0.629862289700279ms; datarate = 1.0E7; } <-- node126.out[2];
        node147.out[1] --> {  delay = 1.0144133301279066ms; datarate = 1.0E7; } --> node123.in[2];
        node147.in[1] <-- {  delay = 1.0144133301279066ms; datarate = 1.0E7; } <-- node123.out[2];
        node148.out[0] --> {  delay = 0.797574002865549ms; datarate = 1.0E7; } --> node105.in[5];
        node148.in[0] <-- {  delay = 0.797574002865549ms; datarate = 1.0E7; } <-- node105.out[5];
        node148.out[1] --> {  delay = 2.136914795224305ms; datarate = 1.0E7; } --> node114.in[3];
        node148.in[1] <-- {  delay = 2.136914795224305ms; datarate = 1.0E7; } <-- node114.out[3];
        node149.out[1] --> {  delay = 0.9059689251567258ms; datarate = 1.0E7; } --> node85.in[9];
        node149.in[1] <-- {  delay = 0.9059689251567258ms; datarate = 1.0E7; } <-- node85.out[9];
        node149.out[2] --> {  delay = 0.7944499633686284ms; datarate = 1.0E7; } --> node168.in[4];
        node149.in[2] <-- {  delay = 0.7944499633686284ms; datarate = 1.0E7; } <-- node168.out[4];
        node150.out[2] --> {  delay = 1.656201997549944ms; datarate = 1.0E7; } --> node194.in[4];
        node150.in[2] <-- {  delay = 1.656201997549944ms; datarate = 1.0E7; } <-- node194.out[4];
        node150.out[3] --> {  delay = 1.2366593611601937ms; datarate = 1.0E7; } --> node32.in[8];
        node150.in[3] <-- {  delay = 1.2366593611601937ms; datarate = 1.0E7; } <-- node32.out[8];
        node151.out[0] --> {  delay = 0.9207879610493492ms; datarate = 1.0E7; } --> node28.in[8];
        node151.in[0] <-- {  delay = 0.9207879610493492ms; datarate = 1.0E7; } <-- node28.out[8];
        node151.out[1] --> {  delay = 0.779898249544305ms; datarate = 1.0E7; } --> node149.in[3];
        node151.in[1] <-- {  delay = 0.779898249544305ms; datarate = 1.0E7; } <-- node149.out[3];
        node152.out[0] --> {  delay = 1.0057552115003066ms; datarate = 1.0E7; } --> node33.in[5];
        node152.in[0] <-- {  delay = 1.0057552115003066ms; datarate = 1.0E7; } <-- node33.out[5];
        node152.out[1] --> {  delay = 1.8705928281644357ms; datarate = 1.0E7; } --> node65.in[2];
        node152.in[1] <-- {  delay = 1.8705928281644357ms; datarate = 1.0E7; } <-- node65.out[2];
        node153.out[4] --> {  delay = 0.8023308646876184ms; datarate = 1.0E7; } --> node146.in[5];
        node153.in[4] <-- {  delay = 0.8023308646876184ms; datarate = 1.0E7; } <-- node146.out[5];
        node153.out[5] --> {  delay = 0.33770790743702744ms; datarate = 1.0E7; } --> node46.in[5];
        node153.in[5] <-- {  delay = 0.33770790743702744ms; datarate = 1.0E7; } <-- node46.out[5];
        node154.out[0] --> {  delay = 0.7373275689192412ms; datarate = 1.0E7; } --> node145.in[4];
        node154.in[0] <-- {  delay = 0.7373275689192412ms; datarate = 1.0E7; } <-- node145.out[4];
        node154.out[1] --> {  delay = 0.3818689779837938ms; datarate = 1.0E7; } --> node23.in[4];
        node154.in[1] <-- {  delay = 0.3818689779837938ms; datarate = 1.0E7; } <-- node23.out[4];
        node155.out[0] --> {  delay = 3.1151564958895475ms; datarate = 1.0E7; } --> node144.in[4];
        node155.in[0] <-- {  delay = 3.1151564958895475ms; datarate = 1.0E7; } <-- node144.out[4];
        node155.out[1] --> {  delay = 0.5743698648257044ms; datarate = 1.0E7; } --> node149.in[4];
        node155.in[1] <-- {  delay = 0.5743698648257044ms; datarate = 1.0E7; } <-- node149.out[4];
        node156.out[2] --> {  delay = 2.3656491681846834ms; datarate = 1.0E7; } --> node43.in[5];
        node156.in[2] <-- {  delay = 2.3656491681846834ms; datarate = 1.0E7; } <-- node43.out[5];
        node156.out[3] --> {  delay = 0.4261534255981895ms; datarate = 1.0E7; } --> node30.in[5];
        node156.in[3] <-- {  delay = 0.4261534255981895ms; datarate = 1.0E7; } <-- node30.out[5];
        node157.out[4] --> {  delay = 0.9038171375634817ms; datarate = 1.0E7; } --> node30.in[6];
        node157.in[4] <-- {  delay = 0.9038171375634817ms; datarate = 1.0E7; } <-- node30.out[6];
        node157.out[5] --> {  delay = 2.2873086153969604ms; datarate = 1.0E7; } --> node160.in[7];
        node157.in[5] <-- {  delay = 2.2873086153969604ms; datarate = 1.0E7; } <-- node160.out[7];
        node158.out[6] --> {  delay = 1.7167698545467385ms; datarate = 1.0E7; } --> node127.in[3];
        node158.in[6] <-- {  delay = 1.7167698545467385ms; datarate = 1.0E7; } <-- node127.out[3];
        node158.out[7] --> {  delay = 0.14401318357997453ms; datarate = 1.0E7; } --> node123.in[3];
        node158.in[7] <-- {  delay = 0.14401318357997453ms; datarate = 1.0E7; } <-- node123.out[3];
        node159.out[5] --> {  delay = 1.5988997471047384ms; datarate = 1.0E7; } --> node31.in[10];
        node159.in[5] <-- {  delay = 1.5988997471047384ms; datarate = 1.0E7; } <-- node31.out[10];
        node159.out[6] --> {  delay = 0.9099638298938607ms; datarate = 1.0E7; } --> node86.in[6];
        node159.in[6] <-- {  delay = 0.9099638298938607ms; datarate = 1.0E7; } <-- node86.out[6];
        node160.out[8] --> {  delay = 0.6530534177885848ms; datarate = 1.0E7; } --> node31.in[11];
        node160.in[8] <-- {  delay = 0.6530534177885848ms; datarate = 1.0E7; } <-- node31.out[11];
        node160.out[9] --> {  delay = 0.8734157838231927ms; datarate = 1.0E7; } --> node158.in[8];
        node160.in[9] <-- {  delay = 0.8734157838231927ms; datarate = 1.0E7; } <-- node158.out[8];
        node161.out[1] --> {  delay = 1.6271262034270806ms; datarate = 1.0E7; } --> node108.in[4];
        node161.in[1] <-- {  delay = 1.6271262034270806ms; datarate = 1.0E7; } <-- node108.out[4];
        node161.out[2] --> {  delay = 0.9477095431869417ms; datarate = 1.0E7; } --> node58.in[3];
        node161.in[2] <-- {  delay = 0.9477095431869417ms; datarate = 1.0E7; } <-- node58.out[3];
        node162.out[2] --> {  delay = 0.5260169350310062ms; datarate = 1.0E7; } --> node85.in[10];
        node162.in[2] <-- {  delay = 0.5260169350310062ms; datarate = 1.0E7; } <-- node85.out[10];
        node162.out[3] --> {  delay = 1.7062838693113582ms; datarate = 1.0E7; } --> node81.in[5];
        node162.in[3] <-- {  delay = 1.7062838693113582ms; datarate = 1.0E7; } <-- node81.out[5];
        node163.out[1] --> {  delay = 0.499867805612605ms; datarate = 1.0E7; } --> node63.in[2];
        node163.in[1] <-- {  delay = 0.499867805612605ms; datarate = 1.0E7; } <-- node63.out[2];
        node163.out[2] --> {  delay = 0.44876459323982426ms; datarate = 1.0E7; } --> node39.in[2];
        node163.in[2] <-- {  delay = 0.44876459323982426ms; datarate = 1.0E7; } <-- node39.out[2];
        node164.out[0] --> {  delay = 0.9128266541233304ms; datarate = 1.0E7; } --> node116.in[2];
        node164.in[0] <-- {  delay = 0.9128266541233304ms; datarate = 1.0E7; } <-- node116.out[2];
        node164.out[1] --> {  delay = 0.5043881791999688ms; datarate = 1.0E7; } --> node102.in[5];
        node164.in[1] <-- {  delay = 0.5043881791999688ms; datarate = 1.0E7; } <-- node102.out[5];
        node165.out[0] --> {  delay = 1.9426207028437275ms; datarate = 1.0E7; } --> node156.in[4];
        node165.in[0] <-- {  delay = 1.9426207028437275ms; datarate = 1.0E7; } <-- node156.out[4];
        node165.out[1] --> {  delay = 2.520449239428871ms; datarate = 1.0E7; } --> node24.in[6];
        node165.in[1] <-- {  delay = 2.520449239428871ms; datarate = 1.0E7; } <-- node24.out[6];
        node166.out[3] --> {  delay = 0.7757785865305914ms; datarate = 1.0E7; } --> node159.in[7];
        node166.in[3] <-- {  delay = 0.7757785865305914ms; datarate = 1.0E7; } <-- node159.out[7];
        node166.out[4] --> {  delay = 0.1934096556134741ms; datarate = 1.0E7; } --> node86.in[7];
        node166.in[4] <-- {  delay = 0.1934096556134741ms; datarate = 1.0E7; } <-- node86.out[7];
        node167.out[0] --> {  delay = 0.7795200928215786ms; datarate = 1.0E7; } --> node96.in[11];
        node167.in[0] <-- {  delay = 0.7795200928215786ms; datarate = 1.0E7; } <-- node96.out[11];
        node167.out[1] --> {  delay = 0.6062888742327653ms; datarate = 1.0E7; } --> node145.in[5];
        node167.in[1] <-- {  delay = 0.6062888742327653ms; datarate = 1.0E7; } <-- node145.out[5];
        node168.out[5] --> {  delay = 1.04652512137801ms; datarate = 1.0E7; } --> node85.in[11];
        node168.in[5] <-- {  delay = 1.04652512137801ms; datarate = 1.0E7; } <-- node85.out[11];
        node168.out[6] --> {  delay = 0.7398959930824746ms; datarate = 1.0E7; } --> node96.in[12];
        node168.in[6] <-- {  delay = 0.7398959930824746ms; datarate = 1.0E7; } <-- node96.out[12];
        node169.out[1] --> {  delay = 1.0844937721128127ms; datarate = 1.0E7; } --> node93.in[3];
        node169.in[1] <-- {  delay = 1.0844937721128127ms; datarate = 1.0E7; } <-- node93.out[3];
        node169.out[2] --> {  delay = 0.5758209067965301ms; datarate = 1.0E7; } --> node40.in[3];
        node169.in[2] <-- {  delay = 0.5758209067965301ms; datarate = 1.0E7; } <-- node40.out[3];
        node170.out[0] --> {  delay = 0.09900730777994309ms; datarate = 1.0E7; } --> node17.in[3];
        node170.in[0] <-- {  delay = 0.09900730777994309ms; datarate = 1.0E7; } <-- node17.out[3];
        node170.out[1] --> {  delay = 0.990528114658358ms; datarate = 1.0E7; } --> node132.in[5];
        node170.in[1] <-- {  delay = 0.990528114658358ms; datarate = 1.0E7; } <-- node132.out[5];
        node171.out[1] --> {  delay = 0.6554513251519316ms; datarate = 1.0E7; } --> node74.in[3];
        node171.in[1] <-- {  delay = 0.6554513251519316ms; datarate = 1.0E7; } <-- node74.out[3];
        node171.out[2] --> {  delay = 3.013356078270947ms; datarate = 1.0E7; } --> node87.in[6];
        node171.in[2] <-- {  delay = 3.013356078270947ms; datarate = 1.0E7; } <-- node87.out[6];
        node172.out[0] --> {  delay = 1.7489258883940049ms; datarate = 1.0E7; } --> node196.in[2];
        node172.in[0] <-- {  delay = 1.7489258883940049ms; datarate = 1.0E7; } <-- node196.out[2];
        node172.out[1] --> {  delay = 1.361636256275657ms; datarate = 1.0E7; } --> node100.in[10];
        node172.in[1] <-- {  delay = 1.361636256275657ms; datarate = 1.0E7; } <-- node100.out[10];
        node173.out[0] --> {  delay = 0.7397079951342205ms; datarate = 1.0E7; } --> node11.in[2];
        node173.in[0] <-- {  delay = 0.7397079951342205ms; datarate = 1.0E7; } <-- node11.out[2];
        node173.out[1] --> {  delay = 1.1801618366420084ms; datarate = 1.0E7; } --> node138.in[2];
        node173.in[1] <-- {  delay = 1.1801618366420084ms; datarate = 1.0E7; } <-- node138.out[2];
        node174.out[0] --> {  delay = 1.0928081067431141ms; datarate = 1.0E7; } --> node13.in[2];
        node174.in[0] <-- {  delay = 1.0928081067431141ms; datarate = 1.0E7; } <-- node13.out[2];
        node174.out[1] --> {  delay = 1.4139498317547845ms; datarate = 1.0E7; } --> node80.in[2];
        node174.in[1] <-- {  delay = 1.4139498317547845ms; datarate = 1.0E7; } <-- node80.out[2];
        node175.out[0] --> {  delay = 0.8972378141030808ms; datarate = 1.0E7; } --> node67.in[2];
        node175.in[0] <-- {  delay = 0.8972378141030808ms; datarate = 1.0E7; } <-- node67.out[2];
        node175.out[1] --> {  delay = 1.6213445497280456ms; datarate = 1.0E7; } --> node80.in[3];
        node175.in[1] <-- {  delay = 1.6213445497280456ms; datarate = 1.0E7; } <-- node80.out[3];
        node176.out[0] --> {  delay = 0.8506473049706972ms; datarate = 1.0E7; } --> node94.in[5];
        node176.in[0] <-- {  delay = 0.8506473049706972ms; datarate = 1.0E7; } <-- node94.out[5];
        node176.out[1] --> {  delay = 1.478708859159426ms; datarate = 1.0E7; } --> node7.in[2];
        node176.in[1] <-- {  delay = 1.478708859159426ms; datarate = 1.0E7; } <-- node7.out[2];
        node177.out[2] --> {  delay = 0.2535964771835349ms; datarate = 1.0E7; } --> node40.in[4];
        node177.in[2] <-- {  delay = 0.2535964771835349ms; datarate = 1.0E7; } <-- node40.out[4];
        node177.out[3] --> {  delay = 0.7258296460195242ms; datarate = 1.0E7; } --> node146.in[6];
        node177.in[3] <-- {  delay = 0.7258296460195242ms; datarate = 1.0E7; } <-- node146.out[6];
        node178.out[2] --> {  delay = 1.3821794913266738ms; datarate = 1.0E7; } --> node104.in[4];
        node178.in[2] <-- {  delay = 1.3821794913266738ms; datarate = 1.0E7; } <-- node104.out[4];
        node178.out[3] --> {  delay = 1.1374975824962235ms; datarate = 1.0E7; } --> node162.in[4];
        node178.in[3] <-- {  delay = 1.1374975824962235ms; datarate = 1.0E7; } <-- node162.out[4];
        node179.out[1] --> {  delay = 1.3044531358864218ms; datarate = 1.0E7; } --> node38.in[6];
        node179.in[1] <-- {  delay = 1.3044531358864218ms; datarate = 1.0E7; } <-- node38.out[6];
        node179.out[2] --> {  delay = 1.2941685088803203ms; datarate = 1.0E7; } --> node102.in[6];
        node179.in[2] <-- {  delay = 1.2941685088803203ms; datarate = 1.0E7; } <-- node102.out[6];
        node180.out[1] --> {  delay = 0.028694268443012757ms; datarate = 1.0E7; } --> node102.in[7];
        node180.in[1] <-- {  delay = 0.028694268443012757ms; datarate = 1.0E7; } <-- node102.out[7];
        node180.out[2] --> {  delay = 0.45081776721725814ms; datarate = 1.0E7; } --> node160.in[10];
        node180.in[2] <-- {  delay = 0.45081776721725814ms; datarate = 1.0E7; } <-- node160.out[10];
        node181.out[2] --> {  delay = 0.7569259497227517ms; datarate = 1.0E7; } --> node88.in[3];
        node181.in[2] <-- {  delay = 0.7569259497227517ms; datarate = 1.0E7; } <-- node88.out[3];
        node181.out[3] --> {  delay = 0.460849913420318ms; datarate = 1.0E7; } --> node36.in[7];
        node181.in[3] <-- {  delay = 0.460849913420318ms; datarate = 1.0E7; } <-- node36.out[7];
        node182.out[2] --> {  delay = 0.5953721985930832ms; datarate = 1.0E7; } --> node146.in[7];
        node182.in[2] <-- {  delay = 0.5953721985930832ms; datarate = 1.0E7; } <-- node146.out[7];
        node182.out[3] --> {  delay = 0.42556556442438753ms; datarate = 1.0E7; } --> node82.in[6];
        node182.in[3] <-- {  delay = 0.42556556442438753ms; datarate = 1.0E7; } <-- node82.out[6];
        node183.out[4] --> {  delay = 1.045126097668524ms; datarate = 1.0E7; } --> node158.in[9];
        node183.in[4] <-- {  delay = 1.045126097668524ms; datarate = 1.0E7; } <-- node158.out[9];
        node183.out[5] --> {  delay = 0.7229958480752084ms; datarate = 1.0E7; } --> node27.in[3];
        node183.in[5] <-- {  delay = 0.7229958480752084ms; datarate = 1.0E7; } <-- node27.out[3];
        node184.out[2] --> {  delay = 0.8878884751001502ms; datarate = 1.0E7; } --> node155.in[2];
        node184.in[2] <-- {  delay = 0.8878884751001502ms; datarate = 1.0E7; } <-- node155.out[2];
        node184.out[3] --> {  delay = 1.7162221170016716ms; datarate = 1.0E7; } --> node146.in[8];
        node184.in[3] <-- {  delay = 1.7162221170016716ms; datarate = 1.0E7; } <-- node146.out[8];
        node185.out[0] --> {  delay = 0.3955935166100949ms; datarate = 1.0E7; } --> node81.in[6];
        node185.in[0] <-- {  delay = 0.3955935166100949ms; datarate = 1.0E7; } <-- node81.out[6];
        node185.out[1] --> {  delay = 0.7721414417292799ms; datarate = 1.0E7; } --> node102.in[8];
        node185.in[1] <-- {  delay = 0.7721414417292799ms; datarate = 1.0E7; } <-- node102.out[8];
        node186.out[0] --> {  delay = 0.06132501275549179ms; datarate = 1.0E7; } --> node32.in[9];
        node186.in[0] <-- {  delay = 0.06132501275549179ms; datarate = 1.0E7; } <-- node32.out[9];
        node186.out[1] --> {  delay = 1.0639648929967156ms; datarate = 1.0E7; } --> node121.in[5];
        node186.in[1] <-- {  delay = 1.0639648929967156ms; datarate = 1.0E7; } <-- node121.out[5];
        node187.out[0] --> {  delay = 1.9794267890956347ms; datarate = 1.0E7; } --> node150.in[4];
        node187.in[0] <-- {  delay = 1.9794267890956347ms; datarate = 1.0E7; } <-- node150.out[4];
        node187.out[1] --> {  delay = 0.6365133658540126ms; datarate = 1.0E7; } --> node26.in[5];
        node187.in[1] <-- {  delay = 0.6365133658540126ms; datarate = 1.0E7; } <-- node26.out[5];
        node188.out[0] --> {  delay = 1.2998304159071752ms; datarate = 1.0E7; } --> node115.in[5];
        node188.in[0] <-- {  delay = 1.2998304159071752ms; datarate = 1.0E7; } <-- node115.out[5];
        node188.out[1] --> {  delay = 0.41282515558736577ms; datarate = 1.0E7; } --> node160.in[11];
        node188.in[1] <-- {  delay = 0.41282515558736577ms; datarate = 1.0E7; } <-- node160.out[11];
        node189.out[1] --> {  delay = 1.111404330668405ms; datarate = 1.0E7; } --> node61.in[3];
        node189.in[1] <-- {  delay = 1.111404330668405ms; datarate = 1.0E7; } <-- node61.out[3];
        node189.out[2] --> {  delay = 1.368417988531691ms; datarate = 1.0E7; } --> node84.in[4];
        node189.in[2] <-- {  delay = 1.368417988531691ms; datarate = 1.0E7; } <-- node84.out[4];
        node190.out[1] --> {  delay = 2.0168163499478315ms; datarate = 1.0E7; } --> node78.in[5];
        node190.in[1] <-- {  delay = 2.0168163499478315ms; datarate = 1.0E7; } <-- node78.out[5];
        node190.out[2] --> {  delay = 2.5416314552742416ms; datarate = 1.0E7; } --> node16.in[2];
        node190.in[2] <-- {  delay = 2.5416314552742416ms; datarate = 1.0E7; } <-- node16.out[2];
        node191.out[0] --> {  delay = 1.5826728779791195ms; datarate = 1.0E7; } --> node98.in[5];
        node191.in[0] <-- {  delay = 1.5826728779791195ms; datarate = 1.0E7; } <-- node98.out[5];
        node191.out[1] --> {  delay = 1.4866671442678312ms; datarate = 1.0E7; } --> node124.in[2];
        node191.in[1] <-- {  delay = 1.4866671442678312ms; datarate = 1.0E7; } <-- node124.out[2];
        node192.out[1] --> {  delay = 0.5980851666114767ms; datarate = 1.0E7; } --> node162.in[5];
        node192.in[1] <-- {  delay = 0.5980851666114767ms; datarate = 1.0E7; } <-- node162.out[5];
        node192.out[2] --> {  delay = 1.3285276276454676ms; datarate = 1.0E7; } --> node36.in[8];
        node192.in[2] <-- {  delay = 1.3285276276454676ms; datarate = 1.0E7; } <-- node36.out[8];
        node193.out[2] --> {  delay = 1.918005149583681ms; datarate = 1.0E7; } --> node150.in[5];
        node193.in[2] <-- {  delay = 1.918005149583681ms; datarate = 1.0E7; } <-- node150.out[5];
        node193.out[3] --> {  delay = 0.3776499492717466ms; datarate = 1.0E7; } --> node194.in[5];
        node193.in[3] <-- {  delay = 0.3776499492717466ms; datarate = 1.0E7; } <-- node194.out[5];
        node194.out[6] --> {  delay = 1.8513558538739041ms; datarate = 1.0E7; } --> node160.in[12];
        node194.in[6] <-- {  delay = 1.8513558538739041ms; datarate = 1.0E7; } <-- node160.out[12];
        node194.out[7] --> {  delay = 1.675464924109603ms; datarate = 1.0E7; } --> node85.in[12];
        node194.in[7] <-- {  delay = 1.675464924109603ms; datarate = 1.0E7; } <-- node85.out[12];
        node195.out[0] --> {  delay = 0.7411505961497372ms; datarate = 1.0E7; } --> node81.in[7];
        node195.in[0] <-- {  delay = 0.7411505961497372ms; datarate = 1.0E7; } <-- node81.out[7];
        node195.out[1] --> {  delay = 0.6213968743507226ms; datarate = 1.0E7; } --> node160.in[13];
        node195.in[1] <-- {  delay = 0.6213968743507226ms; datarate = 1.0E7; } <-- node160.out[13];
        node196.out[3] --> {  delay = 2.4690345829116795ms; datarate = 1.0E7; } --> node85.in[13];
        node196.in[3] <-- {  delay = 2.4690345829116795ms; datarate = 1.0E7; } <-- node85.out[13];
        node196.out[4] --> {  delay = 2.6471143049579093ms; datarate = 1.0E7; } --> node166.in[5];
        node196.in[4] <-- {  delay = 2.6471143049579093ms; datarate = 1.0E7; } <-- node166.out[5];
        node197.out[0] --> {  delay = 1.5616821178990326ms; datarate = 1.0E7; } --> node145.in[6];
        node197.in[0] <-- {  delay = 1.5616821178990326ms; datarate = 1.0E7; } <-- node145.out[6];
        node197.out[1] --> {  delay = 0.5569621491876705ms; datarate = 1.0E7; } --> node198.in[5];
        node197.in[1] <-- {  delay = 0.5569621491876705ms; datarate = 1.0E7; } <-- node198.out[5];
        node198.out[6] --> {  delay = 1.4433153042375693ms; datarate = 1.0E7; } --> node27.in[4];
        node198.in[6] <-- {  delay = 1.4433153042375693ms; datarate = 1.0E7; } <-- node27.out[4];
        node198.out[7] --> {  delay = 1.9400989342360315ms; datarate = 1.0E7; } --> node167.in[2];
        node198.in[7] <-- {  delay = 1.9400989342360315ms; datarate = 1.0E7; } <-- node167.out[2];
        node199.out[2] --> {  delay = 0.7627758303142268ms; datarate = 1.0E7; } --> node99.in[4];
        node199.in[2] <-- {  delay = 0.7627758303142268ms; datarate = 1.0E7; } <-- node99.out[4];
        node199.out[3] --> {  delay = 3.16819521789173ms; datarate = 1.0E7; } --> node82.in[7];
        node199.in[3] <-- {  delay = 3.16819521789173ms; datarate = 1.0E7; } <-- node82.out[7];
}

//
// Example network that instantiates BRITE200Network. For actual simulation,
// provide a module type called InetNode with the same interface as
// BRITE200Node above.
//
network aBRITE200Network extends BRITE200Network
{
    parameters:
        nodetype = "InetNode";
}

