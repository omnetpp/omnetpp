network Wireless extends Base
{
    parameters:
        int numNodes = default(20) @prompt("Number of nodes");
    submodules:
        vec[numNodes]: Node {
            parameters:
                @display("i=ball2,gold,100;is=vs");
        };
    connections allowunconnected:
}

