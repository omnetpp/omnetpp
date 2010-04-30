network EthernetWithRing extends Base
{
        int size = default(10);
        int ringSize = default(10);
    submodules:
        vec1[size]: Node {
            parameters:
                @display("i=pc_s,cyan");
        };
        vec2[ringSize]: Node {
            parameters:
                @display("i=pc_s,gold");
        };
        seg: Node {
            parameters:
                @display("b=400,10,,,blue");
        };
    connections allowunconnected:
        for i=0..size-1 {
            seg.out++ --> vec1[i].in++;
        };
        for i=0..ringSize-1 {
            vec2[i].out++ --> vec2[(i+1)%6].in++;
        };
        seg.out++ --> vec2[1].in++;
}

