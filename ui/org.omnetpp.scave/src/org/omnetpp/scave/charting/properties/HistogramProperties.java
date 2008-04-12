package org.omnetpp.scave.charting.properties;

import org.omnetpp.common.properties.ColorPropertyDescriptor;
import org.omnetpp.common.properties.PropertySource;

public class HistogramProperties extends PropertySource {
	private final ChartProperties chartProps;
	private String histogramId;

	public HistogramProperties(ChartProperties chartProps, String histogramId) {
		this.chartProps = chartProps;
		this.histogramId = histogramId;
	}

	private String propertyName(String baseName) {
		return chartProps.propertyName(baseName, histogramId);
	}

	@org.omnetpp.common.properties.Property(category="Histograms",id=ChartProperties.PROP_HIST_COLOR,descriptorClass=ColorPropertyDescriptor.class,optional=true)
	public String getColor() { return chartProps.getStringProperty(propertyName(ChartProperties.PROP_HIST_COLOR)); } // FIXME use RGB
	public void setColor(String color) { chartProps.setProperty(propertyName(ChartProperties.PROP_HIST_COLOR), color); }
	public String defaultColor() { return null; }
}