simple HexGridNode
    parameters:
        x,y;
     gates:
        in:  n,nw,sw;
        out: s,se,ne;
endsimple


module HexGrid
    parameters:
        rows,
        cols;
    submodules:
        node  : HexGridNode[ rows * cols ];
            parameters:
                x=200*(index%cols)+100*(floor(index/cols)%2),
                y=60*floor(index/cols);
            display:
                "p=50,50,exact,$x,$y";
    connections nocheck:
        for i = 0 .. rows*cols-1 do
            node[i].n <-- node[i-2*cols].s if i+1>2*cols;
            node[i].nw <-- node[i-cols-(i/cols+1)%2].se if i%(2*cols)!=0 && i+1>cols;
            node[i].sw <-- node[i+cols-1+(i/cols)%2].ne if i%(2*cols)!=0 && i<rows*cols-cols;
        endfor;
endmodule


network hexGrid : HexGrid
    parameters:
        rows = 7,
        cols = 3;
endnetwork

