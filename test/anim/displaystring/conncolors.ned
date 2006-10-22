import
    "dummy";

module ConnColors
    submodules:
        src1: Dummy;
            display: "i=misc/node2_s;p=90,72";
        dest1: Dummy;
            display: "i=misc/node2_s;p=240,72";
        src2: Dummy;
            display: "i=misc/node2_s;p=90,130";
        dest2: Dummy;
            display: "i=misc/node2_s;p=240,130";
        src3: Dummy;
            display: "i=misc/node2_s;p=90,194";
        dest3: Dummy;
            display: "i=misc/node2_s;p=240,194";
        src4: Dummy;
            display: "i=misc/node2_s;p=90,248";
        dest4: Dummy;
            display: "i=misc/node2_s;p=240,248";
        src5: Dummy;
            display: "i=misc/node2_s;p=90,320";
        dest5: Dummy;
            display: "i=misc/node2_s;p=240,320";
    connections nocheck:
        src1.out++ --> dest1.in++ display "t=default";
        src2.out++ --> dest2.in++ display "o=,5;t=5px thick";
        src3.out++ --> dest3.in++ display "o=yellow;t=yellow";
        src4.out++ --> dest4.in++ display "o=yellow,5;t=yellow 5px";
        src5.out++ --> dest5.in++ display "o=,0;t=invisible (0px)";
endmodule

