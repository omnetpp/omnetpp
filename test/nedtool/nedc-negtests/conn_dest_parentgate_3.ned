//ERROR: wrong destination gate for connection: extra gate index or '++' ('out' is not a vector gate)
module Network
    gates:
        out: out;
    connections:
        out[0] <-- b.out;
endmodule
