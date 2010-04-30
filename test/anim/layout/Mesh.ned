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

network Mesh extends Base
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

