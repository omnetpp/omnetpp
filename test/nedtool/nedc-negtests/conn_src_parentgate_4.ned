//ERROR: wrong source gate: '++' cannot be used on parent module's gates
module Network
    gates:
        in: in[];
    connections:
        in++ --> b.in;
endmodule
