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
        icon: string;
    submodules:
        node: Node[30];
            parameters:
                x = nodeX,
                y = nodeY,
                ic = icon;
            display: "p=$x,$y;i=$ic";
endmodule

module NedParams
    submodules:
        randomPositions: RandomPositions
            parameters:
                nodeX = uniform(50,450),
                nodeY = uniform(50,300),
                icon = "place/circle_s";
endmodule


