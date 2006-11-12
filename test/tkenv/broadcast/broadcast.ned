import "node";

//
// Model for testing animation of broadcasts
//
module BroadcastTest
    submodules:
        node: Node[10];
            display: "i=misc/node_s";
    connections nocheck:
endmodule

network broadcastTest : BroadcastTest
endnetwork

