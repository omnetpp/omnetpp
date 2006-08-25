package org.omnetpp.scave2.model;

import java.util.List;

import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.command.RemoveCommand;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.emf.edit.domain.AdapterFactoryEditingDomain;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.CheckboxCellEditor;
import org.eclipse.jface.viewers.ComboBoxCellEditor;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TextCellEditor;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.common.properties.EnumCellEditor;
import org.omnetpp.common.properties.PropertySource;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.DatasetType;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model.ScaveModelPackage;

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
		// Axes
		PROP_X_AXIS_MIN			= "X.Axis.Min",
		PROP_X_AXIS_MAX			= "X.Axis.Max",
		PROP_Y_AXIS_MIN			= "Y.Axis.Min",
		PROP_Y_AXIS_MAX			= "Y.Axis.Max",
		PROP_X_AXIS_LOGARITHMIC = "X.Axis.Log",
		PROP_Y_AXIS_LOGARITHMIC	= "Y.Axis.Log",
		PROP_XY_INVERT			= "Axes.Invert",
		PROP_XY_GRID			= "Axes.Grid",
		// Bars
		PROP_BAR_BASELINE		= "Bars.Baseline",
		PROP_BAR_PLACEMENT		= "Bar.Placement",
		// Lines
		PROP_DISPLAY_SYMBOLS	= "Symbols.Display",
		PROP_SYMBOL_TYPE		= "Symbols.Type",
		PROP_SYMBOL_SIZE		= "Symbols.Size",
		PROP_LINE_TYPE			= "Line.Type",
		PROP_HIDE_LINE			= "Lines.Hide",
		// Legend
		PROP_DISPLAY_LEGEND		= "Legend.Display",
		PROP_LEGEND_BORDER		= "Legend.Border",
		PROP_LEGEND_FONT		= "Legend.Font",
		PROP_LEGEND_POSITION	= "Legend.Position",
		PROP_LEGEND_ANCHORING	= "Legend.Anchoring";

	public enum ShowGrid {
		None,
		AtMajorTicks,
		AtAllTicks,
	}
	
	public enum LineStyle {
		None,
		Linear,
		Step,
		Spline,
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
	
	public static ChartProperties createPropertySource(Chart chart) {
		DatasetType type = ScaveModelUtil.getDatasetType(chart);
		switch (type.getValue()) {
		case DatasetType.SCALAR: return new ScalarChartProperties(chart);
		case DatasetType.VECTOR: return new VectorChartProperties(chart);
		case DatasetType.HISTOGRAM: return new HistogramChartProperties(chart);
		default: return new ChartProperties(chart);
		}
	}
	
	private List<Property> properties;
	private Chart owner;
	
	public ChartProperties(List<Property> properties) {
		this.properties = properties;
		this.owner = null;
	}
	
	public ChartProperties(Chart chart) {
		this.properties = chart.getProperties();
		this.owner = chart;
	}
	
	public List<Property> getProperties() {
		return properties;
	}
	
	/*======================================================================
	 *                             Titles
	 *======================================================================*/
	@org.omnetpp.common.properties.Property(category="Titles")
	public String getGraphTitle() { return getStringProperty(PROP_GRAPH_TITLE); }
	public void setGraphTitle(String title) { setProperty(PROP_GRAPH_TITLE, title); }
	
	@org.omnetpp.common.properties.Property(category="Titles")
	public String getGraphTitleFont() { return getStringProperty(PROP_GRAPH_TITLE_FONT); }
	public void setGraphTitleFont(String font) { setProperty(PROP_GRAPH_TITLE_FONT, font); }
	
	@org.omnetpp.common.properties.Property(category="Titles")
	public String getXAxisTitle() { return getStringProperty(PROP_X_AXIS_TITLE); }
	public void setXAxisTitle(String title) { setProperty(PROP_X_AXIS_TITLE, title); }
	
	@org.omnetpp.common.properties.Property(category="Titles")
	public String getYAxisTitle() { return getStringProperty(PROP_Y_AXIS_TITLE); }
	public void setYAxisTitle(String title) { setProperty(PROP_Y_AXIS_TITLE, title); }
	
	@org.omnetpp.common.properties.Property(category="Titles")
	public String getAxisTitleFont() { return getStringProperty(PROP_AXIS_TITLE_FONT); }
	public void setAxisTitleFont(String title) { setProperty(PROP_AXIS_TITLE_FONT, title); }
	
	@org.omnetpp.common.properties.Property(category="Titles")
	public String getLabelsFont() { return getStringProperty(PROP_LABEL_FONT); }
	public void setLabelsFont(String title) { setProperty(PROP_LABEL_FONT, title); }
	
	@org.omnetpp.common.properties.Property(category="Titles", displayName="x labels rotated by")
	public String getXLabelsRotate() { return getStringProperty(PROP_X_LABELS_ROTATE_BY); }
	public void setXLabelsRotate(String title) { setProperty(PROP_X_LABELS_ROTATE_BY, title); }
	/*======================================================================
	 *                             Axes
	 *======================================================================*/
	@org.omnetpp.common.properties.Property(category="Axes")
	public String getXAxisMin() { return getStringProperty(PROP_X_AXIS_MIN); }
	public void setXAxisMin(String min) { setProperty(PROP_X_AXIS_MIN, min); }
	
	@org.omnetpp.common.properties.Property(category="Axes")
	public String getXAxisMax() { return getStringProperty(PROP_X_AXIS_MAX); }
	public void setXAxisMax(String max) { setProperty(PROP_X_AXIS_MAX, max); }
	
	@org.omnetpp.common.properties.Property(category="Axes")
	public String getYAxisMin() { return getStringProperty(PROP_Y_AXIS_MIN); }
	public void setYAxisMin(String min) { setProperty(PROP_Y_AXIS_MIN, min); }
	
	@org.omnetpp.common.properties.Property(category="Axes")
	public String getYAxisMax() { return getStringProperty(PROP_Y_AXIS_MAX); }
	public void setYAxisMax(String max) { setProperty(PROP_Y_AXIS_MAX, max); }
	
	@org.omnetpp.common.properties.Property(category="Axes")
	public boolean getXAxisLogarithmic() { return getBooleanProperty(PROP_X_AXIS_LOGARITHMIC); }
	public void setXAxisLogarithmic(boolean flag) { setProperty(PROP_X_AXIS_LOGARITHMIC, flag); }
	
	@org.omnetpp.common.properties.Property(category="Axes")
	public boolean getYAxisLogarithmic() { return getBooleanProperty(PROP_Y_AXIS_LOGARITHMIC); }
	public void setYAxisLogarithmic(boolean flag) { setProperty(PROP_Y_AXIS_LOGARITHMIC, flag); }
	
	@org.omnetpp.common.properties.Property(category="Axes")
	public boolean getXYInvert() { return getBooleanProperty(PROP_XY_INVERT); }
	public void setXYInvert(boolean flag) { setProperty(PROP_XY_INVERT, flag); }

	@org.omnetpp.common.properties.Property(category="Axes")
	public ShowGrid getXYGrid() { return getEnumProperty(PROP_XY_GRID, ShowGrid.class); }
	public void setXYGrid(ShowGrid showgrid) { setProperty(PROP_XY_GRID, showgrid); }

	/*======================================================================
	 *                             Legend
	 *======================================================================*/
	@org.omnetpp.common.properties.Property(category="Legend")
	public boolean getDisplayLegend() { return getBooleanProperty(PROP_DISPLAY_LEGEND); }
	public void setDisplayLegend(boolean flag) { setProperty(PROP_DISPLAY_LEGEND, flag); }
	
	@org.omnetpp.common.properties.Property(category="Legend")
	public boolean getLegendBorder() { return getBooleanProperty(PROP_LEGEND_BORDER); }
	public void setLegendBorder(boolean flag) { setProperty(PROP_LEGEND_BORDER, flag); }
	
	@org.omnetpp.common.properties.Property(category="Legend")
	public String getLegendFont() { return getStringProperty(PROP_LEGEND_FONT); }
	public void setLegendFont(String font) { setProperty(PROP_LEGEND_FONT, font); }
	
	@org.omnetpp.common.properties.Property(category="Legend")
	public LegendPosition getLegendPosition() { return getEnumProperty(PROP_LEGEND_POSITION, LegendPosition.class); }
	public void setLegendPosition(LegendPosition position) { setProperty(PROP_LEGEND_POSITION, position); }
	
	@org.omnetpp.common.properties.Property(category="Legend")
	public LegendAnchor getLegendAnchoring() { return getEnumProperty(PROP_LEGEND_ANCHORING, LegendAnchor.class); }
	public void setLegendAnchoring(LegendAnchor anchoring) { setProperty(PROP_LEGEND_ANCHORING, anchoring); }

	
	public static class VectorChartProperties extends ChartProperties
	{
		public VectorChartProperties(List<Property> properties) {
			super(properties);
		}
		
		public VectorChartProperties(Chart chart) {
			super(chart);
		}
		
		/*======================================================================
		 *                             Lines
		 *======================================================================*/
		@org.omnetpp.common.properties.Property(category="Lines")
		public boolean getDisplaySymbols() { return getBooleanProperty(PROP_DISPLAY_SYMBOLS); }
		public void setDisplaySymbols(boolean flag) { setProperty(PROP_DISPLAY_SYMBOLS, flag); }

		@org.omnetpp.common.properties.Property(category="Lines")
		public String getSymbolType() { return getStringProperty(PROP_SYMBOL_TYPE); }
		public void setSymbolType(String type) { setProperty(PROP_SYMBOL_TYPE, type); }
		
		@org.omnetpp.common.properties.Property(category="Lines")
		public String getSymbolSize() { return getStringProperty(PROP_SYMBOL_SIZE); }
		public void setSymbolSize(String size) { setProperty(PROP_SYMBOL_SIZE, size); }
		
		@org.omnetpp.common.properties.Property(category="Lines")
		public LineStyle getLineType() { return getEnumProperty(PROP_LINE_TYPE, LineStyle.class); }
		public void setLineType(LineStyle style) { setProperty(PROP_LINE_TYPE, style); }
		
		@org.omnetpp.common.properties.Property(category="Lines")
		public boolean getHideLine() { return getBooleanProperty(PROP_HIDE_LINE); }
		public void setHideLine(boolean flag) { setProperty(PROP_HIDE_LINE, flag); }
	}

	public static class ScalarChartProperties extends ChartProperties
	{
		public ScalarChartProperties(List<Property> properties) {
			super(properties);
		}
		
		public ScalarChartProperties(Chart chart) {
			super(chart);
		}
		
		/*======================================================================
		 *                             Bars
		 *======================================================================*/
		@org.omnetpp.common.properties.Property(category="Bars")
		public String getBarBaseline() { return getStringProperty(PROP_BAR_BASELINE); }
		public void setBarBaseline(String baseline) { setProperty(PROP_BAR_BASELINE, baseline); }

		@org.omnetpp.common.properties.Property(category="Bars")
		public BarPlacement getBarPlacement() { return getEnumProperty(PROP_BAR_PLACEMENT, BarPlacement.class); }
		public void setBarPlacement(BarPlacement placement) { setProperty(PROP_BAR_PLACEMENT, placement); }
	}
	
	public static class HistogramChartProperties extends ChartProperties
	{
		public HistogramChartProperties(List<Property> properties) {
			super(properties);
		}
		
		public HistogramChartProperties(Chart chart) {
			super(chart);
		}
		
		// TODO
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
		return property != null ? StringUtils.defaultString(property.getValue()) : StringUtils.EMPTY;
	}
	
	public Boolean getBooleanProperty(String propertyName) {
		Property property = getProperty(propertyName);
		return property != null ? Boolean.valueOf(property.getValue()) : Boolean.FALSE;
	}
	
	public <T extends Enum<T>> T getEnumProperty(String propertyName, Class<T> type) {
		Property property = getProperty(propertyName);
		return property != null && property.getValue() != null ? Enum.valueOf(type, property.getValue()) : null;
	}
	
	public void setProperty(String propertyName, String propertyValue) {
		EditingDomain domain = getEditingDomain();
		ScaveModelPackage model = ScaveModelPackage.eINSTANCE;
		ScaveModelFactory factory = ScaveModelFactory.eINSTANCE;
		Property property = getProperty(propertyName);
		
		if ("".equals(propertyValue))
			propertyValue = null;

		if (property == null && propertyValue != null ) {
			property = factory.createProperty();
			property.setName(propertyName);
			property.setValue(propertyValue);
			if (domain == null)
				properties.add(property);
			else
				domain.getCommandStack().execute(
					AddCommand.create(domain, owner, model.getChart_Properties(), property));
		}
		else if (property != null && propertyValue != null) {
			if (domain == null)
				property.setValue(propertyValue);
			else
				domain.getCommandStack().execute(
					SetCommand.create(domain, property,	model.getProperty_Value(), propertyValue));
		}
		else if (property != null && propertyValue == null){
			if (domain == null)
				properties.remove(property);
			else
				domain.getCommandStack().execute(
					RemoveCommand.create(domain, property)); 
		}
	}
	
	public void setProperty(String propertyName, Boolean propertyValue) {
		setProperty(propertyName, propertyValue == null || propertyValue == Boolean.FALSE ?  null : String.valueOf(propertyValue));
	}
	
	public void setProperty(String propertyName, Enum<?> propertyValue) {
		setProperty(propertyName, propertyValue == null ? null : String.valueOf(propertyValue));
	}
	
	private EditingDomain getEditingDomain() {
		return owner != null ? AdapterFactoryEditingDomain.getEditingDomainFor(owner) : null;
	}
}
