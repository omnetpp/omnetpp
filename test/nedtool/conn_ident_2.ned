//ERROR: compound module has no parameter named 'j'
simple Node
    gates:
        in: in[];
        out: out[];
endsimple

module Network
    submodules:
        a: Node;
        b: Node;
    connections nocheck:
        a.out[j] --> b.in[k];
endmodule
