//ERROR: wrong destination gate for connection: extra gate index or '++'
module Network
    gates:
        out: out;
    connections:
        out++ <-- b.out;
endmodule
