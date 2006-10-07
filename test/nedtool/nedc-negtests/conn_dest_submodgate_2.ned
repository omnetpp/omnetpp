//ERROR: wrong destination gate for connection: missing gate index ('in' is a vector gate)
simple Node
    gates:
        in: in[];
endsimple

module Network
    submodules:
        a: Node;
    connections:
        a.in <-- b.out;
endmodule
