//ERROR: wrong source gate for connection: 'out' is not a vector gate
simple Node
    gates:
        out: out;
endsimple

module Network
    submodules:
        a: Node;
    connections:
        a.out[0] --> b.in;
endmodule
