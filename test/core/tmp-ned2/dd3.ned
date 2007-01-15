// connecting $o/$i to parent, with scalar gate, gate indexing and gate++
network DD3 {
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
                        pv1[sizeof(pv2)];
                };
            connections:
                p$o --> c.p$i;
                p$i <-- c.p$o;
                for i=0..sizeof(port)-1 {
                    c.pv1$o[i] --> pv1$i[i];
                    c.pv1$i[i] <-- pv1$o[i];
                    c.pv2$o++ --> pv2$i++;
                    c.pv2$i++ <-- pv2$o++;
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


