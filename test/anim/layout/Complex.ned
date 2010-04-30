network Complex extends Base
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

