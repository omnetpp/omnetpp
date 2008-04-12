package org.omnetpp.scave.charting.dataset;

public interface IHistogramDataset extends IDataset {

	int getSeriesCount();
	
	String getSeriesKey(int series);
	
	boolean isDiscrete(int series);

	int getCellsCount(int series);
	
	double getCellMin(int series, int index);
	
	double getCellMax(int series, int index);
	
	double getCellValue(int series, int index);
	
	final IHistogramDataset EMPTY = new IHistogramDataset() {
		public int getSeriesCount() {
			return 0;
		}

		public String getSeriesKey(int series) {
			throw new IndexOutOfBoundsException();
		}
		
		public boolean isDiscrete(int series) {
			throw new IndexOutOfBoundsException();
		}
		
		public int getCellsCount(int series) {
			throw new IndexOutOfBoundsException();
		}

		public double getCellMin(int series, int index) {
			throw new IndexOutOfBoundsException();
		}

		public double getCellMax(int series, int index) {
			throw new IndexOutOfBoundsException();
		}

		public double getCellValue(int series, int index) {
			throw new IndexOutOfBoundsException();
		}
	};
}
