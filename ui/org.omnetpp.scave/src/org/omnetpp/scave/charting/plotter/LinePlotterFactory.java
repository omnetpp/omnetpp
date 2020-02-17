/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.omnetpp.scave.charting.plotter.StepsLinePlotter.Mode;
import org.omnetpp.scave.charting.properties.PlotProperties.DrawStyle;

public class LinePlotterFactory {

    public static ILinePlotter createVectorPlotter(DrawStyle drawStyle) {
        switch (drawStyle) {
        case None: return new NoLinePlotter();
        case Linear: return new LinearLinePlotter();
        case StepsPost: return new StepsLinePlotter(Mode.POST);
        case StepsPre: return new StepsLinePlotter(Mode.PRE);
        case StepsMid: return new StepsLinePlotter(Mode.MID);
        case Pins: return new PinsLinePlotter();
        default: throw new IllegalArgumentException("unknown line style: " + drawStyle);
        }
    }
}
