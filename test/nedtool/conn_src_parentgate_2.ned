//ERROR: source gate 'in' is a vector gate
simple Node
endsimple

module Network
    gates:
        in: in[];
    connections:
        in --> b.in;
endmodule
