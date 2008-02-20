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
                c.p$o --> p$o;
                c.p$i <-- p$i;
                for i=0..sizeof(pv1)-1 {
                    c.pv1$o[i] --> pv1$o[i];
                    c.pv1$i[i] <-- pv1$i[i];
                }
                for i=0..sizeof(pv2)-1 {
                    c.pv2$o++ --> pv2$o++;
                    c.pv2$i++ <-- pv2$i++;
                };
        }

    submodules:
        dump: Dump;

        node: Node {
            gates:
                pv1[5];
                pv2[5];
        }

    connections allowunconnected:
}


