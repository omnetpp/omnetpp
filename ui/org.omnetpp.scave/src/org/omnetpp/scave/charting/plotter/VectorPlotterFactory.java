package org.omnetpp.scave.charting.plotter;

import org.omnetpp.scave.charting.ChartProperties.LineType;

public class VectorPlotterFactory {

	public static IVectorPlotter createVectorPlotter(LineType lineType) {
		switch (lineType) {
		case Dots: return new DotsVectorPlotter();
		case Linear: return new LinesVectorPlotter();
		case SampleHold: return new SampleHoldVectorPlotter(false);
		case BackwardSampleHold: return new SampleHoldVectorPlotter(true);
		case Pins: return new PinsVectorPlotter();
		case Points: return new PointsVectorPlotter();
		default: throw new IllegalArgumentException("unknown line style: " + lineType);
		}
	}
}
