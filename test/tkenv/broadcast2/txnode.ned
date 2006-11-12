import "tx";

module TxNode
    gates:
        out: out[];
    submodules:
        tx: Tx;
          gatesizes:
              out[sizeof(out)];
    connections:
        for i=0..sizeof(out)-1 do
            tx.out[i] --> out[i];
        endfor
endmodule

