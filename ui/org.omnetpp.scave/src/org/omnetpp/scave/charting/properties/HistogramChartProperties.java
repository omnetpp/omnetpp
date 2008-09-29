package org.omnetpp.scave.charting.properties;

import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;
import org.omnetpp.common.properties.BasePropertySource;
import org.omnetpp.common.properties.Property;
import org.omnetpp.scave.charting.dataset.IHistogramDataset;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.HistogramChart;
import org.omnetpp.scave.model2.DatasetManager;

public class HistogramChartProperties extends ChartProperties
{
	private static final String DEFAULT_HIST_PROPERTIES_ID = "default";
	
	public static final String
		PROP_HIST_BAR			= "Hist.Bar",
		PROP_HIST_DATA			= "Hist.Data",
		PROP_SHOW_OVERFLOW_CELL	= "Hist.ShowOverflowCell",
		PROP_BAR_BASELINE		= "Bars.Baseline";
	
	public enum HistogramBar {
		Solid,
		Outline,
	}
	
	public enum HistogramDataType {
		Count("count"),
		Pdf("probability density"),
		Cdf("cumulative density");
		
		private String displayName;
		
		private HistogramDataType(String displayName) {
			this.displayName = displayName;
		}
		
		@Override public String toString() {
			return displayName;
		}
	}
	
	public HistogramChartProperties(Chart chart, List<org.omnetpp.scave.model.Property> properties, ResultFileManager manager) {
		super(chart, properties, manager);
	}

	@Property(category="Plot",id=PROP_HIST_BAR,description="Histogram drawing method.")
	public HistogramBar getBarType() { return getEnumProperty(PROP_HIST_BAR, HistogramBar.class); }
	public void setBarType(HistogramBar placement) { setProperty(PROP_HIST_BAR, placement); }
	public HistogramBar defaultBarType() { return ChartDefaults.DEFAULT_HIST_BAR; }
	
	@Property(category="Plot",id=PROP_BAR_BASELINE,description="Baseline of the bars.")
	public Double getBarBaseline() { return getDoubleProperty(PROP_BAR_BASELINE); }
	public void setBarBaseline(Double baseline) { setProperty(PROP_BAR_BASELINE, baseline); }
	public Double defaultBarBaseline() { return ChartDefaults.DEFAULT_BAR_BASELINE; }
	
	@Property(category="Plot",id=PROP_HIST_DATA,description="Histogram data. Counts, probability density and cumulative density can be displayed.")
	public HistogramDataType getHistogramDataType() { return getEnumProperty(PROP_HIST_DATA, HistogramDataType.class); }
	public void setHistogramDataType(HistogramDataType data) { setProperty(PROP_HIST_DATA, data); }
	public HistogramDataType defaultHistogramData() { return ChartDefaults.DEFAULT_HIST_DATA; }

	@Property(category="Plot",id=PROP_SHOW_OVERFLOW_CELL,description="Show over/underflow cells.")
	public boolean getShowOverflowCell() { return getBooleanProperty(PROP_SHOW_OVERFLOW_CELL); }
	public void setShowOverflowCell(boolean value) { setProperty(PROP_SHOW_OVERFLOW_CELL, value); }
	public boolean defaultShowOverflowCell() { return ChartDefaults.DEFAULT_SHOW_OVERFLOW_CELL; }

	/*======================================================================
	 *                             Histograms
	 *======================================================================*/
	@Property(category="Plot",id="Histograms",displayName="Histograms",
			description="Histogram plot properties.")
	public IPropertySource getHistogramProperties()
	{
		IHistogramDataset dataset = DatasetManager.createHistogramDataset((HistogramChart)chart, manager, null);
        String[] keys = null;
		if (dataset != null) {
			keys = new String[dataset.getSeriesCount()];
			for (int i= 0; i < keys.length; ++i)
				keys[i] = dataset.getSeriesKey(i);
		}
		IPropertyDescriptor[] descriptors = createDescriptors(DEFAULT_HIST_PROPERTIES_ID, keys, keys);
		return new BasePropertySource(descriptors) {
			@Override public Object getPropertyValue(Object id) {
				return new HistogramProperties(HistogramChartProperties.this,
						id == DEFAULT_HIST_PROPERTIES_ID ? null : (String)id);
			}
		};
	}
}