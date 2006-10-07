//ERROR: wrong destination gate for connection: input gate expected but 'out' is an output gate
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
