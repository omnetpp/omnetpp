import "txnode", "sinknode", "sink";

//
// Model for testing animation of broadcasts
//

//
// A: both tx and sink are compound modules
//
module BroadcastTestA
    submodules:
        tx: TxNode;
        sink: SinkNode[10];
    connections:
        for i=0..9 do
            tx.out++ --> sink[i].in++;
        endfor
endmodule

network broadcastTestA : BroadcastTestA
endnetwork


//
// B: tx is compound, sink is simple module
//
module BroadcastTestB
    submodules:
        tx: TxNode;
        sink: Sink[10];
    connections:
        for i=0..9 do
            tx.out++ --> sink[i].in++;
        endfor
endmodule

network broadcastTestB : BroadcastTestB
endnetwork


