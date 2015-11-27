network CanvasTest
{
    submodules:
        line: Line           { @display("p=100,50"); }
        arc: Arc             { @display("p=200,50"); }
        polyline: Polyline   { @display("p=300,50"); }
        rectangle: Rectangle { @display("p=400,50"); }
        oval: Oval           { @display("p=500,50"); }
        ring: Ring           { @display("p=600,50"); }
        pieslice: Pieslice   { @display("p=700,50"); }
        polygon : Polygon    { @display("p=800,50"); }
        text: Text           { @display("p=900,50"); }
        label: Label         { @display("p=1000,50"); }
        image: Image         { @display("p=1100,50"); }
        path: Path           { @display("p=1200,50"); }

        anchor: Anchor       { @display("p=100,150"); }
        font: Font           { @display("p=200,150"); }
        group: Group         { @display("p=300,150"); }
        animate: Animate     { @display("p=400,150"); }
        zOrder: ZOrder       { @display("p=500,150"); }
        tags: Tags           { @display("p=600,150"); }
        submods: Submodules  { @display("p=700,150"); }
        transform: Transform { @display("p=800,150"); }
        pixmap: Pixmap       { @display("p=900,150"); }
        icon: Icon           { @display("p=1000,150"); }

        polygon2: Polygon2   { @display("p=100,250"); }
}

