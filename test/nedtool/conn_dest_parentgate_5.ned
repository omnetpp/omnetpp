//ERROR: wrong destination gate for connection: output gate expected but 'in' is an input gate
module Network
    gates:
        in: in;
    connections:
        in <-- b.out;
endmodule
