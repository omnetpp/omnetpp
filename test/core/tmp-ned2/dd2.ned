// bidirectional connection to parent, with scalar gate, gate indexing and gate++
network DD2 {
    types:
        module Core
        {
            gates:
                inout p;
                inout pv1[];
                inout pv2[];
            connections allowunconnected:
        }

        module Node
        {
            gates:
                inout p;
                inout pv1[];
                inout pv2[];
            submodules:
                c: Core {
                    gates:
                        pv1[sizeof(pv1)];
                };
            connections:
                p <--> c.p;
                for i=0..sizeof(pv1)-1 {
                    c.pv1[i] <--> pv1[i];
                }
                for i=0..sizeof(pv2)-1 {
                    c.pv2++ <--> pv2++;
                };
        }

    submodules:
        node: Node {
            gates:
                pv1[5];
                pv2[5];
        }

    connections allowunconnected:
}


