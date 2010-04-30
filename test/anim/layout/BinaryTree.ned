network BinaryTree extends Base
{
    parameters:
        int height = default(5) @prompt("Binary tree height");
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

