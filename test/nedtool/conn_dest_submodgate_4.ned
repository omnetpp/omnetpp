//ERROR: wrong destination gate for connection: 'out' is an output gate
simple Node
    gates:
        out: out;
endsimple

module Network
    submodules:
        a: Node;
    connections:
        a.out <-- b.out;
endmodule
