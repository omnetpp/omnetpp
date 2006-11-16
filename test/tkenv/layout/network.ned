
module Node
    gates:
        in: in[];
        out: out[];
    connections nocheck:
endmodule


module Anchors
    submodules:
        vec1: Node[10];
            gatesizes: in[3], out[3];
            display: "p=,,row;i=pc_s,cyan";
        vec2: Node[9];
            gatesizes: in[3], out[3];
            display: "p=,,m,3;i=pc_s,green";
        vec3: Node[10];
            gatesizes: in[3], out[3];
            display: "p=,,ring;i=pc_s,gold";
        hub: Node;
            gatesizes: in[3], out[3];
            display: "i=server1";
    connections nocheck:
        // connect three anchored layouts
        hub.out[0] --> vec1[5].in[2];
        hub.out[1] --> vec2[3].in[2];
        hub.out[2] --> vec3[6].in[2];

        // following connections are only decoration, don't affect layout
        for i=0..8 do
            vec1[i].out[0] --> vec1[i+1].in[0];
        endfor;
        for i=0..1, j=0..1 do
            vec2[3*i+j].out[0] --> vec2[3*i+j+1].in[0];
            vec2[3*i+j].out[1] --> vec2[3*i+j+3].in[1];
        endfor;
        for i=0..9 do
            vec3[i].out[0] --> vec3[(i+1)%10].in[0];
        endfor;
endmodule

network anchors : Anchors
endnetwork

module Ethernet
    submodules:
        vec1: Node[10];
            gatesizes: in[3], out[3];
            display: "i=pc_s,cyan";
        vec2: Node[6];
            gatesizes: in[3], out[3];
            display: "i=pc_s,gold";
        seg: Node;
            gatesizes: in[11], out[11];
            display: "b=400,10;o=,blue";
    connections nocheck:
        for i=0..9 do
            seg.out[i] --> vec1[i].in[0];
        endfor;
        for i=0..5 do
            vec2[i].out[0] --> vec2[(i+1)%6].in[0];
        endfor;
        seg.out[10] --> vec2[1].in[1];
endmodule

network ethernet : Ethernet
endnetwork

// These nodes should be scattered, not arranged in a ring
module Wireless
    parameters:
        numNodes: numeric;
    submodules:
        vec: Node[numNodes];
            gatesizes: in[1], out[1];
            display: "i=ball2_vs,gold,100";
    connections nocheck:
endmodule

network wireless : Wireless
    parameters:
       numNodes = input(20);
endnetwork

module Net60
    submodules:
        rte: Node[57];
            gatesizes:
                in[6],
                out[6];
            display: "i=ball2_vs";

    connections nocheck:
        rte[0].out[0] --> rte[1].in[0];
        rte[0].in[0] <-- rte[1].out[0];

        rte[1].out[1] --> rte[2].in[0];
        rte[1].in[1] <-- rte[2].out[0];

        rte[1].out[2] --> rte[4].in[0];
        rte[1].in[2] <-- rte[4].out[0];

        rte[3].out[0] --> rte[4].in[1];
        rte[3].in[0] <-- rte[4].out[1];

        rte[4].out[2] --> rte[5].in[0];
        rte[4].in[2] <-- rte[5].out[0];

        rte[4].out[3] --> rte[7].in[0];
        rte[4].in[3] <-- rte[7].out[0];

        rte[5].out[1] --> rte[6].in[0];
        rte[5].in[1] <-- rte[6].out[0];

        rte[5].out[2] --> rte[10].in[0];
        rte[5].in[2] <-- rte[10].out[0];

        rte[6].out[1] --> rte[7].in[1];
        rte[6].in[1] <-- rte[7].out[1];

        rte[6].out[2] --> rte[9].in[0];
        rte[6].in[2] <-- rte[9].out[0];

        rte[7].out[2] --> rte[8].in[0];
        rte[7].in[2] <-- rte[8].out[0];

        rte[7].out[3] --> rte[12].in[0];
        rte[7].in[3] <-- rte[12].out[0];

        rte[9].out[1] --> rte[11].in[0];
        rte[9].in[1] <-- rte[11].out[0];

        rte[10].out[1] --> rte[11].in[1];
        rte[10].in[1] <-- rte[11].out[1];

        rte[10].out[2] --> rte[13].in[0];
        rte[10].in[2] <-- rte[13].out[0];

        rte[11].out[2] --> rte[12].in[1];
        rte[11].in[2] <-- rte[12].out[1];

        rte[11].out[3] --> rte[14].in[0];
        rte[11].in[3] <-- rte[14].out[0];

        rte[12].out[2] --> rte[15].in[0];
        rte[12].in[2] <-- rte[15].out[0];

        rte[13].out[1] --> rte[18].in[0];
        rte[13].in[1] <-- rte[18].out[0];

        rte[14].out[1] --> rte[15].in[1];
        rte[14].in[1] <-- rte[15].out[1];

        rte[14].out[2] --> rte[22].in[0];
        rte[14].in[2] <-- rte[22].out[0];

        rte[15].out[2] --> rte[16].in[0];
        rte[15].in[2] <-- rte[16].out[0];

        rte[16].out[1] --> rte[17].in[0];
        rte[16].in[1] <-- rte[17].out[0];

        rte[16].out[2] --> rte[23].in[0];
        rte[16].in[2] <-- rte[23].out[0];

        rte[18].out[1] --> rte[19].in[0];
        rte[18].in[1] <-- rte[19].out[0];

        rte[18].out[2] --> rte[21].in[0];
        rte[18].in[2] <-- rte[21].out[0];

        rte[19].out[1] --> rte[20].in[0];
        rte[19].in[1] <-- rte[20].out[0];

        rte[19].out[2] --> rte[26].in[0];
        rte[19].in[2] <-- rte[26].out[0];

        rte[20].out[1] --> rte[21].in[1];
        rte[20].in[1] <-- rte[21].out[1];

        rte[20].out[2] --> rte[24].in[0];
        rte[20].in[2] <-- rte[24].out[0];

        rte[20].out[3] --> rte[25].in[0];
        rte[20].in[3] <-- rte[25].out[0];

        rte[21].out[2] --> rte[22].in[1];
        rte[21].in[2] <-- rte[22].out[1];

        rte[21].out[3] --> rte[23].in[1];
        rte[21].in[3] <-- rte[23].out[1];

        rte[21].out[4] --> rte[24].in[1];
        rte[21].in[4] <-- rte[24].out[1];

        rte[23].out[2] --> rte[24].in[2];
        rte[23].in[2] <-- rte[24].out[2];

        rte[24].out[3] --> rte[28].in[0];
        rte[24].in[3] <-- rte[28].out[0];

        rte[25].out[1] --> rte[27].in[0];
        rte[25].in[1] <-- rte[27].out[0];

        rte[26].out[1] --> rte[32].in[0];
        rte[26].in[1] <-- rte[32].out[0];

        rte[27].out[1] --> rte[29].in[0];
        rte[27].in[1] <-- rte[29].out[0];

        rte[27].out[2] --> rte[30].in[0];
        rte[27].in[2] <-- rte[30].out[0];

        rte[27].out[3] --> rte[32].in[1];
        rte[27].in[3] <-- rte[32].out[1];

        rte[28].out[1] --> rte[29].in[1];
        rte[28].in[1] <-- rte[29].out[1];

        rte[28].out[2] --> rte[31].in[0];
        rte[28].in[2] <-- rte[31].out[0];

        rte[29].out[2] --> rte[31].in[1];
        rte[29].in[2] <-- rte[31].out[1];

        rte[30].out[1] --> rte[36].in[0];
        rte[30].in[1] <-- rte[36].out[0];

        rte[31].out[2] --> rte[33].in[0];
        rte[31].in[2] <-- rte[33].out[0];

        rte[32].out[2] --> rte[34].in[0];
        rte[32].in[2] <-- rte[34].out[0];

        rte[33].out[1] --> rte[37].in[0];
        rte[33].in[1] <-- rte[37].out[0];

        rte[34].out[1] --> rte[35].in[0];
        rte[34].in[1] <-- rte[35].out[0];

        rte[34].out[2] --> rte[40].in[0];
        rte[34].in[2] <-- rte[40].out[0];

        rte[35].out[1] --> rte[36].in[1];
        rte[35].in[1] <-- rte[36].out[1];

        rte[35].out[2] --> rte[39].in[0];
        rte[35].in[2] <-- rte[39].out[0];

        rte[36].out[2] --> rte[37].in[1];
        rte[36].in[2] <-- rte[37].out[1];

        rte[37].out[2] --> rte[38].in[0];
        rte[37].in[2] <-- rte[38].out[0];

        rte[38].out[1] --> rte[39].in[1];
        rte[38].in[1] <-- rte[39].out[1];

        rte[38].out[2] --> rte[42].in[0];
        rte[38].in[2] <-- rte[42].out[0];

        rte[39].out[2] --> rte[40].in[1];
        rte[39].in[2] <-- rte[40].out[1];

        rte[39].out[3] --> rte[41].in[0];
        rte[39].in[3] <-- rte[41].out[0];

        rte[40].out[2] --> rte[43].in[0];
        rte[40].in[2] <-- rte[43].out[0];

        rte[40].out[3] --> rte[44].in[0];
        rte[40].in[3] <-- rte[44].out[0];

        rte[40].out[4] --> rte[52].in[0];
        rte[40].in[4] <-- rte[52].out[0];

        rte[41].out[1] --> rte[42].in[1];
        rte[41].in[1] <-- rte[42].out[1];

        rte[41].out[2] --> rte[43].in[1];
        rte[41].in[2] <-- rte[43].out[1];

        rte[42].out[2] --> rte[43].in[2];
        rte[42].in[2] <-- rte[43].out[2];

        rte[43].out[3] --> rte[46].in[0];
        rte[43].in[3] <-- rte[46].out[0];

        rte[44].out[1] --> rte[45].in[0];
        rte[44].in[1] <-- rte[45].out[0];

        rte[44].out[2] --> rte[47].in[0];
        rte[44].in[2] <-- rte[47].out[0];

        rte[45].out[1] --> rte[46].in[1];
        rte[45].in[1] <-- rte[46].out[1];

        rte[46].out[2] --> rte[49].in[0];
        rte[46].in[2] <-- rte[49].out[0];

        rte[47].out[1] --> rte[48].in[0];
        rte[47].in[1] <-- rte[48].out[0];

        rte[48].out[1] --> rte[50].in[0];
        rte[48].in[1] <-- rte[50].out[0];

        rte[48].out[2] --> rte[51].in[0];
        rte[48].in[2] <-- rte[51].out[0];

        rte[49].out[1] --> rte[50].in[1];
        rte[49].in[1] <-- rte[50].out[1];

        rte[50].out[2] --> rte[53].in[0];
        rte[50].in[2] <-- rte[53].out[0];

        rte[51].out[1] --> rte[52].in[1];
        rte[51].in[1] <-- rte[52].out[1];

        rte[51].out[2] --> rte[53].in[1];
        rte[51].in[2] <-- rte[53].out[1];

        rte[52].out[2] --> rte[53].in[2];
        rte[52].in[2] <-- rte[53].out[2];

        rte[52].out[3] --> rte[54].in[0];
        rte[52].in[3] <-- rte[54].out[0];

        rte[53].out[3] --> rte[56].in[0];
        rte[53].in[3] <-- rte[56].out[0];

        rte[54].out[1] --> rte[55].in[0];
        rte[54].in[1] <-- rte[55].out[0];

        rte[55].out[1] --> rte[56].in[1];
        rte[55].in[1] <-- rte[56].out[1];
endmodule

network net60 : Net60
endnetwork

module BinaryTree
    parameters:
        height : numeric const;
    submodules:
        node: Node[ 2^height-1 ];
            gatesizes: in[1], out[2];
            display: "i=ball2_vs";
    connections nocheck:
        for i=0..2^(height-1)-2 do
            node[i].out[0] --> node[2*i+1].in[0];
            node[i].out[1] --> node[2*i+2].in[0];
        endfor;
endmodule

network binaryTree : BinaryTree
   parameters: height = input(5);
endnetwork

module HexGrid
    parameters:
        rows : numeric const,
        cols : numeric const;
    submodules:
        node  : Node[rows * cols];
            gatesizes: in[3], out[3];
            display: "i=ball2_vs";
    connections nocheck:
        for i = 0..rows*cols-1 do
            node[i].in[0] <-- node[i-2*cols].out[0] if i+1>2*cols;
            node[i].in[1] <-- node[i-cols-(i/cols+1)%2].out[1] if i%(2*cols)!=0 && i+1>cols;
            node[i].in[2] <-- node[i+cols-1+(i/cols)%2].out[2] if i%(2*cols)!=0 && i<rows*cols-cols;
        endfor;
endmodule

network hexGrid : HexGrid
   parameters: rows = input(6), cols = input(3);
endnetwork

module Dyna
    parameters:
        numNodes: numeric;
    submodules:
        server: Node;
            gatesizes: in[1], out[1];
            display: "i=server1,gold";
        switch: Node;
            gatesizes: in[numNodes+1], out[numNodes+1];
            display: "i=switch2,cyan";
        client: Node[numNodes];
            gatesizes: in[1], out[1];
            display: "i=pc,green;p=,,m,10";
    connections nocheck:
        server.out[0] --> switch.in[0];
        for i=0..numNodes-1 do
            client[i].out[0] --> switch.in[i+1];
        endfor;
endmodule

network dyna : Dyna
    parameters:
       numNodes = input(10);
endnetwork

module RandomNet
    parameters:
        numNodes: numeric;
    submodules:
        vec: Node[numNodes];
            gatesizes: in[numNodes], out[numNodes];
            display: "i=ball2_vs,gold,100";
    connections nocheck:
        for i=0..numNodes-1, j=0..numNodes-1 do
            vec[i].out[j] --> vec[j].in[i] if uniform(0,1)<0.7/numNodes && i!=j;
        endfor;
endmodule

network randomNet : RandomNet
    parameters:
       numNodes = input(60);
endnetwork

module Protocols1
    gates:
        in: in;
        out: out;
    submodules:
        mac: Node;
            gatesizes: in[2], out[2];
            display: "i=queue";
        gen: Node;
            gatesizes: in[0], out[1];
            display: "i=gen";
        sink: Node;
            gatesizes: in[1], out[0];
            display: "i=sink";
    connections nocheck:
        in --> mac.in[0];
        out <-- mac.out[0];
        mac.out[1] --> sink.in[0];
        mac.in[1] <-- gen.out[0];
endmodule

network protocols1 : Protocols1
endnetwork

module NetworkLayers
    parameters:
        numPorts: numeric;
    gates:
        in: TCPIn;
        out: TCPOut;
        in: physIn[];
        out: physOut[];
    submodules:
        proc: Node;
            gatesizes: in[numPorts+2],out[numPorts+2];
            display: "i=cogwheel";
        outputQueue: Node[numPorts];
            gatesizes: in[2],out[2];
            display: "p=,,row;i=queue";
        inputQueue: Node;
            gatesizes: in[numPorts],out[1];
            display: "i=queue";
        networkInterface: Node[numPorts];
            gatesizes: in[3],out[3];
            display: "p=,,row;i=bwxconn";
        routingTable: Node;
            display: "p=,,row;i=box3";

    connections nocheck:
        proc.out[numPorts] --> TCPOut;
        proc.in[numPorts] <-- TCPIn;
        proc.in[numPorts+1] <-- inputQueue.out[0];

        for i=0..numPorts-1 do
            proc.out[i] --> outputQueue[i].in[0];
            networkInterface[i].out[0] --> inputQueue.in[i];
            networkInterface[i].out[2] --> outputQueue[i].in[1];
            networkInterface[i].in[2] <-- outputQueue[i].out[0];
            networkInterface[i].out[1] --> physOut[i];
            networkInterface[i].in[1] <-- physIn[i];
        endfor;
endmodule

module NetworkLayersWrapper
    parameters:
       numPorts: numeric;
    submodules:
       doubleClickHere : NetworkLayers;
           parameters:
              numPorts = numPorts;
           gatesizes:
              physIn[numPorts],
              physOut[numPorts];
           display: "i=cloud_l";
endmodule

network networkLayers : NetworkLayersWrapper
    parameters:
       numPorts = input(3);
endnetwork

module Ring
    parameters:
        numNodes : numeric const;
    submodules:
        comp: Node[numNodes];
            gatesizes: in[1], out[1];
            display: "i=pc,cyan";
    connections nocheck:
        comp[numNodes-1].out[0] --> comp[0].in[0];
        for i=0..numNodes-2 do
            comp[i].out[0] --> comp[i+1].in[0];
        endfor;
endmodule

network ring : Ring
    parameters:
       numNodes = input(6);
endnetwork

module Chain
    parameters:
        numNodes : numeric const;
    submodules:
        comp: Node[numNodes];
            gatesizes: in[1], out[1];
            display: "i=ball2";
    connections nocheck:
        for i=0..numNodes-2 do
            comp[i].out[0] --> comp[i+1].in[0];
        endfor;
endmodule

network chain : Chain
    parameters:
       numNodes = input(20);
endnetwork

module SixNodes
    submodules:
        tic: Node[6];
            display: "i=ball2";
    connections:
        tic[0].out++ --> tic[1].in++;
        tic[0].in++ <-- tic[1].out++;

        tic[0].out++ --> tic[3].in++;
        tic[0].in++ <-- tic[3].out++;

        tic[1].out++ --> tic[2].in++;
        tic[1].in++ <-- tic[2].out++;

        tic[1].out++ --> tic[3].in++;
        tic[1].in++ <-- tic[3].out++;

        tic[1].out++ --> tic[4].in++;
        tic[1].in++ <-- tic[4].out++;

        tic[3].out++ --> tic[4].in++;
        tic[3].in++ <-- tic[4].out++;

        tic[4].out++ --> tic[5].in++;
        tic[4].in++ <-- tic[5].out++;
endmodule

network sixNodes : SixNodes
endnetwork


module QNet
    submodules:
        acp1: Node;
        acp2: Node;
        leave: Node;
        src1: Node;
        src2: Node;
        src3: Node;
        src4: Node;
        src5: Node;
        src6: Node;
    connections:
        src3.out++ --> acp2.in++;
        src4.out++ --> acp1.in++;
        src5.out++ --> acp1.in++;
        src6.out++ --> acp1.in++;
        src2.out++ --> acp2.in++;
        src1.out++ --> acp2.in++;
        acp2.out++ --> acp1.in++;
        acp1.out++ --> acp2.in++;
        leave.out++ --> src1.in++;
        leave.out++ --> src2.in++;
        leave.out++ --> src3.in++;
        leave.out++ --> src4.in++;
        leave.out++ --> src5.in++;
        leave.out++ --> src6.in++;
endmodule

network qNet : QNet
endnetwork


