//ERROR: source gate 'in' is an input gate
simple Node
    gates:
        in: in;
endsimple

module Network
    submodules:
        a: Node;
    connections:
        a.in --> b.in;
endmodule
