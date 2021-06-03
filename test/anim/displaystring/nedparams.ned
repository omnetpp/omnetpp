module Node
{
    parameters:
        double x;
        double y;
        string ic;
    connections allowunconnected:
}

module RandomPositions
{
    parameters:
        volatile double nodeX;
        volatile double nodeY;
        string icon;
        string color;
        volatile double perc;
    submodules:
        node[30]: Node {
            parameters:
                x = parent.nodeX;
                y = parent.nodeY;
                ic = parent.icon;
                // NOTE: color and perc params are NOT taken by the submodule,
                // so the parent module's value will be used! And because perc
                // is non-const (i.e. volatile), each time a different value will be
                // generated.
                @display("p=$x,$y;i=$ic,$color,$perc;tt=IMPORTANT: Attention testers: Re-layouting should change the shade but not the position!");
        };
}

module NedParams
{
    submodules:
        randomPositions: RandomPositions {
            parameters:
                nodeX = uniform(50, 450);
                nodeY = uniform(50, 300);
                icon = "misc/node_s";
                color = "green2";
                perc = uniform(0, 60);
        };
}


