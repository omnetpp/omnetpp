import "dummy";
import "icons";

module TextAndTooltip
    submodules:
        txt1: Dummy;
            display: "p=100,100;i=block/sink;t=some lines\nof text\n(default pos);tt=This is a\ntooltip for txt1.";
        txt2: Dummy;
            display: "p=200,100;i=block/sink;t=some lines\nof text\n(left),l;tt=This is a\ntooltip for txt2.";
        txt3: Dummy;
            display: "p=300,100;i=block/sink;t=some lines\nof text\n(top),t;tt=This is a\ntooltip for txt4.";
        txt4: Dummy;
            display: "p=400,100;i=block/sink;t=some lines\nof text\n(right),r;tt=This is a\ntooltip for txt3.";
endmodule

