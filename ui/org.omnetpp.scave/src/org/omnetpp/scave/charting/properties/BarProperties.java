package org.omnetpp.scave.charting.properties;

import org.omnetpp.common.properties.ColorPropertyDescriptor;
import org.omnetpp.common.properties.PropertySource;

public class BarProperties extends PropertySource {

	public static final String
		PROP_BAR_COLOR			= "Bar.Color";

	private final ChartProperties chartProps;
	private String barId;

	public BarProperties(ChartProperties chartProps, String barId) {
		this.chartProps = chartProps;
		this.barId = barId;
	}

	private String propertyName(String baseName) {
		return barId == null ? baseName : baseName + "/" + barId;
	}

	@org.omnetpp.common.properties.Property(category="Bars",id=PROP_BAR_COLOR,descriptorClass=ColorPropertyDescriptor.class,optional=true)
	public String getColor() { return chartProps.getStringProperty(propertyName(PROP_BAR_COLOR)); } // FIXME use RGB
	public void setColor(String color) { chartProps.setProperty(propertyName(PROP_BAR_COLOR), color); }
	public String defaultColor() { return null; }
}