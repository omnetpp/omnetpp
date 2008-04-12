package org.omnetpp.scave.charting.properties;

import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;
import org.omnetpp.common.properties.BasePropertySource;
import org.omnetpp.scave.charting.dataset.IHistogramDataset;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.HistogramChart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model2.DatasetManager;

public class HistogramChartProperties extends ChartProperties
{
	private static final String DEFAULT_HIST_PROPERTIES_ID = "default";
	
	public HistogramChartProperties(Chart chart, List<Property> properties, ResultFileManager manager) {
		super(chart, properties, manager);
	}

	@org.omnetpp.common.properties.Property(category="Plot",id=ChartProperties.PROP_HIST_BAR)
	public HistogramBar getBarType() { return getEnumProperty(ChartProperties.PROP_HIST_BAR, HistogramBar.class); }
	public void setBarType(HistogramBar placement) { setProperty(ChartProperties.PROP_HIST_BAR, placement); }
	public HistogramBar defaultBarType() { return ChartDefaults.DEFAULT_HIST_BAR; }
	
	/*======================================================================
	 *                             Histograms
	 *======================================================================*/
	@org.omnetpp.common.properties.Property(category="Plot",id="Histograms",displayName="Histograms")
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