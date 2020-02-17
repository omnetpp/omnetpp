/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.omnetpp.scave.charting.properties.PlotProperties.SymbolType;

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
        case Dot: return new OvalSymbol(size);
        case Plus: return new PlusSymbol(size);
        case VLine: return new LineSymbol(size, false);
        case HLine: return new LineSymbol(size, true);
        case Octagon: return new OctagonSymbol(size);
        case Square: return new SquareSymbol(size);
        case Pentagon: return new PentagonSymbol(size);
        case Star: return new StarSymbol(size);
        case Triangle_Up: return new TriangleSymbol(size, 0);
        case Triangle_Down: return new TriangleSymbol(size, 2);
        case Triangle_Left: return new TriangleSymbol(size, 1);
        case Triangle_Right: return new TriangleSymbol(size, 3);
        case Tri_Up: return new TriSymbol(size, 0);
        case Tri_Down: return new TriSymbol(size, 2);
        case Tri_Left: return new TriSymbol(size, 1);
        case Tri_Right: return new TriSymbol(size, 3);
        default: throw new IllegalArgumentException("unknown symbol type: " + type);
        }
    }
}
