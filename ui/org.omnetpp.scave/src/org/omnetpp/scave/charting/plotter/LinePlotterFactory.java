/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.omnetpp.scave.charting.properties.LineVisualProperties.DrawStyle;

public class LinePlotterFactory {

    public static ILinePlotter createVectorPlotter(DrawStyle drawStyle) {
        switch (drawStyle) {
        case None: return new NoLinePlotter();
        case Linear: return new LinearLinePlotter();
        case StepsPost: return new StepsLinePlotter(false);
        case StepsPre: return new StepsLinePlotter(true);
        case Pins: return new PinsLinePlotter();
        default: throw new IllegalArgumentException("unknown line style: " + drawStyle);
        }
    }
}
