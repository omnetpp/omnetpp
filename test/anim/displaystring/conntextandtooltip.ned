import
    "dummy";

module ConnTextAndTooltip
    submodules:
        src1: Dummy;
            display: "i=misc/node2_s;p=93,72";
        dest1: Dummy;
            display: "i=misc/node2_s;p=241,72";
        src2: Dummy;
            display: "i=misc/node2_s;p=241,138";
        dest2: Dummy;
            display: "i=misc/node2_s;p=93,138";
        src3: Dummy;
            display: "i=misc/node2_s;p=364,74";
        dest3: Dummy;
            display: "i=misc/node2_s;p=364,184";
        src4: Dummy;
            display: "i=misc/node2_s;p=486,183";
        dest4: Dummy;
            display: "i=misc/node2_s;p=486,72";
        src5: Dummy;
            display: "i=misc/node2_s;p=72,347";
        dest5: Dummy;
            display: "i=misc/node2_s;p=191,259";
        src6: Dummy;
            display: "i=misc/node2_s;p=300,258";
        dest6: Dummy;
            display: "i=misc/node2_s;p=182,348";
        src7: Dummy;
            display: "i=misc/node2_s;p=558,329";
        dest7: Dummy;
            display: "i=misc/node2_s;p=409,258";
        src8: Dummy;
            display: "i=misc/node2_s;p=525,255";
        dest8: Dummy;
            display: "i=misc/node2_s;p=673,327";
    connections nocheck:
        src1.out++ --> dest1.in++ display "t=some text;tt=some tooltip";
        src2.out++ --> dest2.in++ display "t=some text;tt=some tooltip";
        src3.out++ --> dest3.in++ display "t=some text;tt=some tooltip";
        src4.out++ --> dest4.in++ display "t=some text;tt=some tooltip";
        src5.out++ --> dest5.in++ display "t=some text;tt=some tooltip";
        src6.out++ --> dest6.in++ display "t=some text;tt=some tooltip";
        src7.out++ --> dest7.in++ display "t=some text;tt=some tooltip";
        src8.out++ --> dest8.in++ display "t=some text;tt=some tooltip";
endmodule

