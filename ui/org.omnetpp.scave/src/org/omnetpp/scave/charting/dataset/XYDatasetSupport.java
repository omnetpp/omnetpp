package org.omnetpp.scave.charting.dataset;

abstract class XYDatasetSupport implements IXYDataset {
	
	protected boolean initialized;
	protected double minX, maxX;
	protected double minY, maxY;

	public double getMinX() {
		if (!initialized)
			calculateBounds();
		return minX;
	}

	public double getMaxX() {
		if (!initialized)
			calculateBounds();
		return maxX;
	}
	
	public double getMinY() {
		if (!initialized)
			calculateBounds();
		return minY;
	}

	public double getMaxY() {
		if (!initialized)
			calculateBounds();
		return maxY;
	}
	
	protected void calculateBounds() {
		minX = Double.POSITIVE_INFINITY;
		minY = Double.POSITIVE_INFINITY;
		maxX = Double.NEGATIVE_INFINITY;
		maxY = Double.NEGATIVE_INFINITY;

		int count = getSeriesCount();
		for (int series = 0; series < count; series++) {
			int n = getItemCount(series);
			if (n > 0) {
				// X must be increasing
				minX = Math.min(minX, getX(series, 0));
				maxX = Math.max(maxX, getX(series, n-1));
				for (int i = 0; i < n; i++) {
					double y = getY(series, i);
					if (!Double.isNaN(y)) {
						minY = Math.min(minY, y);
						maxY = Math.max(maxY, y);
					}
				}
			}
		}
		initialized = true;
	}
}
