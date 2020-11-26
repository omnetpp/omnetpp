network CanvasTest
{
    submodules:

        // Figure property tests:

        lineStyle: LineStyle {
            @display("p=100,50");
        }
        lineColor: LineColor {
            @display("p=200,50");
        }
        lineWidth: LineWidth {
            @display("p=300,50");
        }
        lineOpacity: LineOpacity {
            @display("p=400,50");
        }
        capStyle: LineCap {
            @display("p=500,50");
        }
        joinStyle: LineJoin {
            @display("p=600,50");
        }
        arrowhead: Arrowhead {
            @display("p=700,50");
        }
        fillColor: FillColor {
            @display("p=800,50");
        }
        fillOpacity: FillOpacity {
            @display("p=900,50");
        }
        filledAndOutlined: FilledAndOutlined {
            @display("p=1000,50");
        }

        textAnchor: TextAnchor {
            @display("p=100,150");
        }
        textColor: TextColor {
            @display("p=200,150");
        }
        textOpacity: TextOpacity {
            @display("p=300,150");
        }
        textFont: TextFont {
            @display("p=400,150");
        }
        textHalo: TextHalo {
            @display("p=500,150");
        }
        textUtf8: TextUtf8 {
            @display("p=600,150");
        }
        textAlignment: TextAlignment {
            @display("p=700,150");
        }

        imageAnchor: ImageAnchor {
            @display("p=100,250");
        }
        imageOpacity: ImageOpacity {
            @display("p=200,250");
        }
        imageInterpolation: ImageInterpolation {
            @display("p=300,250");
        }
        imageColorization: ImageColorization {
            @display("p=400,250");
        }

        tooltip: Tooltip {
            @display("p=500,250");
        }
        associatedObject: AssociatedObject {
            @display("p=600,250");
        }


        // Figure type tests:


        line: Line {
            @display("p=100,400");
        }
        polyline: Polyline {
            @display("p=200,400");
        }

        arc: Arc {
            @display("p=300,400");
        }
        pieslice: Pieslice {
            @display("p=400,400");
        }

        ring: Ring {
            @display("p=500,400");
        }
        oval: Oval {
            @display("p=600,400");
        }
        rectangle: Rectangle {
            @display("p=700,400");
        }

        polygon: Polygon {
            @display("p=800,400");
        }

        text: Text {
            @display("p=900,400");
        }
        label: Label {
            @display("p=1000,400");
        }

        image: Image {
            @display("p=100,500");
        }
        icon: Icon {
            @display("p=200,500");
        }
        pixmap: Pixmap {
            @display("p=300,500");
        }

        path: Path {
            @display("p=100,650");
        }

        //TODO group
        //TODO panel (not in the canvas API yet)

        shapeAnchor: ShapeAnchor {
            @display("p=100,750");
        }
        panel: Panel {
            @display("p=400,750");
        }
        group: Group {
            @display("p=500,750");
        }
        animate: Animate {
            @display("p=600,750");
        }
        zOrder: ZOrder {
            @display("p=700,750");
        }
        tags: Tags {
            @display("p=800,750");
        }
        submods: Submodules {
            @display("p=900,750");
        }
        transform: Transform {
            @display("p=1000,750");
        }
}

