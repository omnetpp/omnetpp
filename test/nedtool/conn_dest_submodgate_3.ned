//ERROR: wrong destination gate for connection: extra gate index ('in' is not a vector gate)
simple Node
    gates:
        in: in;
endsimple

module Network
    submodules:
        a: Node;
    connections:
        a.in[0] <-- b.out;
endmodule
