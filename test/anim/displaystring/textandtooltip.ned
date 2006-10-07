import "dummy";
import "icons";

module TextAndTooltip
    submodules:
        icon1: Dummy;
            display: "p=100,100;i=block/sink;t=some lines\nof text\n(default pos);tt=This is a\ntooltip for txt1.";
        icon2: Dummy;
            display: "p=200,100;i=block/sink;t=some lines\nof text\n(left),l;tt=This is a\ntooltip for txt2.";
        icon3: Dummy;
            display: "p=300,100;i=block/sink;t=some lines\nof text\n(top),t;tt=This is a\ntooltip for txt3.";
        icon4: Dummy;
            display: "p=400,100;i=block/sink;t=some lines\nof text\n(right),r;tt=This is a\ntooltip for txt4.";
        xicon1: Dummy;
            display: "p=100,200;i=block/sink;i2=status/busy;t=some lines\nof text\n(default pos);tt=This is a\ntooltip for txt1.";
        xicon2: Dummy;
            display: "p=200,200;i=block/sink;i2=status/busy;t=some lines\nof text\n(left),l;tt=This is a\ntooltip for txt2.";
        xicon3: Dummy;
            display: "p=300,200;i=block/sink;i2=status/busy;t=some lines\nof text\n(top),t;tt=This is a\ntooltip for txt3.";
        xicon4: Dummy;
            display: "p=400,200;i=block/sink;i2=status/busy;t=some lines\nof text\n(right),r;tt=This is a\ntooltip for txt4.";
        range1: Dummy;
            display: "p=100,300;i=block/sink;i2=status/busy;r=80,white;t=some lines\nof text\n(default pos);tt=This is a\ntooltip for txt1.";
        range2: Dummy;
            display: "p=200,300;i=block/sink;i2=status/busy;r=80,white;t=some lines\nof text\n(left),l;tt=This is a\ntooltip for txt2.";
        range3: Dummy;
            display: "p=300,300;i=block/sink;i2=status/busy;r=80,white;t=some lines\nof text\n(top),t;tt=This is a\ntooltip for txt3.";
        range4: Dummy;
            display: "p=400,300;i=block/sink;i2=status/busy;r=80,white;t=some lines\nof text\n(right),r;tt=This is a\ntooltip for txt4.";
endmodule

