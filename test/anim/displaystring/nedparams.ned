module Node
    parameters:
        x: numeric const,
        y: numeric const,
        ic: string;
    connections nocheck:
endmodule

module RandomPositions
    parameters:
        nodeX: numeric,  // non-const
        nodeY: numeric,  // non-const
        icon: string,
        color: string,
        perc: numeric;   // non-const
    submodules:
        node: Node[30];
            parameters:
                x = nodeX,
                y = nodeY,
                ic = icon;
            // NOTE: color and perc params are NOT taken by the submodule,
            // so the parent module's value will be used! And because perc
            // is non-const (i.e. volatile), each time a different value will be
            // generated.
            display: "p=$x,$y;i=$ic,$color,$perc;tt=IMPORTANT: Attention testers: Re-layouting should change the shade but not the position!";
endmodule

module NedParams
    submodules:
        randomPositions: RandomPositions
            parameters:
                nodeX = uniform(50,450),
                nodeY = uniform(50,300),
                icon = "misc/node_s",
                color = "green2",
                perc = uniform(0,60);
endmodule


