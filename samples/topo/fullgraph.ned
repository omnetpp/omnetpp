
simple FullGraphNode 
    gates: 
        in: in[]; 
        out: out[]; 
endsimple 


module FullGraph 
    parameters: 
        count : numeric const;
    submodules: 
        node: FullGraphNode[count]; 
            gatesizes: 
                in[count], 
                out[count]; 
    connections nocheck: 
        for i=0..count-1, j=0..count-1 do 
            node[i].out[j] --> node[j].in[i] if i!=j; 
        endfor; 
endmodule 


network fullGraph : FullGraph 
    parameters: 
        count = 10;
endnetwork 

