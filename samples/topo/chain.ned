
simple ChainNode
    gates:
        in: left;
        out: right;
endsimple


module Chain
    parameters:
        n : numeric const;
    submodules:
        node: ChainNode[n];
    connections nocheck:
        for i=0..n-2 do
            node[i].right --> node[i+1].left;
        endfor;
endmodule


network chain : Chain
    parameters:
        n = 5;
endnetwork

