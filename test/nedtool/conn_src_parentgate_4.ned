//ERROR: source gate 'out' is an output gate
simple Node
endsimple

module Network
    gates:
        out: out;
    connections:
        out --> b.in;
endmodule
