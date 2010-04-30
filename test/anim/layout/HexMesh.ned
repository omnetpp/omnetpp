network HexMesh extends Base
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

