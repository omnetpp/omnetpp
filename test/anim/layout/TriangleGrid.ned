network TriangleGrid extends Base
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

