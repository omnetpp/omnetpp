//ERROR: wrong destination gate: '++' cannot be used on parent module's gates
module Network
    gates:
        out: out[];
    connections:
        out++ <-- b.out;
endmodule
