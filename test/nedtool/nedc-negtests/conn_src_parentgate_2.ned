//ERROR: wrong source gate for connection: missing gate index ('in' is a vector gate)
module Network
    gates:
        in: in[];
    connections:
        in --> b.in;
endmodule
