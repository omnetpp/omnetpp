//ERROR: wrong source gate for connection: 'in' is not a vector gate
module Network
    gates:
        in: in;
    connections:
        in[0] --> b.in;
endmodule
