/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.omnetpp.scave.charting.properties.LineProperties.SymbolType;

public class ChartSymbolFactory {

    public static IChartSymbol createChartSymbol(SymbolType type, int size) {
        if (type == null)
            return null;
        switch (type) {
        case None: return null;
        case Cross: return new CrossSymbol(size);
        case Diamond: return new DiamondSymbol(size);
        case Dot: return new OvalSymbol(size);
        case Plus: return new PlusSymbol(size);
        case Square: return new SquareSymbol(size);
        case Triangle: return new TriangleSymbol(size);
        default: throw new IllegalArgumentException("unknown symbol type: " + type);
        }
    }
}
