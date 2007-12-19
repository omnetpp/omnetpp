package org.omnetpp.scave.charting;

import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.command.RemoveCommand;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.emf.edit.domain.AdapterFactoryEditingDomain;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.common.properties.BasePropertySource;
import org.omnetpp.common.properties.ColorPropertyDescriptor;
import org.omnetpp.common.properties.PropertySource;
import org.omnetpp.common.util.Converter;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.charting.dataset.ScalarDataset;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.HistogramChart;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScatterChart;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.DatasetManager;

/**
 * Property source for charts.  
 * @author tomi
 */
//XXX should take the default values from ChartDefaults
public class ChartProperties extends PropertySource {

	/**
	 * Property names used in the model.
	 */
	public static final String
		// Titles
		PROP_GRAPH_TITLE		= "Graph.Title",
		PROP_GRAPH_TITLE_FONT	= "Graph.Title.Font",
		PROP_X_AXIS_TITLE		= "X.Axis.Title",
		PROP_Y_AXIS_TITLE		= "Y.Axis.Title",
		PROP_AXIS_TITLE_FONT	= "Axis.Title.Font",
		PROP_LABEL_FONT			= "Label.Font",
		PROP_X_LABELS_ROTATE_BY	= "X.Label.RotateBy",
		PROP_WRAP_LABELS		= "X.Label.Wrap",
		// Axes
		PROP_X_AXIS_MIN			= "X.Axis.Min",
		PROP_X_AXIS_MAX			= "X.Axis.Max",
		PROP_Y_AXIS_MIN			= "Y.Axis.Min",
		PROP_Y_AXIS_MAX			= "Y.Axis.Max",
		PROP_Y_AXIS_LOGARITHMIC	= "Y.Axis.Log",
		PROP_XY_GRID			= "Axes.Grid",
		// Bars
		PROP_BAR_BASELINE		= "Bars.Baseline",
		PROP_BAR_PLACEMENT		= "Bar.Placement",
		PROP_BAR_COLOR			= "Bar.Color",
		// Lines
		PROP_DISPLAY_LINE		= "Line.Display",
		PROP_SYMBOL_TYPE		= "Symbols.Type",
		PROP_SYMBOL_SIZE		= "Symbols.Size",
		PROP_LINE_TYPE			= "Line.Type",
		PROP_LINE_COLOR			= "Line.Color",
		// Legend
		PROP_DISPLAY_LEGEND		= "Legend.Display",
		PROP_LEGEND_BORDER		= "Legend.Border",
		PROP_LEGEND_FONT		= "Legend.Font",
		PROP_LEGEND_POSITION	= "Legend.Position",
		PROP_LEGEND_ANCHORING	= "Legend.Anchoring",
		// Plot
		PROP_ANTIALIAS			= "Plot.Antialias",
		PROP_CACHING			= "Plot.Caching";

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

	public enum BarPlacement {
		Aligned,
		Overlap,
		InFront,
		Stacked,
	}

	public enum LegendPosition {
		Inside,
		Above,
		Below,
		Left,
		Right,
	}

	public enum LegendAnchor {
		North,
		NorthEast,
		East,
		SouthEast,
		South,
		SouthWest,
		West,
		NorthWest,
	}
	
	public enum ShowGrid {
		None,
		Major,
		All,
	}

	
	@SuppressWarnings("unchecked")
	public static ChartProperties createPropertySource(Chart chart, ResultFileManager manager) {
		return createPropertySource(chart, chart.getProperties(), manager);
	}
	
	public static ChartProperties createPropertySource(Chart chart, List<Property> properties, ResultFileManager manager) {
		if (chart instanceof BarChart)
			return new ScalarChartProperties(chart, properties, manager);
		else if (chart instanceof LineChart)
			return new VectorChartProperties(chart, properties, manager);
		else if (chart instanceof HistogramChart)
			return new HistogramChartProperties(chart, properties, manager);
		else if (chart instanceof ScatterChart)
			return new ScatterChartProperties(chart, properties, manager);
		else 
			ScavePlugin.logError(new IllegalArgumentException("chart type unrecognized"));
		return new ChartProperties(chart, properties, manager);
	}

	protected Chart chart; 				 // the chart what the properties belongs to
	protected List<Property> properties; // the chart properties, might not be contained by the chart yet
	protected ResultFileManager manager; // result file manager to access chart content (for line properties)
	protected EditingDomain domain;		 // editing domain to execute changes, if null the property list changed directly

	@SuppressWarnings("unchecked")
	public ChartProperties(Chart chart, List<Property> properties, ResultFileManager manager) {
		this.properties = properties;
		this.chart = chart;
		this.manager = manager;
		this.domain = properties == chart.getProperties() ? AdapterFactoryEditingDomain.getEditingDomainFor(chart) : null;
	}

	public List<Property> getProperties() {
		return properties;
	}

	/*======================================================================
	 *                             Main
	 *======================================================================*/
	@org.omnetpp.common.properties.Property(category="Main",id=PROP_ANTIALIAS,displayName="antialias")
	public boolean getAntialias() { return getBooleanProperty(PROP_ANTIALIAS); }
	public void setAntialias(boolean flag) { setProperty(PROP_ANTIALIAS, flag); }
	public boolean defaultAntialias() { return ChartDefaults.DEFAULT_ANTIALIAS; }

	@org.omnetpp.common.properties.Property(category="Main",id=PROP_CACHING,displayName="caching")
	public boolean getCaching() { return getBooleanProperty(PROP_CACHING); }
	public void setCaching(boolean flag) { setProperty(PROP_CACHING, flag); }
	public boolean defaultCaching() { return ChartDefaults.DEFAULT_CANVAS_CACHING; }

	/*======================================================================
	 *                             Titles
	 *======================================================================*/
	@org.omnetpp.common.properties.Property(category="Titles",id=PROP_GRAPH_TITLE)
	public String getGraphTitle() { return getStringProperty(PROP_GRAPH_TITLE); }
	public void setGraphTitle(String title) { setProperty(PROP_GRAPH_TITLE, title); }
	public String defaultGraphTitle() { return ChartDefaults.DEFAULT_TITLE; }

	@org.omnetpp.common.properties.Property(category="Titles",id=PROP_GRAPH_TITLE_FONT)
	public FontData getGraphTitleFont() { return getFontProperty(PROP_GRAPH_TITLE_FONT); }
	public void setGraphTitleFont(FontData font) { setProperty(PROP_GRAPH_TITLE_FONT, font); }
	public FontData getDefaultTitleFont() { return getDefaultFontProperty(PROP_GRAPH_TITLE_FONT); }

	@org.omnetpp.common.properties.Property(category="Titles",id=PROP_X_AXIS_TITLE)
	public String getXAxisTitle() { return getStringProperty(PROP_X_AXIS_TITLE); }
	public void setXAxisTitle(String title) { setProperty(PROP_X_AXIS_TITLE, title); }
	public String defaultXAxisTitle() { return ChartDefaults.DEFAULT_X_AXIS_TITLE; }

	@org.omnetpp.common.properties.Property(category="Titles",id=PROP_Y_AXIS_TITLE)
	public String getYAxisTitle() { return getStringProperty(PROP_Y_AXIS_TITLE); }
	public void setYAxisTitle(String title) { setProperty(PROP_Y_AXIS_TITLE, title); }
	public String defaultYAxisTitle() { return ChartDefaults.DEFAULT_Y_AXIS_TITLE; }

	@org.omnetpp.common.properties.Property(category="Titles",id=PROP_AXIS_TITLE_FONT)
	public FontData getAxisTitleFont() { return getFontProperty(PROP_AXIS_TITLE_FONT); }
	public void setAxisTitleFont(FontData font) { setProperty(PROP_AXIS_TITLE_FONT, font); }
	public FontData defaultAxisTitleFont() { return getDefaultFontProperty(PROP_AXIS_TITLE_FONT); }

	@org.omnetpp.common.properties.Property(category="Titles",id=PROP_LABEL_FONT)
	public FontData getLabelsFont() { return getFontProperty(PROP_LABEL_FONT); }
	public void setLabelsFont(FontData font) { setProperty(PROP_LABEL_FONT, font); }
	public FontData defaultLabelsFont() { return getDefaultFontProperty(PROP_LABEL_FONT); }
	
	@org.omnetpp.common.properties.Property(category="Titles",id=PROP_X_LABELS_ROTATE_BY,displayName="x labels rotated by")
	public String getXLabelsRotate() { return getStringProperty(PROP_X_LABELS_ROTATE_BY); }
	public void setXLabelsRotate(String title) { setProperty(PROP_X_LABELS_ROTATE_BY, title); }
	public String defaultXLabelsRotate() { return String.valueOf(ChartDefaults.DEFAULT_X_LABELS_ROTATED_BY); } // XXX: use Double

	/*======================================================================
	 *                             Axes
	 *======================================================================*/
	@org.omnetpp.common.properties.Property(category="Axes",id=PROP_Y_AXIS_MIN)
	public String getYAxisMin() { return getStringProperty(PROP_Y_AXIS_MIN); }
	public void setYAxisMin(String min) { setProperty(PROP_Y_AXIS_MIN, min); }

	@org.omnetpp.common.properties.Property(category="Axes",id=PROP_Y_AXIS_MAX)
	public String getYAxisMax() { return getStringProperty(PROP_Y_AXIS_MAX); }
	public void setYAxisMax(String max) { setProperty(PROP_Y_AXIS_MAX, max); }

	@org.omnetpp.common.properties.Property(category="Axes",id=PROP_Y_AXIS_LOGARITHMIC)
	public boolean getYAxisLogarithmic() { return getBooleanProperty(PROP_Y_AXIS_LOGARITHMIC); }
	public void setYAxisLogarithmic(boolean flag) { setProperty(PROP_Y_AXIS_LOGARITHMIC, flag); }
	public boolean defaultYAxisLogarithmic() { return ChartDefaults.DEFAULT_Y_AXIS_LOGARITHMIC; }

	@org.omnetpp.common.properties.Property(category="Axes",id=PROP_XY_GRID,displayName="grid")
	public ShowGrid getXYGrid() { return getEnumProperty(PROP_XY_GRID, ShowGrid.class); }
	public void setXYGrid(ShowGrid showgrid) { setProperty(PROP_XY_GRID, showgrid); }
	public ShowGrid defaultXYGrid() { return ChartDefaults.DEFAULT_SHOW_GRID; }

	/*======================================================================
	 *                             Legend
	 *======================================================================*/
	@org.omnetpp.common.properties.Property(category="Legend", id=PROP_DISPLAY_LEGEND,
			displayName="display")
	public boolean getDisplayLegend() { return getBooleanProperty(PROP_DISPLAY_LEGEND); }
	public void setDisplayLegend(boolean flag) { setProperty(PROP_DISPLAY_LEGEND, flag); }
	public boolean defaultDisplayLegend() { return ChartDefaults.DEFAULT_DISPLAY_LEGEND; }

	@org.omnetpp.common.properties.Property(category="Legend", id=PROP_LEGEND_BORDER,
			displayName="border")
	public boolean getLegendBorder() { return getBooleanProperty(PROP_LEGEND_BORDER); }
	public void setLegendBorder(boolean flag) { setProperty(PROP_LEGEND_BORDER, flag); }
	public boolean defaultLegendBorder() { return ChartDefaults.DEFAULT_LEGEND_BORDER; }

	@org.omnetpp.common.properties.Property(category="Legend", id=PROP_LEGEND_FONT,
			displayName="font")
	public FontData getLegendFont() { return getFontProperty(PROP_LEGEND_FONT); }
	public void setLegendFont(FontData font) { setProperty(PROP_LEGEND_FONT, font); }
	public FontData defaultLegendFont() { return getDefaultFontProperty(PROP_LEGEND_FONT); }

	@org.omnetpp.common.properties.Property(category="Legend", id=PROP_LEGEND_POSITION,
			displayName="position")
	public LegendPosition getLegendPosition() { return getEnumProperty(PROP_LEGEND_POSITION, LegendPosition.class); }
	public void setLegendPosition(LegendPosition position) { setProperty(PROP_LEGEND_POSITION, position); }
	public LegendPosition defaultLegendPosition() { return ChartDefaults.DEFAULT_LEGEND_POSITION; }

	@org.omnetpp.common.properties.Property(category="Legend",id=PROP_LEGEND_ANCHORING,
			displayName="anchor point")
	public LegendAnchor getLegendAnchoring() { return getEnumProperty(PROP_LEGEND_ANCHORING, LegendAnchor.class); }
	public void setLegendAnchoring(LegendAnchor anchoring) { setProperty(PROP_LEGEND_ANCHORING, anchoring); }
	public LegendAnchor defaultLegendAnchor() { return ChartDefaults.DEFAULT_LEGEND_ANCHOR; }

	/*======================================================================
	 *                             Line
	 *======================================================================*/
	private static final String DEFAULT_LINE_PROPERTIES_ID = "default";
	
	public class LineProperties extends PropertySource {
		private String lineId;

		public LineProperties(String lineId) {
			this.lineId = lineId;
		}

		private String propertyName(String baseName) {
			return lineId == null ? baseName : baseName + "/" + lineId;
		}

		@org.omnetpp.common.properties.Property(category="Lines",id=PROP_DISPLAY_LINE,optional=true)
		public boolean getDisplayLine() { return getBooleanProperty(propertyName(PROP_DISPLAY_LINE)); }
		public void setDisplayLine(boolean display) { setProperty(propertyName(PROP_DISPLAY_LINE), display); }
		public boolean defaultDisplayLine() { return ChartDefaults.DEFAULT_DISPLAY_LINE; }

		@org.omnetpp.common.properties.Property(category="Lines",id=PROP_SYMBOL_TYPE,optional=true)
		public SymbolType getSymbolType() { return getEnumProperty(propertyName(PROP_SYMBOL_TYPE), SymbolType.class); }
		public void setSymbolType(SymbolType type) { setProperty(propertyName(PROP_SYMBOL_TYPE), type); }
		public SymbolType defaultSymbolType() { return null; }

		@org.omnetpp.common.properties.Property(category="Lines",id=PROP_SYMBOL_SIZE)
		public String getSymbolSize() { return getStringProperty(propertyName(PROP_SYMBOL_SIZE)); }
		public void setSymbolSize(String size) { setProperty(propertyName(PROP_SYMBOL_SIZE), size); }
		public String defaultSymbolSize() { return null; }

		@org.omnetpp.common.properties.Property(category="Lines",id=PROP_LINE_TYPE,optional=true)
		public LineType getLineType() { return getEnumProperty(propertyName(PROP_LINE_TYPE), LineType.class); }
		public void setLineType(LineType style) { setProperty(propertyName(PROP_LINE_TYPE), style); }
		public LineType defaultLineType() { return null; }

		@org.omnetpp.common.properties.Property(category="Lines",id=PROP_LINE_COLOR,descriptorClass=ColorPropertyDescriptor.class,optional=true)
		public String getLineColor() { return getStringProperty(propertyName(PROP_LINE_COLOR)); }
		public void setLineColor(String color) { setProperty(propertyName(PROP_LINE_COLOR), color); }
		public String defaultLineColor() { return null; }
	}
	
	public class LinesPropertySource extends BasePropertySource {
		IPropertyDescriptor[] descriptors;

		public LinesPropertySource() {
			this.descriptors = createLineDescriptors();
		}

		public IPropertyDescriptor[] getPropertyDescriptors() {
			return descriptors;
		}

		public Object getPropertyValue(Object id) {
			return new LineProperties(id == DEFAULT_LINE_PROPERTIES_ID ? null : (String)id);
		}
		
		protected IPropertyDescriptor[] createLineDescriptors() {
			IPropertyDescriptor[] descriptors;
			IXYDataset dataset = DatasetManager.createXYDataset(chart, null, false, manager, null);
			
			if (dataset != null) {
				descriptors = new IPropertyDescriptor[dataset.getSeriesCount() + 1];
				descriptors[0] = new PropertyDescriptor(DEFAULT_LINE_PROPERTIES_ID, "default");
				for (int i= 0; i < dataset.getSeriesCount(); ++i) {
					String key = dataset.getSeriesKey(i);
					descriptors[i+1] = new PropertyDescriptor(key, key);
				}
			}
			else
				descriptors = new IPropertyDescriptor[0];
			
			return descriptors;
		}
	}

	public static class VectorChartProperties extends ChartProperties
	{
		public VectorChartProperties(Chart chart, List<Property> properties, ResultFileManager manager) {
			super(chart, properties, manager);
		}
		/*======================================================================
		 *                             Axes
		 *======================================================================*/
		@org.omnetpp.common.properties.Property(category="Axes",id=PROP_X_AXIS_MIN)
		public String getXAxisMin() { return getStringProperty(PROP_X_AXIS_MIN); }
		public void setXAxisMin(String min) { setProperty(PROP_X_AXIS_MIN, min); }

		@org.omnetpp.common.properties.Property(category="Axes",id=PROP_X_AXIS_MAX)
		public String getXAxisMax() { return getStringProperty(PROP_X_AXIS_MAX); }
		public void setXAxisMax(String max) { setProperty(PROP_X_AXIS_MAX, max); }

		/*======================================================================
		 *                             Lines
		 *======================================================================*/
		@org.omnetpp.common.properties.Property(category="Plot",id="Lines",displayName="Lines")
		public LinesPropertySource getLineProperties() { return new LinesPropertySource(); }

		public LineProperties getLineProperties(String lineId) { return new LineProperties(lineId); }
	}
	
	public class BarColorsPropertySource extends BasePropertySource {
		IPropertyDescriptor[] descriptors;

		public BarColorsPropertySource() {
			this.descriptors = createBarDescriptors();
		}

		public IPropertyDescriptor[] getPropertyDescriptors() {
			return descriptors;
		}

		@Override
		public Object getPropertyValue(Object id) {
			return getStringProperty((String)id);
		}

		@Override
		public void setPropertyValue(Object id, Object value) {
			setProperty((String)id, (String)value);				
		}
		
		@Override
		public boolean isPropertyResettable(Object id) {
			return true;
		}

		@Override
		public boolean isPropertySet(Object id) {
			return getPropertyValue(id) != null;
		}

		@Override
		public void resetPropertyValue(Object id) {
			setPropertyValue(id, null);
		}

		protected IPropertyDescriptor[] createBarDescriptors() {
			IPropertyDescriptor[] descriptors;
			
			ScalarDataset dataset = DatasetManager.createScalarDataset((BarChart)chart, manager, null);
			String[] names = new String[dataset.getColumnCount()];
			for (int i = 0; i < names.length; ++i)
				names[i] = dataset.getColumnKey(i);
			if (names != null) {
				descriptors = new IPropertyDescriptor[names.length + 1];
				descriptors[0] = new ColorPropertyDescriptor(PROP_BAR_COLOR, "default"); //new PropertyDescriptor(DEFAULT_LINE_PROPERTIES_ID, "default");
				for (int i= 0; i < names.length; ++i)
					descriptors[i+1] = new ColorPropertyDescriptor(PROP_BAR_COLOR + "/" + names[i], names[i]); // new PropertyDescriptor(names[i], names[i]);
			}
			else
				descriptors = new IPropertyDescriptor[0];
			
			return descriptors;
		}
	}
	

	public static class ScalarChartProperties extends ChartProperties
	{
		public ScalarChartProperties(Chart chart, List<Property> properties, ResultFileManager manager) {
			super(chart, properties, manager);
		}

		/*======================================================================
		 *                             Titles
		 *======================================================================*/
		@org.omnetpp.common.properties.Property(category="Titles",id=PROP_WRAP_LABELS)
		public boolean getWrapLabels() { return getBooleanProperty(PROP_WRAP_LABELS); }
		public void setWrapLabels(boolean wrap) { setProperty(PROP_WRAP_LABELS, wrap); }
		public boolean defaultWrapLabels() { return ChartDefaults.DEFAULT_WRAP_LABELS; }

		/*======================================================================
		 *                             Bars
		 *======================================================================*/
		@org.omnetpp.common.properties.Property(category="Bars",id=PROP_BAR_BASELINE)
		public String getBarBaseline() { return getStringProperty(PROP_BAR_BASELINE); } // XXX return Double
		public void setBarBaseline(String baseline) { setProperty(PROP_BAR_BASELINE, baseline); }
		public String defaultBarBaseline() { return String.valueOf(ChartDefaults.DEFAULT_BAR_BASELINE); }

		@org.omnetpp.common.properties.Property(category="Bars",id=PROP_BAR_PLACEMENT)
		public BarPlacement getBarPlacement() { return getEnumProperty(PROP_BAR_PLACEMENT, BarPlacement.class); }
		public void setBarPlacement(BarPlacement placement) { setProperty(PROP_BAR_PLACEMENT, placement); }
		public BarPlacement defaultBarPlacement() { return ChartDefaults.DEFAULT_BAR_PLACEMENT; }
		
		@org.omnetpp.common.properties.Property(category="Bars",id="Colors",displayName="Colors")
		public BarColorsPropertySource getBarProperties() { return new BarColorsPropertySource(); }
	}

	public static class HistogramChartProperties extends ChartProperties
	{
		public HistogramChartProperties(Chart chart, List<Property> properties, ResultFileManager manager) {
			super(chart, properties, manager);
		}

		// TODO
	}

	public static class ScatterChartProperties extends VectorChartProperties
	{
		public ScatterChartProperties(Chart chart, List<Property> properties, ResultFileManager manager) {
			super(chart, properties, manager);
			Assert.isLegal(chart == null || chart instanceof ScatterChart);
		}
	}

	/*======================================================================
	 *                             Generic interface
	 *======================================================================*/

	public Property getProperty(String propertyName) {
		for (Property property : properties)
			if (property.getName().equals(propertyName))
				return property;
		return null;
	}

	public String getStringProperty(String propertyName) {
		Property property = getProperty(propertyName);
		return property != null ? StringUtils.defaultString(property.getValue()) : 
								  getDefaultStringProperty(propertyName);
	}

	public Boolean getBooleanProperty(String propertyName) {
		Property property = getProperty(propertyName);
		return property != null ? Boolean.valueOf(property.getValue()) :
								  getDefaultBooleanProperty(propertyName);
	}

	public <T extends Enum<T>> T getEnumProperty(String propertyName, Class<T> type) {
		Property property = getProperty(propertyName);
		return property != null && property.getValue() != null ? Enum.valueOf(type, property.getValue()) :
																 getDefaultEnumProperty(propertyName, type);
	}

	public FontData getFontProperty(String propertyName) {
		Property property = getProperty(propertyName);
		return property != null ? Converter.stringToFontdata(property.getValue()) :
			                      getDefaultFontProperty(propertyName);
	}
	
	public RGB getColorProperty(String propertyName) {
		Property property = getProperty(propertyName);
		return property != null ? Converter.stringToRGB(property.getValue()) :
								  getDefaultColorProperty(propertyName);
	}

	/**
	 * Sets the property value in the property list.
	 * If the value is null then the property node is deleted.
	 * If the editing domain was set it will change the list by executing a command,
	 * otherwise it modifies the list directly.
	 */
	protected void doSetProperty(String propertyName, String propertyValue) {
		ScaveModelPackage model = ScaveModelPackage.eINSTANCE;
		ScaveModelFactory factory = ScaveModelFactory.eINSTANCE;
		Property property = getProperty(propertyName);

		if (property == null && propertyValue != null ) { // add new property
			property = factory.createProperty();
			property.setName(propertyName);
			property.setValue(propertyValue);
			if (domain == null)
				properties.add(property);
			else
				domain.getCommandStack().execute(
					AddCommand.create(domain, chart, model.getChart_Properties(), property));
		}
		else if (property != null && propertyValue != null) { // change existing property
			if (domain == null)
				property.setValue(propertyValue);
			else
				domain.getCommandStack().execute(
					SetCommand.create(domain, property,	model.getProperty_Value(), propertyValue));
		}
		else if (property != null && propertyValue == null){ // delete existing property
			if (domain == null)
				properties.remove(property);
			else
				domain.getCommandStack().execute(
					RemoveCommand.create(domain, property));
		}
	}
	
	public void setProperty(String propertyName, String propertyValue) {
		String defaultValue = getDefaultStringProperty(propertyName);
		if (defaultValue != null && defaultValue.equals(propertyValue))
			propertyValue = null;
		doSetProperty(propertyName, propertyValue);
	}

	public void setProperty(String propertyName, Boolean propertyValue) {
		Boolean defaultValue = getDefaultBooleanProperty(propertyName);
		if (defaultValue != null && defaultValue.equals(propertyValue))
			propertyValue = null;
		doSetProperty(propertyName, propertyValue == null ? null : String.valueOf(propertyValue));
	}

	@SuppressWarnings("unchecked")
	public void setProperty(String propertyName, Enum<?> propertyValue) {
		Enum<?> defaultValue = propertyValue == null ? null : getDefaultEnumProperty(propertyName, propertyValue.getClass());
		if (defaultValue != null && defaultValue.equals(propertyValue))
			propertyValue = null;
		doSetProperty(propertyName, propertyValue == null ? null : propertyValue.name());
	}

	public void setProperty(String propertyName, FontData propertyValue) {
		FontData defaultValue = getDefaultFontProperty(propertyName);
		if (defaultValue != null && defaultValue.equals(propertyValue))
			propertyValue = null;
		doSetProperty(propertyName, Converter.fontdataToString(propertyValue));
	}
	
	public void setProperty(String propertyName, RGB propertyValue) {
		RGB defaultValue = getDefaultColorProperty(propertyName);
		if (defaultValue != null && defaultValue.equals(propertyValue))
			propertyValue = null;
		doSetProperty(propertyName, Converter.rgbToString(propertyValue));
	}

	public String getDefaultStringProperty(String propertyName) {
		Object defaultValue = ChartDefaults.getDefaultPropertyValue(propertyName);
		if (defaultValue instanceof String)
			return (String)defaultValue;
		else
			return StringUtils.EMPTY;
	}
	
	public boolean getDefaultBooleanProperty(String propertyName) {
		Object defaultValue = ChartDefaults.getDefaultPropertyValue(propertyName);
		if (defaultValue instanceof Boolean)
			return (Boolean)defaultValue;
		else
			return false;
	}
	
	@SuppressWarnings("unchecked")
	public <T extends Enum<T>> T getDefaultEnumProperty(String propertyName, Class<T> type) {
		Object defaultValue = ChartDefaults.getDefaultPropertyValue(propertyName);
		if (defaultValue != null && type.isInstance(defaultValue))
			return (T)defaultValue;
		else
			return null;
	}
	
	public FontData getDefaultFontProperty(String propertyName) {
		Object defaultValue = ChartDefaults.getDefaultPropertyValue(propertyName);
		if (defaultValue instanceof Font)
			return Converter.swtfontToFontdata((Font)defaultValue);
		else
			return null;
	}
	
	public RGB getDefaultColorProperty(String propertyName) {
		Object defaultValue = ChartDefaults.getDefaultPropertyValue(propertyName);
		if (defaultValue instanceof RGB)
			return (RGB)defaultValue;
		else
			return null;
	}
}
