//ERROR: wrong destination gate for connection: 'in' is an input gate
module Network
    gates:
        in: in;
    connections:
        in <-- b.out;
endmodule
