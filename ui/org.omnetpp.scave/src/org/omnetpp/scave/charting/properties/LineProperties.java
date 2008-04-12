package org.omnetpp.scave.charting.properties;

import org.omnetpp.common.properties.ColorPropertyDescriptor;
import org.omnetpp.common.properties.PropertySource;
import org.omnetpp.scave.charting.properties.ChartProperties.LineType;
import org.omnetpp.scave.charting.properties.ChartProperties.SymbolType;

public class LineProperties extends PropertySource {
	private final ChartProperties chartProps;
	private String lineId;

	public LineProperties(ChartProperties chartProps, String lineId) {
		this.chartProps = chartProps;
		this.lineId = lineId;
	}

	private String propertyName(String baseName) {
		return lineId == null ? baseName : baseName + "/" + lineId;
	}

	@org.omnetpp.common.properties.Property(category="Lines",id=ChartProperties.PROP_DISPLAY_LINE,optional=true)
	public boolean getDisplayLine() { return chartProps.getBooleanProperty(propertyName(ChartProperties.PROP_DISPLAY_LINE)); }
	public void setDisplayLine(boolean display) { chartProps.setProperty(propertyName(ChartProperties.PROP_DISPLAY_LINE), display); }
	public boolean defaultDisplayLine() { return ChartDefaults.DEFAULT_DISPLAY_LINE; }

	@org.omnetpp.common.properties.Property(category="Lines",id=ChartProperties.PROP_SYMBOL_TYPE,optional=true)
	public SymbolType getSymbolType() { return chartProps.getEnumProperty(propertyName(ChartProperties.PROP_SYMBOL_TYPE), SymbolType.class); }
	public void setSymbolType(SymbolType type) { chartProps.setProperty(propertyName(ChartProperties.PROP_SYMBOL_TYPE), type); }
	public SymbolType defaultSymbolType() { return null; }

	@org.omnetpp.common.properties.Property(category="Lines",id=ChartProperties.PROP_SYMBOL_SIZE)
	public Integer getSymbolSize() { return chartProps.getIntegerProperty(propertyName(ChartProperties.PROP_SYMBOL_SIZE)); }
	public void setSymbolSize(Integer size) { chartProps.setProperty(propertyName(ChartProperties.PROP_SYMBOL_SIZE), size); }
	public Integer defaultSymbolSize() { return ChartDefaults.DEFAULT_SYMBOL_SIZE; }

	@org.omnetpp.common.properties.Property(category="Lines",id=ChartProperties.PROP_LINE_TYPE,optional=true)
	public LineType getLineType() { return chartProps.getEnumProperty(propertyName(ChartProperties.PROP_LINE_TYPE), LineType.class); }
	public void setLineType(LineType style) { chartProps.setProperty(propertyName(ChartProperties.PROP_LINE_TYPE), style); }
	public LineType defaultLineType() { return null; }

	@org.omnetpp.common.properties.Property(category="Lines",id=ChartProperties.PROP_LINE_COLOR,descriptorClass=ColorPropertyDescriptor.class,optional=true)
	public String getLineColor() { return chartProps.getStringProperty(propertyName(ChartProperties.PROP_LINE_COLOR)); } // FIXME use RGB
	public void setLineColor(String color) { chartProps.setProperty(propertyName(ChartProperties.PROP_LINE_COLOR), color); }
	public String defaultLineColor() { return null; }
}