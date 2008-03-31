module ConnTextAndTooltip
{
    @display("bgb=654,387");
    submodules:
        referencePic: Dummy {
            @display("p=602,345;i=device/scanner;bgi=conntextandtooltip;bgb=658,414,,,0");
        }
        src1: Dummy {
            parameters:
                @display("i=misc/node2_s;p=35,35");
        }
        dest1: Dummy {
            parameters:
                @display("i=misc/node2_s;p=183,35");
        }
        src2: Dummy {
            parameters:
                @display("i=misc/node2_s;p=182,81");
        }
        dest2: Dummy {
            parameters:
                @display("i=misc/node2_s;p=34,81");
        }
        src3: Dummy {
            parameters:
                @display("i=misc/node2_s;p=253,35");
        }
        dest3: Dummy {
            parameters:
                @display("i=misc/node2_s;p=253,146");
        }
        src4: Dummy {
            parameters:
                @display("i=misc/node2_s;p=300,146");
        }
        dest4: Dummy {
            parameters:
                @display("i=misc/node2_s;p=300,35");
        }
        src5: Dummy {
            parameters:
                @display("i=misc/node2_s;p=346,123");
        }
        dest5: Dummy {
            parameters:
                @display("i=misc/node2_s;p=465,35");
        }
        src6: Dummy {
            parameters:
                @display("i=misc/node2_s;p=574,34");
        }
        dest6: Dummy {
            parameters:
                @display("i=misc/node2_s;p=456,124");
        }
        src7: Dummy {
            parameters:
                @display("i=misc/node2_s;p=495,243");
        }
        dest7: Dummy {
            parameters:
                @display("i=misc/node2_s;p=346,172");
        }
        src8: Dummy {
            parameters:
                @display("i=misc/node2_s;p=462,169");
        }
        dest8: Dummy {
            parameters:
                @display("i=misc/node2_s;p=610,241");
        }
        src9: Dummy {
            parameters:
                @display("i=misc/node2_s;p=35,266");
        }
        dest9: Dummy {
            parameters:
                @display("i=misc/node2_s;p=183,266");
        }
        src10: Dummy {
            parameters:
                @display("i=misc/node2_s;p=35,314");
        }
        dest10: Dummy {
            parameters:
                @display("i=misc/node2_s;p=183,314");
        }
        src11: Dummy {
            parameters:
                @display("i=misc/node2_s;p=272,314");
        }
        dest11: Dummy {
            parameters:
                @display("i=misc/node2_s;p=420,314");
        }
        dest12: Dummy {
            parameters:
                @display("i=misc/node2_s;p=420,362");
        }
        src12: Dummy {
            parameters:
                @display("i=misc/node2_s;p=272,266");
        }
    connections allowunconnected:
        src1.out++ --> { @display("t=some text;tt=some tooltip"); } --> dest1.in++;
        src2.out++ --> { @display("t=some text;tt=some tooltip"); } --> dest2.in++;
        src3.out++ --> { @display("t=some text;tt=some tooltip"); } --> dest3.in++;
        src4.out++ --> { @display("t=some text;tt=some tooltip"); } --> dest4.in++;
        src5.out++ --> { @display("t=some text;tt=some tooltip"); } --> dest5.in++;
        src6.out++ --> { @display("t=some text;tt=some tooltip"); } --> dest6.in++;
        src7.out++ --> { @display("t=some text;tt=some tooltip"); } --> dest7.in++;
        src8.out++ --> { @display("t=some text;tt=some tooltip"); } --> dest8.in++;
        src9.out++ --> { @display("t=left aligned,l;tt=some tooltip"); } --> dest9.in++;
        src10.out++ --> { @display("t=right aligned,r;tt=some tooltip"); } --> dest10.in++;
        src11.out++ --> { @display("t=right aligned,r;tt=some tooltip"); } --> dest12.in++;
        src12.out++ --> { @display("t=left aligned,l;tt=some tooltip"); } --> dest11.in++;
}

