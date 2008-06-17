package org.omnetpp.scave.charting.properties;

import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;
import org.omnetpp.common.properties.BasePropertySource;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model2.DatasetManager;

public class VectorChartProperties extends ChartProperties
{
	private static final String DEFAULT_LINE_PROPERTIES_ID = "default";
	
	public static final String
		PROP_X_AXIS_MIN			= "X.Axis.Min",
		PROP_X_AXIS_MAX			= "X.Axis.Max";
	
	
	public VectorChartProperties(Chart chart, List<Property> properties, ResultFileManager manager) {
		super(chart, properties, manager);
	}
	/*======================================================================
	 *                             Axes
	 *======================================================================*/
	@org.omnetpp.common.properties.Property(category="Axes",id=PROP_X_AXIS_MIN,
			description="Crops the input below this x value.")
	public Double getXAxisMin() { return getDoubleProperty(PROP_X_AXIS_MIN); }
	public void setXAxisMin(Double min) { setProperty(PROP_X_AXIS_MIN, min); }

	@org.omnetpp.common.properties.Property(category="Axes",id=PROP_X_AXIS_MAX,
			description="Crops the input above this x value.")
	public Double getXAxisMax() { return getDoubleProperty(PROP_X_AXIS_MAX); }
	public void setXAxisMax(Double max) { setProperty(PROP_X_AXIS_MAX, max); }

	/*======================================================================
	 *                             Lines
	 *======================================================================*/
	@org.omnetpp.common.properties.Property(category="Plot",id="Lines",displayName="Lines",
			description="Properties of individual lines. Default is applied to all properties not set individually.")
	public IPropertySource getLineProperties()
	{
		IXYDataset dataset = DatasetManager.createXYDataset(chart, null, false, manager, null);
        String[] keys = null;
		if (dataset != null) {
			keys = new String[dataset.getSeriesCount()];
			for (int i= 0; i < keys.length; ++i)
				keys[i] = dataset.getSeriesKey(i);
		}
		IPropertyDescriptor[] descriptors = createDescriptors(DEFAULT_LINE_PROPERTIES_ID, keys, keys);
		return new BasePropertySource(descriptors) {
			@Override public Object getPropertyValue(Object id) {
				return new LineProperties(VectorChartProperties.this,
						id == DEFAULT_LINE_PROPERTIES_ID ? null : (String)id);
			}
		};
	}

	public LineProperties getLineProperties(String lineId) { return new LineProperties(this, lineId); }
}