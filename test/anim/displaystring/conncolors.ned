import "dummy";

module ConnColors
{
    submodules:
        src1: Dummy {
            parameters:
                @display("i=misc/node2_s;p=90,72");
        };
        dest1: Dummy {
            parameters:
                @display("i=misc/node2_s;p=240,72");
        };
        src2: Dummy {
            parameters:
                @display("i=misc/node2_s;p=90,130");
        };
        dest2: Dummy {
            parameters:
                @display("i=misc/node2_s;p=240,130");
        };
        src3: Dummy {
            parameters:
                @display("i=misc/node2_s;p=90,194");
        };
        dest3: Dummy {
            parameters:
                @display("i=misc/node2_s;p=240,194");
        };
        src4: Dummy {
            parameters:
                @display("i=misc/node2_s;p=90,248");
        };
        dest4: Dummy {
            parameters:
                @display("i=misc/node2_s;p=240,248");
        };
        src5: Dummy {
            parameters:
                @display("i=misc/node2_s;p=90,320");
        };
        dest5: Dummy {
            parameters:
                @display("i=misc/node2_s;p=240,320");
        };
    connections allowunconnected:
        src1.out++ --> { @display("t=default"); } --> dest1.in++;
        src2.out++ --> { @display("ls=,5;t=5px thick"); } --> dest2.in++;
        src3.out++ --> { @display("ls=yellow;t=yellow"); } --> dest3.in++;
        src4.out++ --> { @display("ls=yellow,5;t=yellow 5px"); } --> dest4.in++;
        src5.out++ --> { @display("ls=,0;t=invisible (0px)"); } --> dest5.in++;
}

