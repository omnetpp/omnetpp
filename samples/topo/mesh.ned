
simple MeshNode
    gates:
        in: up, left;
        out: down, right;
endsimple


module Mesh
    parameters:
        height : numeric const,
        width : numeric const;
    submodules:
        node: MeshNode[height*width];
            display: "p=,,m,$width";
    connections nocheck:
        for i=0..height-1, j=0..width-1 do
            node[i*width+j].down --> node[(i+1)*width+j].up if i!=height-1;
            node[i*width+j].right --> node[i*width+j+1].left if j!=width-1;
        endfor;
endmodule


network mesh : Mesh
    parameters:
        height = 4,
        width = 6;
endnetwork

