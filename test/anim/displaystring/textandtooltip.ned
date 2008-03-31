module TextAndTooltip
{
    @display("bgb=517,387");
    submodules:
        referencePic: Dummy {
            @display("p=479,339;i=device/scanner;bgi=textandtooltip;bgb=522,414,,,0");
        }
        icon1: Dummy {
            parameters:
                @display("p=100,100;i=block/sink;t=some lines\nof text\n(default pos);tt=This is a\ntooltip for txt1.");
        }
        icon2: Dummy {
            parameters:
                @display("p=200,100;i=block/sink;t=some lines\nof text\n(left),l;tt=This is a\ntooltip for txt2.");
        }
        icon3: Dummy {
            parameters:
                @display("p=300,100;i=block/sink;t=some lines\nof text\n(top),t;tt=This is a\ntooltip for txt3.");
        }
        icon4: Dummy {
            parameters:
                @display("p=400,100;i=block/sink;t=some lines\nof text\n(right),r;tt=This is a\ntooltip for txt4.");
        }
        xicon1: Dummy {
            parameters:
                @display("p=100,200;i=block/sink;i2=status/busy;t=some lines\nof text\n(default pos);tt=This is a\ntooltip for txt1.");
        }
        xicon2: Dummy {
            parameters:
                @display("p=200,200;i=block/sink;i2=status/busy;t=some lines\nof text\n(left),l;tt=This is a\ntooltip for txt2.");
        }
        xicon3: Dummy {
            parameters:
                @display("p=300,200;i=block/sink;i2=status/busy;t=some lines\nof text\n(top),t;tt=This is a\ntooltip for txt3.");
        }
        xicon4: Dummy {
            parameters:
                @display("p=400,200;i=block/sink;i2=status/busy;t=some lines\nof text\n(right),r;tt=This is a\ntooltip for txt4.");
        }
        range1: Dummy {
            parameters:
                @display("p=100,300;i=block/sink;i2=status/busy;r=80,white;t=some lines\nof text\n(default pos);tt=This is a\ntooltip for txt1.");
        }
        range2: Dummy {
            parameters:
                @display("p=200,300;i=block/sink;i2=status/busy;r=80,white;t=some lines\nof text\n(left),l;tt=This is a\ntooltip for txt2.");
        }
        range3: Dummy {
            parameters:
                @display("p=300,300;i=block/sink;i2=status/busy;r=80,white;t=some lines\nof text\n(top),t;tt=This is a\ntooltip for txt3.");
        }
        range4: Dummy {
            parameters:
                @display("p=400,300;i=block/sink;i2=status/busy;r=80,white;t=some lines\nof text\n(right),r;tt=This is a\ntooltip for txt4.");
        }
}

