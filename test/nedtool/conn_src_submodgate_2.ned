//ERROR: wrong source gate for connection: missing gate index ('out' is a vector gate)
simple Node
    gates:
        out: out[];
endsimple

module Network
    submodules:
        a: Node;
    connections:
        a.out --> b.in;
endmodule
