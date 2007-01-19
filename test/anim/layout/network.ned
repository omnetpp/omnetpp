module Node
{
    gates:
        input in[];
        output out[];
    connections allowunconnected:
}

network Anchors
{
    submodules:
        vec1[10]: Node {
            parameters:
                @display("p=,,row;i=pc_s,cyan");
            gates:
                in[3];
                out[3];
        };
        vec2[9]: Node {
            parameters:
                @display("p=,,m,3;i=pc_s,green");
            gates:
                in[3];
                out[3];
        };
        vec3[10]: Node {
            parameters:
                @display("p=,,ring;i=pc_s,gold");
            gates:
                in[3];
                out[3];
        };
        hub: Node {
            parameters:
                @display("i=server1");
            gates:
                in[3];
                out[3];
        };
    connections allowunconnected:
        // connect three anchored layouts
        hub.out[0] --> vec1[5].in[2];
        hub.out[1] --> vec2[3].in[2];
        hub.out[2] --> vec3[6].in[2];

        // following connections are only decoration, don't affect layout
        for i=0..8 {
            vec1[i].out[0] --> vec1[i+1].in[0];
        };
        for i=0..1, for j=0..1 {
            vec2[3*i+j].out[0] --> vec2[(3*i+j)+1].in[0];
            vec2[3*i+j].out[1] --> vec2[(3*i+j)+3].in[1];
        };
        for i=0..9 {
            vec3[i].out[0] --> vec3[(i+1)%10].in[0];
        };
}

network Ethernet
{
    submodules:
        vec1[10]: Node {
            parameters:
                @display("i=pc_s,cyan");
            gates:
                in[3];
                out[3];
        };
        seg: Node {
            parameters:
                @display("b=400,10,,,blue");
            gates:
                in[11];
                out[11];
        };
    connections allowunconnected:
        for i=0..9 {
            seg.out[i] --> vec1[i].in[0];
        };
}

network EthernetWithRing
{
    submodules:
        vec1[10]: Node {
            parameters:
                @display("i=pc_s,cyan");
            gates:
                in[3];
                out[3];
        };
        vec2[6]: Node {
            parameters:
                @display("i=pc_s,gold");
            gates:
                in[3];
                out[3];
        };
        seg: Node {
            parameters:
                @display("b=400,10,,,blue");
            gates:
                in[11];
                out[11];
        };
    connections allowunconnected:
        for i=0..9 {
            seg.out[i] --> vec1[i].in[0];
        };
        for i=0..5 {
            vec2[i].out[0] --> vec2[(i+1)%6].in[0];
        };
        seg.out[10] --> vec2[1].in[1];
}

// These nodes should be scattered, not arranged in a ring
network Wireless
{
    parameters:
        volatile double numNodes = default(20) @prompt("Number of nodes");
    submodules:
        vec[numNodes]: Node {
            parameters:
                @display("i=ball2,gold,100;is=vs");
            gates:
                in[1];
                out[1];
        };
    connections allowunconnected:
}

network Net60
{
    submodules:
        rte[57]: Node {
            parameters:
                @display("i=old/ball2;is=vs");

            gates:
                in[6];
                out[6];
        };
    connections allowunconnected:
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
}

network BinaryTree
{
    parameters:
        double height = default(5) @prompt("Binary tree height");
    submodules:
        node[2^height-1]: Node {
            parameters:
                @display("i=ball2;is=vs");
            gates:
                in[1];
                out[2];
        };
    connections allowunconnected:
        for i=0..2^(height-1)-2 {
            node[i].out[0] --> node[2*i+1].in[0];
            node[i].out[1] --> node[2*i+2].in[0];
        };
}

network TriangleGrid
{
    parameters:
        int rows = default(5) @prompt("TriangleGrid rows");
        int cols = default(5) @prompt("TriangleGrid columns");
    submodules:
        node[rows*cols]: Node {
            parameters:
                @display("i=ball2;is=vs");
            gates:
                in[3];
                out[3];
        };
    connections allowunconnected:
        for i=0..rows*cols-1 {
              node[i].in[0] <-- node[i+1].out[0] if (i+1)%cols!=0;
              node[i].in[1] <-- node[i+cols].out[1] if i<(rows-1)*cols;
              node[i].in[2] <-- node[i+cols+1].out[2] if i<(rows-1)*cols && (i+1)%cols!=0;
        };
}

network HexMesh
{
    parameters:
        int rows = default(2) @prompt("HexMesh rows");
        int cols = default(2) @prompt("HexMesh columns");
        int num = 2*(rows*cols+rows+cols);
    submodules:
        node[num]: Node {
            parameters:
                @display("i=ball2;is=vs");
            gates:
                in[3];
                out[3];
        };
    connections allowunconnected:
        for i=0..num-1 {
              node[i].in[0] <-- node[i+1].out[0] if i<num-1 && i%(2*cols+2)!=2*cols;
              node[i].in[1] <-- node[i+2*cols+1].out[1] if i<num-2*cols-1 && i%2==0;
        };
}

network Dyna
{
    parameters:
        volatile double numNodes = default(10) @prompt("Number of nodes");
    submodules:
        server: Node {
            parameters:
                @display("i=server1,gold");
            gates:
                in[1];
                out[1];
        };
        switch: Node {
            parameters:
                @display("i=switch2,cyan");
            gates:
                in[numNodes+1];
                out[numNodes+1];
        };
        client[numNodes]: Node {
            parameters:
                @display("i=pc,green;p=,,m,10");
            gates:
                in[1];
                out[1];
        };
    connections allowunconnected:
        server.out[0] --> switch.in[0];
        for i=0..numNodes-1 {
            client[i].out[0] --> switch.in[i+1];
        };
}

network RandomNet
{
    parameters:
        volatile double numNodes = default(60) @prompt("Number of nodes");
    submodules:
        vec[numNodes]: Node {
            parameters:
                @display("i=ball2,gold,100;is=vs");
            gates:
                in[numNodes];
                out[numNodes];
        };
    connections allowunconnected:
        for i=0..numNodes-1, for j=0..numNodes-1 {
            vec[i].out[j] --> vec[j].in[i] if uniform(0, 1)<0.8/numNodes && i!=j;
        };
}

network Protocols1
{
    gates:
        input in;
        output out;
    submodules:
        mac: Node {
            parameters:
                @display("i=queue");
            gates:
                in[2];
                out[2];
        };
        gen: Node {
            parameters:
                @display("i=gen");
            gates:
                in[0];
                out[1];
        };
        sink: Node {
            parameters:
                @display("i=sink");
            gates:
                in[1];
                out[0];
        };
    connections allowunconnected:
        in --> mac.in[0];
        out <-- mac.out[0];
        mac.out[1] --> sink.in[0];
        mac.in[1] <-- gen.out[0];
}

module NetworkLayers
{
    parameters:
        volatile double numPorts;
    gates:
        input TCPIn;
        output TCPOut;
        input physIn[];
        output physOut[];
    submodules:
        proc: Node {
            parameters:
                @display("i=cogwheel");
            gates:
                in[numPorts+2];
                out[numPorts+2];
        };
        outputQueue[numPorts]: Node {
            parameters:
                @display("p=,,row;i=queue");
            gates:
                in[2];
                out[2];
        };
        inputQueue: Node {
            parameters:
                @display("i=queue");
            gates:
                in[numPorts];
                out[1];
        };
        networkInterface[numPorts]: Node {
            parameters:
                @display("p=,,row;i=bwxconn");
        //routingTable: Node;
        //    display: "p=,,row;i=box3";

            gates:
                in[3];
                out[3];
        };
    connections allowunconnected:
        proc.out[numPorts] --> TCPOut;
        proc.in[numPorts] <-- TCPIn;
        proc.in[numPorts+1] <-- inputQueue.out[0];

        for i=0..numPorts-1 {
            proc.out[i] --> outputQueue[i].in[0];
            networkInterface[i].out[0] --> inputQueue.in[i];
            networkInterface[i].out[2] --> outputQueue[i].in[1];
            networkInterface[i].in[2] <-- outputQueue[i].out[0];
            networkInterface[i].out[1] --> physOut[i];
            networkInterface[i].in[1] <-- physIn[i];
        };
}

network NetworkLayersWrapper
{
    parameters:
        volatile double numPorts = default(3) @prompt("Number of ports");
    submodules:
        doubleClickHere: NetworkLayers {
            parameters:
                numPorts = numPorts;
                @display("i=cloud_l");
            gates:
                physIn[numPorts];
                physOut[numPorts];
        };
    connections allowunconnected:
}

network Ring
{
    parameters:
        double numNodes = default(8) @prompt("Number of nodes");
    submodules:
        comp[numNodes]: Node {
            parameters:
                @display("i=pc,cyan");
            gates:
                in[1];
                out[1];
        };
    connections allowunconnected:
        comp[numNodes-1].out[0] --> comp[0].in[0];
        for i=0..numNodes-2 {
            comp[i].out[0] --> comp[i+1].in[0];
        };
}

network Chain
{
    parameters:
        double numNodes = default(20) @prompt("Number of nodes");
    submodules:
        comp[numNodes]: Node {
            parameters:
                @display("i=ball2");
            gates:
                in[1];
                out[1];
        };
    connections allowunconnected:
        for i=0..numNodes-2 {
            comp[i].out[0] --> comp[i+1].in[0];
        };
}

network SixNodes
{
    submodules:
        tic[6]: Node {
            parameters:
                @display("i=ball2");
        };
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
}

network R37
{
    submodules:
        LSR0: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR1: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR2: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR3: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR4: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR5: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR6: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR7: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR8: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR9: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR10: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR11: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR12: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR13: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR14: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR15: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR16: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR17: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR18: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR19: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR20: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR21: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR22: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR23: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR24: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR25: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR26: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR27: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR28: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR29: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR30: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR31: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR32: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR33: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR34: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR35: Node {
            parameters:
                @display("i=abstract/router_s");
        };
        LSR36: Node {
            parameters:
                @display("i=abstract/router_s");

        };
        host1: Node {
            parameters:
                @display("i=device/laptop_s");
        };
        host2: Node {
            parameters:
                @display("i=device/laptop_s");

        };
    connections:
        LSR7.out++ --> LSR0.in++;
        LSR7.in++ <-- LSR0.out++; // cost 259
        LSR8.out++ --> LSR3.in++;
        LSR8.in++ <-- LSR3.out++; // cost 474
        LSR9.out++ --> LSR4.in++;
        LSR9.in++ <-- LSR4.out++; // cost 540
        LSR11.out++ --> LSR7.in++;
        LSR11.in++ <-- LSR7.out++; // cost 264
        LSR12.out++ --> LSR11.in++;
        LSR12.in++ <-- LSR11.out++; // cost 275
        LSR13.out++ --> LSR0.in++;
        LSR13.in++ <-- LSR0.out++; // cost 1067
        LSR13.out++ --> LSR5.in++;
        LSR13.in++ <-- LSR5.out++; // cost 610
        LSR13.out++ --> LSR10.in++;
        LSR13.in++ <-- LSR10.out++; // cost 462
        LSR14.out++ --> LSR0.in++;
        LSR14.in++ <-- LSR0.out++; // cost 552
        LSR14.out++ --> LSR4.in++;
        LSR14.in++ <-- LSR4.out++; // cost 381
        LSR14.out++ --> LSR12.in++;
        LSR14.in++ <-- LSR12.out++; // cost 592
        LSR16.out++ --> LSR8.in++;
        LSR16.in++ <-- LSR8.out++; // cost 436
        LSR18.out++ --> LSR0.in++;
        LSR18.in++ <-- LSR0.out++; // cost 540
        LSR18.out++ --> LSR5.in++;
        LSR18.in++ <-- LSR5.out++; // cost 239
        LSR18.out++ --> LSR10.in++;
        LSR18.in++ <-- LSR10.out++; // cost 690
        LSR18.out++ --> LSR17.in++;
        LSR18.in++ <-- LSR17.out++; // cost 1977
        LSR20.out++ --> LSR2.in++;
        LSR20.in++ <-- LSR2.out++; // cost 760
        LSR20.out++ --> LSR6.in++;
        LSR20.in++ <-- LSR6.out++; // cost 834
        LSR20.out++ --> LSR17.in++;
        LSR20.in++ <-- LSR17.out++; // cost 751
        LSR21.out++ --> LSR2.in++;
        LSR21.in++ <-- LSR2.out++; // cost 508
        LSR21.out++ --> LSR6.in++;
        LSR21.in++ <-- LSR6.out++; // cost 756
        LSR21.out++ --> LSR19.in++;
        LSR21.in++ <-- LSR19.out++; // cost 411
        LSR23.out++ --> LSR4.in++;
        LSR23.in++ <-- LSR4.out++; // cost 757
        LSR23.out++ --> LSR12.in++;
        LSR23.in++ <-- LSR12.out++; // cost 456
        LSR23.out++ --> LSR22.in++;
        LSR23.in++ <-- LSR22.out++; // cost 522
        LSR24.out++ --> LSR9.in++;
        LSR24.in++ <-- LSR9.out++; // cost 722
        LSR24.out++ --> LSR15.in++;
        LSR24.in++ <-- LSR15.out++; // cost 1182
        LSR25.out++ --> LSR1.in++;
        LSR25.in++ <-- LSR1.out++; // cost 1363
        LSR26.out++ --> LSR6.in++;
        LSR26.in++ <-- LSR6.out++; // cost 747
        LSR26.out++ --> LSR7.in++;
        LSR26.in++ <-- LSR7.out++; // cost 393
        LSR26.out++ --> LSR18.in++;
        LSR26.in++ <-- LSR18.out++; // cost 514
        LSR26.out++ --> LSR19.in++;
        LSR26.in++ <-- LSR19.out++; // cost 594
        LSR27.out++ --> LSR4.in++;
        LSR27.in++ <-- LSR4.out++; // cost 420
        LSR27.out++ --> LSR8.in++;
        LSR27.in++ <-- LSR8.out++; // cost 668
        LSR28.out++ --> LSR21.in++;
        LSR28.in++ <-- LSR21.out++; // cost 907
        LSR28.out++ --> LSR22.in++;
        LSR28.in++ <-- LSR22.out++; // cost 720
        LSR28.out++ --> LSR25.in++;
        LSR28.in++ <-- LSR25.out++; // cost 638
        LSR29.out++ --> LSR2.in++;
        LSR29.in++ <-- LSR2.out++; // cost 1244
        LSR29.out++ --> LSR17.in++;
        LSR29.in++ <-- LSR17.out++; // cost 471
        LSR30.out++ --> LSR1.in++;
        LSR30.in++ <-- LSR1.out++; // cost 794
        LSR30.out++ --> LSR3.in++;
        LSR30.in++ <-- LSR3.out++; // cost 487
        LSR31.out++ --> LSR9.in++;
        LSR31.in++ <-- LSR9.out++; // cost 777
        LSR31.out++ --> LSR15.in++;
        LSR31.in++ <-- LSR15.out++; // cost 597
        LSR32.out++ --> LSR12.in++;
        LSR32.in++ <-- LSR12.out++; // cost 271
        LSR32.out++ --> LSR26.in++;
        LSR32.in++ <-- LSR26.out++; // cost 600
        LSR33.out++ --> LSR23.in++;
        LSR33.in++ <-- LSR23.out++; // cost 534
        LSR33.out++ --> LSR27.in++;
        LSR33.in++ <-- LSR27.out++; // cost 376
        LSR34.out++ --> LSR4.in++;
        LSR34.in++ <-- LSR4.out++; // cost 775
        LSR34.out++ --> LSR15.in++;
        LSR34.in++ <-- LSR15.out++; // cost 1370
        LSR34.out++ --> LSR16.in++;
        LSR34.in++ <-- LSR16.out++; // cost 383
        LSR35.out++ --> LSR1.in++;
        LSR35.in++ <-- LSR1.out++; // cost 1500
        LSR35.out++ --> LSR3.in++;
        LSR35.in++ <-- LSR3.out++; // cost 551
        LSR35.out++ --> LSR28.in++;
        LSR35.in++ <-- LSR28.out++; // cost 783
        LSR35.out++ --> LSR33.in++;
        LSR35.in++ <-- LSR33.out++; // cost 400
        LSR36.out++ --> LSR19.in++;
        LSR36.in++ <-- LSR19.out++; // cost 507
        LSR36.out++ --> LSR22.in++;
        LSR36.in++ <-- LSR22.out++; // cost 327
        LSR36.out++ --> LSR32.in++;
        LSR36.in++ <-- LSR32.out++; // cost 218

        host1.out++ --> LSR2.in++;
        host1.in++ <-- LSR2.out++;
        host2.out++ --> LSR34.in++;
        host2.in++ <-- LSR34.out++;

}

module MeshNode
{
    parameters:
        @display("i=misc/node_s");
    gates:
        input up;
        input left;
        output down;
        output right;
    connections allowunconnected:
}


network Mesh
{
    parameters:
        int height = default(4) @prompt("Number of rows");
        int width = default(6) @prompt("Number of columns");
    submodules:
        node[height*width]: MeshNode {
            parameters:
                @display("p=,,,$width");
        };
    connections allowunconnected:
        for i=0..height-1, for j=0..width-1 {
            node[i*width+j].down --> node[(i+1)*width+j].up if i!=height-1;
            node[i*width+j].right --> node[(i*width+j)+1].left if j!=width-1;
        };
}

module FullGraphNode
{
    parameters:
        @display("i=misc/node_vs");
    gates:
        input in[];
        output out[];
    connections allowunconnected:
}


network FullGraph
{
    parameters:
        int n = default(10) @prompt("Number of nodes");
    submodules:
        node[n]: FullGraphNode {
            gates:
                in[n];
                out[n];
        };
    connections allowunconnected:
        for i=0..n-1, for j=0..n-1 {
            node[i].out[j] --> node[j].in[i] if i!=j;
        };
}

module StarNode
{
    parameters:
        @display("i=device/pc2_s");
    gates:
        input in;
    connections allowunconnected:
}


module HubNode
{
    parameters:
        @display("i=device/switch");
    gates:
        output out[];
    connections allowunconnected:
}


network Star
{
    parameters:
        int n = default(11) @prompt("Number of stations");
    submodules:
        node[n]: StarNode {
        };
        hub: HubNode {
            gates:
                out[n];
        };
    connections allowunconnected:
        for i=0..n-1 {
            hub.out[i] --> node[i].in;
        };
}

network complex
{
    parameters:
        int starNodes = default(11) @prompt("Number of star stations");
        int height = default(4) @prompt("Number of mesh rows");
        int width = default(6) @prompt("Number of mesh columns");
        int ringNodes = default(8) @prompt("Number of ring nodes");
        int chainNodes = default(10) @prompt("Number of chain nodes");
    submodules:
        star_node[starNodes]: Node;
        hub: Node;
        mesh_node[height*width]: MeshNode {
            @display("i=misc/globe;p=,,m,$width");
        };
        ring_comp[ringNodes]: Node {
            @display("i=misc/node;is=s;p=,,ring");
        };
        chain_comp[chainNodes]: Node;
    connections allowunconnected:
        for i=0..starNodes-1 {
            hub.out++ --> star_node[i].in++;
        };

        for i=0..height-1, for j=0..width-1 {
            mesh_node[i*width+j].down --> mesh_node[(i+1)*width+j].up if i!=height-1;
            mesh_node[i*width+j].right --> mesh_node[(i*width+j)+1].left if j!=width-1;
        };

        ring_comp[ringNodes-1].out++ --> ring_comp[0].in++;
        for i=0..ringNodes-2 {
            ring_comp[i].out++ --> ring_comp[i+1].in++;
        };

        for i=0..chainNodes-2 {
            chain_comp[i].out++ --> chain_comp[i+1].in++;
        };

        hub.in++ <-- ring_comp[0].out++;
        ring_comp[ringNodes/2].in++ <-- chain_comp[0].out++;
        chain_comp[chainNodes/2].in++ <-- mesh_node[height*width-1].right;
        mesh_node[0].left <-- hub.out++;
}

network FixedBox
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

network Border
{
    parameters:
        double numNodes = default(20) @prompt("Number of nodes");
    gates:
        input in[1];
        output out[1];
    submodules:
        comp[numNodes]: Node {
            parameters:
                @display("i=ball2");
        };
    connections allowunconnected:
        for i=0..numNodes-2 {
            comp[i].out++ --> comp[i+1].in++;
        };
        comp[0].out++ --> out++;
        comp[numNodes/2].in++ <-- in++;
}
