module StarNode
{
    parameters:
        @display("i=device/pc2_s");
    gates:
        input in;
    connections allowunconnected:
}


module HubNode
{
    parameters:
        @display("i=device/switch");
    gates:
        output out[];
    connections allowunconnected:
}


network Star extends Base
{
    parameters:
        int n = default(11) @prompt("Number of stations");
    submodules:
        node[n]: StarNode {
        };
        hub: HubNode {
            gates:
                out[n];
        };
    connections allowunconnected:
        for i=0..n-1 {
            hub.out[i] --> node[i].in;
        };
}

