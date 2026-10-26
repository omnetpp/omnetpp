// Tests connection routing with cardinal direction constraints.
// New syntax: m=<srcDir>,<destDir> where each is n/e/s/w or empty.
// Also tests bundling (multiple connections between the same module pair).

//
// Submodule-to-parent connections (containment case)
//
module ConnRoutingToParent
{
    parameters:
        @display("bgb=500,400");
        @figure[note](type=label;pos=10,20;anchor=nw;text="Move modules around (shift+drag) to test all relative positions!\nTwo arrows per pair! Turn on 'Distribute Connection Arrows' (context menu) to see them.");
    gates:
        input in[8] @loose;
        output out[8] @loose;
    submodules:
        // center module: unidir connections to all four parent edges
        center: Dummy { @display("p=250,200;is=s"); }

        // side modules: auto-pick nearest edge (unidir)
        top:    Dummy { @display("p=300,60;is=s"); }
        bottom: Dummy { @display("p=300,340;is=s"); }
        left:   Dummy { @display("p=80,250;is=s"); }
        right:  Dummy { @display("p=420,250;is=s"); }

    connections allowunconnected:
        // explicit direction to parent edge (unidir)
        center.out++ --> { @display("m=n;t=n"); } --> out++;
        center.out++ --> { @display("m=e;t=e"); } --> out++;
        center.out++ --> { @display("m=s;t=s"); } --> out++;
        center.out++ --> { @display("m=w;t=w"); } --> out++;

        center.in++ <-- { @display("m=n;t=n"); } <-- in++;
        center.in++ <-- { @display("m=e;t=e"); } <-- in++;
        center.in++ <-- { @display("m=s;t=s"); } <-- in++;
        center.in++ <-- { @display("m=w;t=w"); } <-- in++;

        // auto (no constraint, unidir) -- should pick nearest edge
        top.out++ --> { @display("t=auto"); } --> out++;
        bottom.out++ --> { @display("t=auto"); } --> out++;
        left.out++ --> { @display("t=auto"); } --> out++;
        right.out++ --> { @display("t=auto"); } --> out++;

        top.in++ <-- { @display("t=auto"); } <-- in++;
        bottom.in++ <-- { @display("t=auto"); } <-- in++;
        left.in++ <-- { @display("t=auto"); } <-- in++;
        right.in++ <-- { @display("t=auto"); } <-- in++;
}

//
// Submodule-to-parent connections (containment case)
//
module ConnRoutingToParentBidi
{
    parameters:
        @display("bgb=500,400");
        @figure[note](type=label;pos=10,20;anchor=nw;text="Move modules around (shift+drag) to test all relative positions!\nTwo arrows per pair! Turn on 'Distribute Connection Arrows' (context menu) to see them.");
    gates:
        inout twoway[8] @loose;
    submodules:
        // center module: connections to all four parent edges
        center: Dummy { @display("p=250,200;is=s"); }

        // side modules: auto-pick nearest edge (unidir)
        top:    Dummy { @display("p=300,60;is=s"); }
        bottom: Dummy { @display("p=300,340;is=s"); }
        left:   Dummy { @display("p=80,250;is=s"); }
        right:  Dummy { @display("p=420,250;is=s"); }

    connections allowunconnected:
        // auto (no constraint) -- should pick nearest edge
        top.twoway++ <--> { @display("t=auto"); } <--> twoway++;
        bottom.twoway++ <--> { @display("t=auto"); } <--> twoway++;
        left.twoway++ <--> { @display("t=auto"); } <--> twoway++;
        right.twoway++ <--> { @display("t=auto"); } <--> twoway++;

        // explicit direction to parent edge (bidir)
        center.twoway++ <--> { @display("m=n;t=n"); } <--> twoway++;
        center.twoway++ <--> { @display("m=e;t=e"); } <--> twoway++;
        center.twoway++ <--> { @display("m=s;t=s"); } <--> twoway++;
        center.twoway++ <--> { @display("m=w;t=w"); } <--> twoway++;
}

//
// Connections between two gates of the compound module itself (input -> output),
// not touching any submodule. Each must be drawn across the whole module, from
// one edge to the opposite one, never ending in its middle.
//
module ConnRoutingThroughParent
{
    parameters:
        @display("bgb=500,300");
        @figure[note](type=label;pos=10,20;anchor=nw;text="Lines must span the module from edge to edge.\nAuto picks the shorter axis (vertical here); m=h/m=v force the axis.\nTurn on 'Distribute Connection Arrows' (context menu) to spread them.");
    gates:
        input in[6] @loose;
        output out[6] @loose;
        inout twoway[2] @loose;
    connections allowunconnected:
        // auto (unidir)
        in++ --> { @display("t=auto"); } --> out++;
        in++ --> { @display("t=auto"); } --> out++;

        // explicit axis (unidir)
        in++ --> { @display("m=h;t=m=h"); } --> out++;
        in++ --> { @display("m=h;t=m=h"); } --> out++;
        in++ --> { @display("m=v;t=m=v"); } --> out++;
        in++ --> { @display("m=v;t=m=v"); } --> out++;

        // bidir
        twoway[0] <--> { @display("t=bidir"); } <--> twoway[1];
}

//
// Full cartesian product: 3 srcDir x 3 destDir (-, h, v)
// 2 unidir connections per pair (to test bundling offsets).
// Modules positioned NW-SE within each pair.
//
module ConnRoutingCartesian
{
    parameters:
        @display("bgb=620,530");
        @figure[note](type=label;pos=10,20;anchor=nw;text="Move modules around (shift+drag) to test all relative positions!\nTwo arrows per pair! Turn on 'Distribute Connection Arrows' (context menu) to see them.");
    submodules:
        // src=- (unconstrained), dest=-/h/v
        xx1: Dummy { @display("p=60,70;is=s"); }
        xx2: Dummy { @display("p=140,130;is=s"); }
        xh1: Dummy { @display("p=260,70;is=s"); }
        xh2: Dummy { @display("p=340,130;is=s"); }
        xv1: Dummy { @display("p=460,70;is=s"); }
        xv2: Dummy { @display("p=540,130;is=s"); }

        // src=h, dest=-/h/v
        hx1: Dummy { @display("p=60,240;is=s"); }
        hx2: Dummy { @display("p=140,300;is=s"); }
        hh1: Dummy { @display("p=260,240;is=s"); }
        hh2: Dummy { @display("p=340,300;is=s"); }
        hv1: Dummy { @display("p=460,240;is=s"); }
        hv2: Dummy { @display("p=540,300;is=s"); }

        // src=v, dest=-/h/v
        vx1: Dummy { @display("p=60,410;is=s"); }
        vx2: Dummy { @display("p=140,470;is=s"); }
        vh1: Dummy { @display("p=260,410;is=s"); }
        vh2: Dummy { @display("p=340,470;is=s"); }
        vv1: Dummy { @display("p=460,410;is=s"); }
        vv2: Dummy { @display("p=540,470;is=s"); }

    connections allowunconnected:
        // src=-, dest=-/h/v
        xx1.out++ --> { @display("t=m=(auto)"); } --> xx2.in++;
        xx1.out++ --> { @display("t=m=(auto)"); } --> xx2.in++;
        xh1.out++ --> { @display("m=,h;t=m=/h"); } --> xh2.in++;
        xh1.out++ --> { @display("m=,h;t=m=/h"); } --> xh2.in++;
        xv1.out++ --> { @display("m=,v;t=m=/v"); } --> xv2.in++;
        xv1.out++ --> { @display("m=,v;t=m=/v"); } --> xv2.in++;

        // src=h, dest=-/h/v
        hx1.out++ --> { @display("m=h;t=m=h"); } --> hx2.in++;
        hx1.out++ --> { @display("m=h;t=m=h"); } --> hx2.in++;
        hh1.out++ --> { @display("m=h,h;t=m=h/h"); } --> hh2.in++;
        hh1.out++ --> { @display("m=h,h;t=m=h/h"); } --> hh2.in++;
        hv1.out++ --> { @display("m=h,v;t=m=h/v"); } --> hv2.in++;
        hv1.out++ --> { @display("m=h,v;t=m=h/v"); } --> hv2.in++;

        // src=v, dest=-/h/v
        vx1.out++ --> { @display("m=v;t=m=v"); } --> vx2.in++;
        vx1.out++ --> { @display("m=v;t=m=v"); } --> vx2.in++;
        vh1.out++ --> { @display("m=v,h;t=m=v/h"); } --> vh2.in++;
        vh1.out++ --> { @display("m=v,h;t=m=v/h"); } --> vh2.in++;
        vv1.out++ --> { @display("m=v,v;t=m=v/v"); } --> vv2.in++;
        vv1.out++ --> { @display("m=v,v;t=m=v/v"); } --> vv2.in++;
}

//
// Full cartesian product: 3 srcDir x 3 destDir (-, h, v)
// 2 bidir connections per pair (to test bundling offsets).
// Modules positioned NW-SE within each pair.
//
module ConnRoutingCartesianBidi
{
    parameters:
        @display("bgb=620,530");
        @figure[note](type=label;pos=10,20;anchor=nw;text="Move modules around (shift+drag) to test all relative positions!\nTwo arrows per pair! Turn on 'Distribute Connection Arrows' (context menu) to see them.");
    submodules:
        // src=- (unconstrained), dest=-/h/v
        xx1: Dummy { @display("p=60,70;is=s"); }
        xx2: Dummy { @display("p=140,130;is=s"); }
        xh1: Dummy { @display("p=260,70;is=s"); }
        xh2: Dummy { @display("p=340,130;is=s"); }
        xv1: Dummy { @display("p=460,70;is=s"); }
        xv2: Dummy { @display("p=540,130;is=s"); }

        // src=h, dest=-/h/v
        hx1: Dummy { @display("p=60,240;is=s"); }
        hx2: Dummy { @display("p=140,300;is=s"); }
        hh1: Dummy { @display("p=260,240;is=s"); }
        hh2: Dummy { @display("p=340,300;is=s"); }
        hv1: Dummy { @display("p=460,240;is=s"); }
        hv2: Dummy { @display("p=540,300;is=s"); }

        // src=v, dest=-/h/v
        vx1: Dummy { @display("p=60,410;is=s"); }
        vx2: Dummy { @display("p=140,470;is=s"); }
        vh1: Dummy { @display("p=260,410;is=s"); }
        vh2: Dummy { @display("p=340,470;is=s"); }
        vv1: Dummy { @display("p=460,410;is=s"); }
        vv2: Dummy { @display("p=540,470;is=s"); }

    connections allowunconnected:
        // src=-, dest=-/h/v
        xx1.twoway++ <--> { @display("t=m=(auto)"); } <--> xx2.twoway++;
        xx1.twoway++ <--> { @display("t=m=(auto)"); } <--> xx2.twoway++;
        xh1.twoway++ <--> { @display("m=,h;t=m=/h"); } <--> xh2.twoway++;
        xh1.twoway++ <--> { @display("m=,h;t=m=/h"); } <--> xh2.twoway++;
        xv1.twoway++ <--> { @display("m=,v;t=m=/v"); } <--> xv2.twoway++;
        xv1.twoway++ <--> { @display("m=,v;t=m=/v"); } <--> xv2.twoway++;

        // src=h, dest=-/h/v
        hx1.twoway++ <--> { @display("m=h;t=m=h"); } <--> hx2.twoway++;
        hx1.twoway++ <--> { @display("m=h;t=m=h"); } <--> hx2.twoway++;
        hh1.twoway++ <--> { @display("m=h,h;t=m=h/h"); } <--> hh2.twoway++;
        hh1.twoway++ <--> { @display("m=h,h;t=m=h/h"); } <--> hh2.twoway++;
        hv1.twoway++ <--> { @display("m=h,v;t=m=h/v"); } <--> hv2.twoway++;
        hv1.twoway++ <--> { @display("m=h,v;t=m=h/v"); } <--> hv2.twoway++;

        // src=v, dest=-/h/v
        vx1.twoway++ <--> { @display("m=v;t=m=v"); } <--> vx2.twoway++;
        vx1.twoway++ <--> { @display("m=v;t=m=v"); } <--> vx2.twoway++;
        vh1.twoway++ <--> { @display("m=v,h;t=m=v/h"); } <--> vh2.twoway++;
        vh1.twoway++ <--> { @display("m=v,h;t=m=v/h"); } <--> vh2.twoway++;
        vv1.twoway++ <--> { @display("m=v,v;t=m=v/v"); } <--> vv2.twoway++;
        vv1.twoway++ <--> { @display("m=v,v;t=m=v/v"); } <--> vv2.twoway++;
}

//
// Manual connection routing: m=m,sx,sy,dx,dy
// sx,sy = source anchor as percentage of bounding rect (0..100)
// dx,dy = destination anchor as percentage of bounding rect (0..100)
// Tests with a large icon and a rectangle, plus connections to parent.
//
module ConnRoutingManual
{
    parameters:
        @display("bgb=700,600");
        @figure[note](type=label;pos=10,20;anchor=nw;text="Manual routing: m=m,sx,sy,dx,dy\nPercentages specify anchor points on src/dest bounding box.");
    gates:
        input in[1] @loose;
        output out[1] @loose;
        inout twoway[1] @loose;
    submodules:
        rect: Dummy { @display("p=200,300;b=200,200,rect"); }
        node1:  Dummy { @display("p=540,200"); }
        node2:  Dummy { @display("p=540,400"); }

    connections allowunconnected:
        // submodule to submodule
        node1.out++ --> { @display("m=m,,,90,30;t=m -/90\\,30/-"); } --> rect.in++;
        rect.out++ --> { @display("m=m,80,60;t=m 80\\,60/-"); } --> node1.in++;
        node2.twoway++ <--> { @display("m=m,70,80;t=m 70\\,80/-\\,-"); } <--> rect.twoway++;
        //node2.twoway++ <--> { @display("m=m,,,70,80;t=m -/70\\,80/-\\,- ignored"); } <--> rect.twoway++;

        // TODO submodule to parent test -- these currently result in weird behavior
}
