

//
// Model for testing animation of broadcasts
//
network BroadcastTest
{
    submodules:
        node[10]: Node {
            parameters:
                @display("i=misc/node_s");
        };
    connections allowunconnected:
}
