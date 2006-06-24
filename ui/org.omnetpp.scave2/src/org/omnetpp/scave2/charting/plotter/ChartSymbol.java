package org.omnetpp.scave2.charting.plotter;

import org.eclipse.draw2d.Graphics;

/**
 * Interface for chart symbol drawing classes
 * 
 * @author andras
 */
public abstract class ChartSymbol implements IChartSymbol {
	protected int size = 5;

	public ChartSymbol() {
	}

	public ChartSymbol(int size) {
		setSize(size);
	}

	public int getSize() {
		return size;
	}

	public void setSize(int size) {
		this.size = size;
	}
	
}
