//
// Model for testing animation of broadcasts
//
module BroadcastTest
    submodules:
        tx: TxNode;
        sink: SinkNode[10];
    connections:
        for i=0..9 do
            tx.out++ --> sink[i].in++;
        endfor
endmodule

network broadcastTest : BroadcastTest
endnetwork

