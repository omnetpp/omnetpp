network Anchors extends Base
{
        int sizeRow = default(10);
        int sizeMatrix = default(9);
        int sizeRing = default(10);
    submodules:
        vec1[sizeRow]: Node {
            parameters:
                @display("p=,,row;i=pc_s,cyan");
            gates:
                in[3];
                out[3];
        };
        vec2[sizeMatrix]: Node {
            parameters:
                @display("p=,,m,3;i=pc_s,green");
            gates:
                in[3];
                out[3];
        };
        vec3[sizeRing]: Node {
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
        hub.out[0] --> vec1[0].in[2];
        hub.out[1] --> vec2[0].in[2];
        hub.out[2] --> vec3[0].in[2];

        // following connections are only decoration, don't affect layout
        for i=0..sizeRow-2 {
            vec1[i].out[0] --> vec1[i+1].in[0];
        };
        for i=0..1, for j=0..1 {
            vec2[3*i+j].out[0] --> vec2[(3*i+j)+1].in[0];
            vec2[3*i+j].out[1] --> vec2[(3*i+j)+3].in[1];
        };
        for i=0..sizeRing-1 {
            vec3[i].out[0] --> vec3[(i+1)%10].in[0];
        };
}

