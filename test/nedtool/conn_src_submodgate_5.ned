//ERROR: wrong source gate for connection: output gate expected but 'in' is an input gate
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
