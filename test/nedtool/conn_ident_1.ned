//ERROR: no such param or loop var
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
        for i=0..10 do
            a.out[i] --> b.in[j];
        endfor;
endmodule
