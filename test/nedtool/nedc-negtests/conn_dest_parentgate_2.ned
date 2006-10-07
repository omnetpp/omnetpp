//ERROR: wrong destination gate for connection: missing gate index ('out' is a vector gate)
module Network
    gates:
        out: out[];
    connections:
        out <-- b.out;
endmodule
