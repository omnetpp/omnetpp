//ERROR: wrong destination gate for connection: 'in' is a vector gate
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
