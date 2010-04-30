network Ethernet extends Base
{
        int size = default(10);
    submodules:
        vec1[size]: Node {
            parameters:
                @display("i=pc_s,cyan");
        };
        seg: Node {
            parameters:
                @display("b=400,10,,,blue");
        };
    connections allowunconnected:
        for i=0..size-1 {
            seg.out++ --> vec1[i].in++;
        };
}

