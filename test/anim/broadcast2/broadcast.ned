
//
// Model for testing animation of broadcasts
//

//
// A: both tx and sink are compound modules
//
network BroadcastTestA
{
    submodules:
        tx: TxNode;
        sink[10]: SinkNode;
    connections:
        for i=0..9 {
            tx.out++ --> sink[i].in++;
        };
}



//
// B: tx is compound, sink is simple module
//
network BroadcastTestB
{
    submodules:
        tx: TxNode;
        sink[10]: Sink;
    connections:
        for i=0..9 {
            tx.out++ --> sink[i].in++;
        };
}



