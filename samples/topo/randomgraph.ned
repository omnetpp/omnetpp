
simple RndNode 
    parameters:
        x,y;
    gates: 
        in: in[]; 
        out: out[]; 
endsimple 


module RandomGraph 
    parameters: 
        count : numeric const, 
        connectedness : numeric; // 0.0<x<1.0
    submodules: 
        node: RndNode[count]; 
            parameters: 
                x = uniform(0,350),
                y = uniform(0,350);
            gatesizes: 
                in[count], 
                out[count]; 
            display: "p=20,20,exact,$x,$y;i=ball_vs";
    connections nocheck: 
        for i=0..count-1, j=0..count-1 do 
            node[i].out[j] --> node[j].in[i] if i!=j && uniform(0,1) < connectedness; 
        endfor; 
endmodule 


network randomGraph : RandomGraph 
    parameters: 
        count = 10, 
        connectedness = 0.3; 
endnetwork 

