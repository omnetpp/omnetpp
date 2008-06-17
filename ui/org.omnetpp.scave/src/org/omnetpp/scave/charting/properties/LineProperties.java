package org.omnetpp.scave.charting.properties;

import org.omnetpp.common.properties.ColorPropertyDescriptor;
import org.omnetpp.common.properties.PropertySource;

public class LineProperties extends PropertySource {
	
	public static final String
		PROP_DISPLAY_LINE		= "Line.Display",
		PROP_SYMBOL_TYPE		= "Symbols.Type",
		PROP_SYMBOL_SIZE		= "Symbols.Size",
		PROP_LINE_TYPE			= "Line.Type",
		PROP_LINE_COLOR			= "Line.Color";
	
	
	public enum SymbolType {
		None("None", "none"), //XXX allowed?
		Cross("Cross", "cross"),
		Diamond("Diamond", "diamond"),
		Dot("Dot", "dot"),
		Plus("Plus", "plus"),
		Square("Square", "square"),
		Triangle("Triangle", "triangle");
	
		private String name;
		private String imageId;
	
		private SymbolType(String name, String img) {
			this.name = name;
			imageId = "icons/full/obj16/sym_"+img+".png";
		}
	
		@Override
		public String toString() {
			return name;
		}
	
		public String getImageId() {
			return imageId;
		}
	}
	
	public enum LineType {
		Linear("Linear", "linear"),
		Pins("Pins", "pins"), 
		Dots("Dots", "dots"),
		Points("Points", "points"), 
		SampleHold("Sample-Hold", "samplehold"),
		BackwardSampleHold("Backward Sample-Hold", "bksamplehold");
	
		private String name;
		private String imageId;
	
		private LineType(String name, String img) {
			this.name = name;
			this.imageId = "icons/full/obj16/line_"+img+".png";
		}
	
		@Override
		public String toString() {
			return name;
		}
	
		public String getImageId() {
			return imageId;
		}
	}
	private final ChartProperties chartProps;
	private String lineId;

	public LineProperties(ChartProperties chartProps, String lineId) {
		this.chartProps = chartProps;
		this.lineId = lineId;
	}

	private String propertyName(String baseName) {
		return lineId == null ? baseName : baseName + "/" + lineId;
	}

	@org.omnetpp.common.properties.Property(category="Lines",id=PROP_DISPLAY_LINE,optional=true,
			description="Displays the line.")
	public boolean getDisplayLine() { return chartProps.getBooleanProperty(propertyName(PROP_DISPLAY_LINE)); }
	public void setDisplayLine(boolean display) { chartProps.setProperty(propertyName(PROP_DISPLAY_LINE), display); }
	public boolean defaultDisplayLine() { return ChartDefaults.DEFAULT_DISPLAY_LINE; }

	@org.omnetpp.common.properties.Property(category="Lines",id=PROP_SYMBOL_TYPE,optional=true,
			description="The symbol drawn at the data points.")
	public SymbolType getSymbolType() { return chartProps.getEnumProperty(propertyName(PROP_SYMBOL_TYPE), SymbolType.class); }
	public void setSymbolType(SymbolType type) { chartProps.setProperty(propertyName(PROP_SYMBOL_TYPE), type); }
	public SymbolType defaultSymbolType() { return null; }

	@org.omnetpp.common.properties.Property(category="Lines",id=PROP_SYMBOL_SIZE,
			description="The size of the symbol drawn at the data points.")
	public Integer getSymbolSize() { return chartProps.getIntegerProperty(propertyName(PROP_SYMBOL_SIZE)); }
	public void setSymbolSize(Integer size) { chartProps.setProperty(propertyName(PROP_SYMBOL_SIZE), size); }
	public Integer defaultSymbolSize() { return ChartDefaults.DEFAULT_SYMBOL_SIZE; }

	@org.omnetpp.common.properties.Property(category="Lines",id=PROP_LINE_TYPE,optional=true,
			description="Line drawing method. One of Linear, Pins, Dots, Points, Sample-Hold or Backward Sample-Hold.")
	public LineType getLineType() { return chartProps.getEnumProperty(propertyName(PROP_LINE_TYPE), LineType.class); }
	public void setLineType(LineType style) { chartProps.setProperty(propertyName(PROP_LINE_TYPE), style); }
	public LineType defaultLineType() { return null; }

	@org.omnetpp.common.properties.Property(category="Lines",id=PROP_LINE_COLOR,descriptorClass=ColorPropertyDescriptor.class,optional=true,
			description="Color of the line. Color name or #RRGGBB. Press Ctrl+Space for a list of color names.")
	public String getLineColor() { return chartProps.getStringProperty(propertyName(PROP_LINE_COLOR)); } // FIXME use RGB
	public void setLineColor(String color) { chartProps.setProperty(propertyName(PROP_LINE_COLOR), color); }
	public String defaultLineColor() { return null; }
}