network CanvasTest
{
    submodules:
        lineStyle: LineStyle                   { @display("p=100,50"); }
        lineColor: LineColor                   { @display("p=200,50"); }
        lineWidth: LineWidth                   { @display("p=300,50"); }
        lineOpacity: LineOpacity               { @display("p=400,50"); }
        capStyle: LineCap                      { @display("p=500,50"); }
        joinStyle : LineJoin                   { @display("p=600,50"); }
        arrowhead : Arrowhead                  { @display("p=700,50"); }
        fillColor : FillColor                  { @display("p=800,50"); }
        fillOpacity : FillOpacity              { @display("p=900,50"); }
        filledAndOutlined : FilledAndOutlined  { @display("p=1000,50"); }

        textAnchor: TextAnchor                 { @display("p=100,150"); }
        textColor: TextColor                   { @display("p=200,150"); }
        textOpacity: TextOpacity               { @display("p=300,150"); }
        textFont: TextFont                     { @display("p=400,150"); }

        imageAnchor: ImageAnchor               { @display("p=100,250"); }
        imageOpacity: ImageOpacity             { @display("p=200,250"); }
        imageInterpolation: ImageInterpolation { @display("p=300,250"); }
        imageColorization: ImageColorization   { @display("p=400,250"); }

        tooltip: Tooltip                       { @display("p=500,250"); }
        associatedObject: AssociatedObject     { @display("p=600,250"); }

        line: Line           { @display("p=100,450"); }
        polyline: Polyline   { @display("p=200,450"); }

        arc: Arc             { @display("p=300,450"); }
        pieslice: Pieslice   { @display("p=400,450"); }

        ring: Ring           { @display("p=500,450"); }
        oval: Oval           { @display("p=600,450"); }
        rectangle: Rectangle { @display("p=700,450"); }

        polygon: Polygon     { @display("p=800,450"); }

        text: Text           { @display("p=100,650"); }
        label: Label         { @display("p=200,650"); }
        image: Image         { @display("p=300,650"); }
        path: Path           { @display("p=400,650"); }
        pixmap: Pixmap       { @display("p=500,650"); }
        icon: Icon           { @display("p=600,650"); }

        shapeAnchor: ShapeAnchor { @display("p=100,750"); }
        group: Group         { @display("p=500,750"); }
        animate: Animate     { @display("p=600,750"); }
        zOrder: ZOrder       { @display("p=700,750"); }
        tags: Tags           { @display("p=800,750"); }
        submods: Submodules  { @display("p=900,750"); }
        transform: Transform { @display("p=1000,750"); }
}

