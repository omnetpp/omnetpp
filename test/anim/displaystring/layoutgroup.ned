module LayoutGroup
{
    submodules:
        node[23]: Dummy; // note: layout is defined in omnetpp.ini
}

module LayoutGroup2
{
    submodules:
        // row anchored at (60,60): expected at (60,60), (130,60), (200,60)
        a: Dummy {
            @display("p=60,60,row,70;g=top");
        }
        c: Dummy {
            @display("p=60,60,row,70;g=top");
        }
        b: Dummy {
            @display("p=60,60,row,70;g=top");
        }

        // ring centered at (150,220), radius 70: expected at
        // (150,150), (80,220), (150,290), (220,220)
        n: Dummy {
            @display("p=163,251,cring,70;g=circle");
        }
        e: Dummy {
            @display("p=163,251,cring,70;g=circle");
        }
        s: Dummy {
            @display("p=163,251,cring,70;g=circle");
        }
        // 3x2 matrix at (350,60): expected at (350,60), (410,60), (470,60),
        // (350,120), (410,120), (470,120)
        m1: Dummy {
            @display("p=293,81,matrix,3,60,60;g=grid");
        }
        m2: Dummy {
            @display("p=293,81,matrix,3,60,60;g=grid");
        }
        m3: Dummy {
            @display("p=293,81,matrix,3,60,60;g=grid");
        }
        m4: Dummy {
            @display("p=293,81,matrix,3,60,60;g=grid");
        }
        m5: Dummy {
            @display("p=293,81,matrix,3,60,60;g=grid");
        }
        m6: Dummy {
            @display("p=293,81,matrix,3,60,60;g=grid");
        }

        // group without an explicit position: the auto-layouter should place the
        // three modules as one rigid column, 60 units apart
        f1: Dummy {
            @display("p=353,233,column,60;g=floating");
        }
        f2: Dummy {
            @display("p=353,233,column,60;g=floating");
        }
        f3: Dummy {
            @display("p=353,233,column,60;g=floating");
        }
}

