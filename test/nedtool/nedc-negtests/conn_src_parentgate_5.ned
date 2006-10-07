//ERROR: wrong source gate for connection: input gate expected but 'out' is an output gate
module Network
    gates:
        out: out;
    connections:
        out --> b.in;
endmodule
