//
// Model for testing animation of broadcasts
//
module BroadcastTest
    submodules:
        txnode: TxNode;
        sink: Sink[10];
    connections:
        for i=0..9 do
            txnode.out++ --> sink[i].in;
        endfor
endmodule

network broadcastTest : BroadcastTest
endnetwork

