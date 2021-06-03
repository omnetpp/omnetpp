
module TxNode
{
    gates:
        output out[];
    submodules:
        tx: Tx {
            gates:
                out[sizeof(parent.out)];
        };
    connections:
        for i=0..sizeof(out)-1 {
            tx.out[i] --> out[i];
        };
}

