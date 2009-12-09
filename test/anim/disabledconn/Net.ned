module BoxedNode
{
    gates:
        inout g;
    submodules:
        node: Node;
    connections:
        node.g <--> g;

}

network Net
{
    submodules:
        node1: Node;
        node2: Node;
        boxedNode1: BoxedNode;
        boxedNode2: BoxedNode;
    connections:
        node1.g <--> {disabled=true;} <--> node2.g;
        boxedNode1.g <--> {disabled=true;} <--> boxedNode2.g;
}
