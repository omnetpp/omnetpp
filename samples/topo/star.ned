
simple StarNode
    gates:
        in: in;
endsimple

simple HubNode
    gates:
        out: out[];
endsimple

module Star
    parameters:
        n : numeric const;
    submodules:
        node: StarNode[n];
            display: "p=50,50,ring,150,150;i=comp2_s";
        hub: HubNode;
            gatesizes:
                out[n];
            display: "p=200,200;i=switch2";
    connections nocheck:
        for i=0..n-1 do
            hub.out[i] --> node[i].in;
        endfor;
endmodule


network star : Star
    parameters:
        n = input(11,"Number of stations");
endnetwork

