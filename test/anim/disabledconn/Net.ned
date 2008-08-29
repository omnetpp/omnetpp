network Net
{
    submodules:
        node1: Node;
        node2: Node;
    connections:
        node1.g <--> {disabled=true;} <--> node2.g;
}
