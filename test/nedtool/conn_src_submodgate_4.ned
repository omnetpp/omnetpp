//ERROR: wrong source gate for connection: 'in' is an input gate
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
