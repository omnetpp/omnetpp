package org.omnetpp.scave.charting.dataset;

public interface IHistogramDataset extends IDataset {

	/**
	 * Number of histograms in this dataset.
	 */
	int getSeriesCount();
	
	/**
	 * Name of the histogram at {@series} index used to
	 * identify the histogram on the UI.
	 * 
	 * @param series index of the histogram
	 * @return name of the histogram
	 */
	String getSeriesKey(int series);
	
	/**
	 * Returns true if integers are collected by the histogram.
	 * 
	 * @param series index of the histogram
	 * @return true if integers are collected
	 */
	boolean isDiscrete(int series);
	
	/**
	 * Returns the number of collected values.
	 * 
	 * @param series index of the histogram
	 * @return number of data values collected
	 */
	int getValueCount(int series);

	/**
	 * Returns the least collected value.
	 * 
	 * @param series index of the histogram
	 * @return least collected value
	 */
	double getMinValue(int series);
	
	/**
	 * Returns the greatest collected value.
	 * 
	 * @param series index of the histogram
	 * @return greatest collected value
	 */
	double getMaxValue(int series);
	
	/**
	 * Returns the number of cells.
	 * 
	 * @param series index of the histogram
	 * @return number of cells including underflow/overflow cells
	 */
	int getCellsCount(int series);
	
	/**
	 * Returns the lower bound of the cell, Double.NEGATIVE_INFINITY
	 * for the underflow cell.
	 * 
	 * @param series index of the histogram
	 * @param index index of the cell
	 * @return lower bound of the cell
	 */
	double getCellLowerBound(int series, int index);
	
	/**
	 * Returns the upper bound of the cell, Double.POSITIVE_INFINITY
	 * for the overflow cell.
	 * 
	 * @param series index of the histogram
	 * @param index index of the cell
	 * @return upper bound of the cell
	 */
	double getCellUpperBound(int series, int index);
	
	/**
	 * Returns the value of the cell.
	 * 
	 * @param series index of the histogram
	 * @param index index of the cell
	 * @return value of the cell
	 */
	double getCellValue(int series, int index);
	
	/**
	 * IHistogramDataset null object.
	 */
	final IHistogramDataset EMPTY = new IHistogramDataset() {
		public String getTitle(String format) {
			return null;
		}

		public int getSeriesCount() {
			return 0;
		}

		public String getSeriesKey(int series) {
			throw new IndexOutOfBoundsException();
		}
		
		public boolean isDiscrete(int series) {
			throw new IndexOutOfBoundsException();
		}
		
		public int getValueCount(int series) {
			throw new IndexOutOfBoundsException();
		}

		public double getMaxValue(int series) {
			throw new IndexOutOfBoundsException();
		}

		public double getMinValue(int series) {
			throw new IndexOutOfBoundsException();
		}

		public int getCellsCount(int series) {
			throw new IndexOutOfBoundsException();
		}

		public double getCellLowerBound(int series, int index) {
			throw new IndexOutOfBoundsException();
		}

		public double getCellUpperBound(int series, int index) {
			throw new IndexOutOfBoundsException();
		}

		public double getCellValue(int series, int index) {
			throw new IndexOutOfBoundsException();
		}
	};
}
