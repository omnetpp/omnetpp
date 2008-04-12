package org.omnetpp.scave.charting;

import org.eclipse.swt.graphics.Color;
import org.omnetpp.scave.charting.plotter.IChartSymbol;

public interface ILegend {
	
	void clearItems();
	
	void addItem(Color color, String text, IChartSymbol symbol, boolean drawLine);
}
