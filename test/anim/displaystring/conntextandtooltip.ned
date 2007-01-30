import "dummy";

module ConnTextAndTooltip
{
    submodules:
        src1: Dummy {
            parameters:
                @display("i=misc/node2_s;p=93,72");
        };
        dest1: Dummy {
            parameters:
                @display("i=misc/node2_s;p=241,72");
        };
        src2: Dummy {
            parameters:
                @display("i=misc/node2_s;p=241,138");
        };
        dest2: Dummy {
            parameters:
                @display("i=misc/node2_s;p=93,138");
        };
        src3: Dummy {
            parameters:
                @display("i=misc/node2_s;p=364,74");
        };
        dest3: Dummy {
            parameters:
                @display("i=misc/node2_s;p=364,184");
        };
        src4: Dummy {
            parameters:
                @display("i=misc/node2_s;p=486,183");
        };
        dest4: Dummy {
            parameters:
                @display("i=misc/node2_s;p=486,72");
        };
        src5: Dummy {
            parameters:
                @display("i=misc/node2_s;p=72,347");
        };
        dest5: Dummy {
            parameters:
                @display("i=misc/node2_s;p=191,259");
        };
        src6: Dummy {
            parameters:
                @display("i=misc/node2_s;p=300,258");
        };
        dest6: Dummy {
            parameters:
                @display("i=misc/node2_s;p=182,348");
        };
        src7: Dummy {
            parameters:
                @display("i=misc/node2_s;p=558,329");
        };
        dest7: Dummy {
            parameters:
                @display("i=misc/node2_s;p=409,258");
        };
        src8: Dummy {
            parameters:
                @display("i=misc/node2_s;p=525,255");
        };
        dest8: Dummy {
            parameters:
                @display("i=misc/node2_s;p=673,327");
        };
    connections allowunconnected:
        src1.out++ --> { @display("t=some text;tt=some tooltip"); } --> dest1.in++;
        src2.out++ --> { @display("t=some text;tt=some tooltip"); } --> dest2.in++;
        src3.out++ --> { @display("t=some text;tt=some tooltip"); } --> dest3.in++;
        src4.out++ --> { @display("t=some text;tt=some tooltip"); } --> dest4.in++;
        src5.out++ --> { @display("t=some text;tt=some tooltip"); } --> dest5.in++;
        src6.out++ --> { @display("t=some text;tt=some tooltip"); } --> dest6.in++;
        src7.out++ --> { @display("t=some text;tt=some tooltip"); } --> dest7.in++;
        src8.out++ --> { @display("t=some text;tt=some tooltip"); } --> dest8.in++;
}

