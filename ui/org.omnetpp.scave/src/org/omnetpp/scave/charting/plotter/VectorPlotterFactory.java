/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.omnetpp.scave.charting.properties.LineVisualProperties.LineType;

public class VectorPlotterFactory {

    public static IVectorPlotter createVectorPlotter(LineType lineType) {
        switch (lineType) {
        case Dots: return new DotsVectorPlotter();
        case Linear: return new LinesVectorPlotter();
        case StepsPost: return new StepsVectorPlotter(false);
        case StepsPre: return new StepsVectorPlotter(true);
        case Pins: return new PinsVectorPlotter();
        case Points: return new PointsVectorPlotter();
        default: throw new IllegalArgumentException("unknown line style: " + lineType);
        }
    }
}
