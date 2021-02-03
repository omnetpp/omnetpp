/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.omnetpp.scave.charting.properties.PlotProperty.SymbolType;

public class ChartSymbolFactory {

    public static IPlotSymbol createChartSymbol(SymbolType type, int size) {
        if (type == null)
            return null;
        switch (type) {
        case None: return null;
        case Point: return new PointSymbol();
        case Cross: return new CrossSymbol(size);
        case Diamond: return new DiamondSymbol(size);
        case ThinDiamond: return new ThinDiamondSymbol(size);
        case Circle: return new OvalSymbol(size);
        case Dot: return new OvalSymbol(size / 2);
        case Plus: return new PlusSymbol(size);
        case VLine: return new LineSymbol.VerticalLineSymbol(size);
        case HLine: return new LineSymbol.HorizontalLineSymbol(size);
        case Octagon: return new OctagonSymbol(size);
        case Square: return new SquareSymbol(size);
        case Pentagon: return new PentagonSymbol(size);
        case Star: return new StarSymbol(size);
        case Triangle_Up: return new TriangleSymbol.UpTriangleSymbol(size);
        case Triangle_Down: return new TriangleSymbol.DownTriangleSymbol(size);
        case Triangle_Left: return new TriangleSymbol.LeftTriangleSymbol(size);
        case Triangle_Right: return new TriangleSymbol.RightTriangleSymbol(size);
        case Tri_Up: return new TriSymbol.UpTriSymbol(size);
        case Tri_Down: return new TriSymbol.DownTriSymbol(size);
        case Tri_Left: return new TriSymbol.LeftTriSymbol(size);
        case Tri_Right: return new TriSymbol.RightTriSymbol(size);
        default: throw new IllegalArgumentException("unknown symbol type: " + type);
        }
    }
}
