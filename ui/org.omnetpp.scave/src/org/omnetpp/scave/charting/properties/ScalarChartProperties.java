package org.omnetpp.scave.charting.properties;

import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;
import org.omnetpp.common.properties.BasePropertySource;
import org.omnetpp.scave.charting.dataset.ScalarDataset;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model2.DatasetManager;

public class ScalarChartProperties extends ChartProperties
{
	private static final String DEFAULT_BAR_PROPERTIES_ID = "default";
	
	public ScalarChartProperties(Chart chart, List<Property> properties, ResultFileManager manager) {
		super(chart, properties, manager);
	}

	/*======================================================================
	 *                             Titles
	 *======================================================================*/
	@org.omnetpp.common.properties.Property(category="Titles",id=ChartProperties.PROP_WRAP_LABELS)
	public boolean getWrapLabels() { return getBooleanProperty(ChartProperties.PROP_WRAP_LABELS); }
	public void setWrapLabels(boolean wrap) { setProperty(ChartProperties.PROP_WRAP_LABELS, wrap); }
	public boolean defaultWrapLabels() { return ChartDefaults.DEFAULT_WRAP_LABELS; }

	/*======================================================================
	 *                             Bars
	 *======================================================================*/
	@org.omnetpp.common.properties.Property(category="Plot",id=ChartProperties.PROP_BAR_BASELINE)
	public Double getBarBaseline() { return getDoubleProperty(ChartProperties.PROP_BAR_BASELINE); }
	public void setBarBaseline(Double baseline) { setProperty(ChartProperties.PROP_BAR_BASELINE, baseline); }
	public Double defaultBarBaseline() { return ChartDefaults.DEFAULT_BAR_BASELINE; }

	@org.omnetpp.common.properties.Property(category="Plot",id=ChartProperties.PROP_BAR_PLACEMENT)
	public BarPlacement getBarPlacement() { return getEnumProperty(ChartProperties.PROP_BAR_PLACEMENT, BarPlacement.class); }
	public void setBarPlacement(BarPlacement placement) { setProperty(ChartProperties.PROP_BAR_PLACEMENT, placement); }
	public BarPlacement defaultBarPlacement() { return ChartDefaults.DEFAULT_BAR_PLACEMENT; }
	
	@org.omnetpp.common.properties.Property(category="Plot",id="Bars",displayName="Bars")
	public IPropertySource getBarProperties() {
		ScalarDataset dataset = DatasetManager.createScalarDataset((BarChart)chart, manager, null);
		String[] names = new String[dataset.getColumnCount()];
		for (int i = 0; i < names.length; ++i)
			names[i] = dataset.getColumnKey(i);

		IPropertyDescriptor[] descriptors = createDescriptors(DEFAULT_BAR_PROPERTIES_ID, names, names);
		return new BasePropertySource(descriptors) {
			@Override
			public Object getPropertyValue(Object id) {
				return new BarProperties(ScalarChartProperties.this,
						id == DEFAULT_BAR_PROPERTIES_ID ? null : (String)id);
			}
		};
	}
}