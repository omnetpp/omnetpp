//ERROR: wrong destination gate for connection: 'out' is a vector gate
module Network
    gates:
        out: out[];
    connections:
        out <-- b.out;
endmodule
