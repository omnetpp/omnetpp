network CanvasTest
{
    submodules:
        lines: Lines     { @display("p=100,50"); }
        shapes: Shapes   { @display("p=200,50"); }
        anchor: Anchor   { @display("p=300,50"); }
        font: Font       { @display("p=400,50"); }
        group: Group     { @display("p=500,50"); }
        animate: Animate { @display("p=600,50"); }
        zOrder: ZOrder   { @display("p=700,50"); }
        tags: Tags       { @display("p=800,50"); }
        submods: Submodules { @display("p=900,50"); }
}

