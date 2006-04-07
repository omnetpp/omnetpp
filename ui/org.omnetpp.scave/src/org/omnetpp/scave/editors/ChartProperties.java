package org.omnetpp.scave.editors;

import java.awt.Font;
import java.awt.Paint;

import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.ui.views.properties.ColorPropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.Axis;
import org.jfree.chart.axis.CategoryAxis;
import org.jfree.chart.block.BlockBorder;
import org.jfree.chart.plot.CategoryPlot;
import org.jfree.chart.plot.Plot;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.title.LegendTitle;
import org.jfree.chart.title.TextTitle;
import org.jfree.chart.title.Title;
import org.jfree.ui.HorizontalAlignment;
import org.jfree.ui.RectangleEdge;
import org.jfree.ui.RectangleInsets;
import org.jfree.ui.VerticalAlignment;

import org.omnetpp.scave.charting.ChartSWTWrapper;
import org.omnetpp.scave.properties.CheckboxPropertyDescriptor;
import org.omnetpp.scave.properties.EnumPropertyDescriptor;
import org.omnetpp.scave.properties.FontPropertyDescriptor;
import org.omnetpp.scave.properties.NumberPropertyDescriptor;
import org.omnetpp.scave.properties.Property;
import org.omnetpp.scave.properties.PropertySource;

public class ChartProperties extends PropertySource {

	private ChartSWTWrapper swtChart;
	
	public ChartProperties(ChartSWTWrapper swtChart)
	{
		this.swtChart = swtChart;
	}
	
	public JFreeChart getChart() {
		return swtChart.getChart();
	}
	
	@Property
	public TitleProperties getTitle() {
		return new TitleProperties(getChart().getTitle());
	}
	
	@Property
	public LegendProperties getLegend() {
		return new LegendProperties(getChart().getLegend());
	}
	
	@Property
	public PlotProperties getPlot() {
		Plot plot = getChart().getPlot();
		if (plot instanceof CategoryPlot)
			return new CategoryPlotProperties((CategoryPlot)plot);
		else
			return new PlotProperties(getChart().getPlot());
	}
	
	@Property
	public AxesProperties getAxes() {
		Plot plot = getChart().getPlot();
		if (plot instanceof CategoryPlot)
			return new CategoryPlotAxesProperties((CategoryPlot)plot);
		else if (plot instanceof XYPlot)
			return new XYPlotAxesProperties((XYPlot)plot);
		else
			return null;
	}
	
	private void refresh() {
		swtChart.refresh();
	}
	

	public class TitleProperties extends PropertySource
	{
		private TextTitle title;
		
		public TitleProperties(TextTitle title)
		{
			this.title = title;
		}
		
		@Property(descriptorClass = TextPropertyDescriptor.class)
		public String getText() { return title.getText(); }
		public void setText(String text) { title.setText(text); }
		
		@Property(descriptorClass = FontPropertyDescriptor.class)
		public FontData getFont() { return fontToFontData(title.getFont()); }
		public void setFont(FontData font) { title.setFont(fontDataToFont(font)); }

		@Property(descriptorClass = ColorPropertyDescriptor.class)
		public RGB getColor() { return paintToRGB(title.getPaint()); }
		public void setColor(RGB rgb) { title.setPaint(rgbToPaint(rgb)); }

		@Property(descriptorClass = EnumPropertyDescriptor.class,
				  category = "Layout")
		public RectangleEdge getPosition() { return title.getPosition(); }
		public void setPosition(RectangleEdge pos) { title.setPosition(pos); }
		
		@Property(descriptorClass = EnumPropertyDescriptor.class,
				  displayName = "horizontal alignment",
				  category = "Layout")
		public HorizontalAlignment getHorizontalAlignment() { return title.getHorizontalAlignment(); }
		public void setHorizontalAlignment(HorizontalAlignment alignment) { title.setHorizontalAlignment(alignment); }
		public HorizontalAlignment defaultHorizontalAlignment() { return Title.DEFAULT_HORIZONTAL_ALIGNMENT; }
		
		@Property(descriptorClass = EnumPropertyDescriptor.class,
				  displayName = "vertical alignment",
				  category = "Layout")
		public VerticalAlignment getVerticalAlignment() { return title.getVerticalAlignment(); }
		public void setVerticalAlignment(VerticalAlignment alignment) { title.setVerticalAlignment(alignment); }
		public VerticalAlignment defaultVerticalAlignment() { return Title.DEFAULT_VERTICAL_ALIGNMENT; }
			
		@Property(category = "Layout")
		public InsetsProperties getPadding() { return new InsetsProperties(title.getPadding()); }
		public void setPadding(InsetsProperties padding) { title.setPadding(padding.getValue()); }
	}
	
	public class LegendProperties extends PropertySource
	{
		private LegendTitle legend;
		
		public LegendProperties(LegendTitle legend)
		{
			this.legend = legend;
		}
		
		@Property(descriptorClass = ColorPropertyDescriptor.class)
		public RGB getBackground() { return paintToRGB(legend.getBackgroundPaint()); }
		public void setBackground(RGB rgb) { legend.setBackgroundPaint(rgbToPaint(rgb)); refresh(); }
		
		@Property
		public InsetsProperties getMargin()	{
			return new InsetsProperties(legend.getMargin());
		}
		public void setMargin(InsetsProperties margin) {
			legend.setMargin(margin.getValue());
			refresh(); 
		}
		
		@Property(descriptorClass = ColorPropertyDescriptor.class)
		public RGB getBorder() { return paintToRGB(legend.getBorder().getPaint()); }
		public void setBorder(RGB rgb) { 
			BlockBorder border = legend.getBorder();
			legend.setBorder(new BlockBorder(border.getInsets(), rgbToPaint(rgb)));
			refresh();
		}
		
		@Property(descriptorClass = EnumPropertyDescriptor.class)
		public RectangleEdge getPosition() { return legend.getPosition(); }
		public void setPosition(RectangleEdge pos) { legend.setPosition(pos); refresh(); }
		
		@Property(descriptorClass = FontPropertyDescriptor.class,
				  displayName = "item font")
		public FontData getItemFont() { return fontToFontData(legend.getItemFont()); }
		public void setItemFont(FontData fd) { legend.setItemFont(fontDataToFont(fd)); refresh(); }

		@Property(descriptorClass = ColorPropertyDescriptor.class,
				  displayName = "item color")
		public RGB getItemColor() { return paintToRGB(legend.getItemPaint()); }
		public void setItemColor(RGB rgb) { legend.setItemPaint(rgbToPaint(rgb)); refresh(); }
	}
	
	public class PlotProperties extends PropertySource
	{
		Plot plot;
		
		public PlotProperties(Plot plot) {
			this.plot = plot;
		}
		
		@Property(descriptorClass = ColorPropertyDescriptor.class,
				  category = "Background")
		public RGB getBackground() { return paintToRGB(plot.getBackgroundPaint()); }
		public void setBackground(RGB rgb) { plot.setBackgroundPaint(rgbToPaint(rgb)); }
		public RGB defaultBackground() { return paintToRGB(Plot.DEFAULT_BACKGROUND_PAINT); }
		
		@Property(descriptorClass = ColorPropertyDescriptor.class,
				  category = "Outline")
		public RGB getOutline() { return paintToRGB(plot.getOutlinePaint()); }
		public void setOutline(RGB rgb) { plot.setOutlinePaint(rgbToPaint(rgb)); }
		public RGB defaultOutline() { return paintToRGB(Plot.DEFAULT_OUTLINE_PAINT); }
		
		@Property(category = "Outline")
		public InsetsProperties getInsets() { return new InsetsProperties(plot.getInsets()); }
		public void setInsets(InsetsProperties insets) { plot.setInsets(insets.getValue()); }
	}
	
	public class CategoryPlotProperties extends PlotProperties
	{
		CategoryPlot plot;

		public CategoryPlotProperties(CategoryPlot plot) {
			super(plot);
			this.plot = plot;
		}
		
		@Property(descriptorClass = CheckboxPropertyDescriptor.class,
				  displayName = "domain grid visible",
				  category = "Grid")
		public boolean getDomainGridVisible() { return plot.isDomainGridlinesVisible(); }
		public void setDomainGridVisible(boolean value) { plot.setDomainGridlinesVisible(value); }
		
		@Property(descriptorClass = CheckboxPropertyDescriptor.class,
				  displayName = "range grid visible",
				  category = "Grid")
		public boolean getRangeGridVisible() { return plot.isRangeGridlinesVisible(); }
		public void setRangeGridVisible(boolean value) { plot.setRangeGridlinesVisible(value); }
		
		
	}
	
	public class AxesProperties extends PropertySource
	{
	}
	
	public class CategoryPlotAxesProperties extends AxesProperties
	{
		CategoryPlot plot;
		
		public CategoryPlotAxesProperties(CategoryPlot plot) {
			this.plot = plot;
		}
		
		@Property(displayName = "domain axis")
		public AxisProperties getDomainAxis() {
			return new AxisProperties(plot.getDomainAxis());
		}
		
		@Property(displayName = "range axis")
		public AxisProperties getRangeAxis() {
			return new AxisProperties(plot.getRangeAxis());
		}
	}
	
	public class XYPlotAxesProperties extends AxesProperties
	{
		XYPlot plot;
		
		public XYPlotAxesProperties(XYPlot plot) {
			this.plot = plot;
		}

		@Property(displayName = "domain axis")
		public AxisProperties getDomainAxis() {
			return new AxisProperties(plot.getDomainAxis());
		}
		
		@Property(displayName = "range axis")
		public AxisProperties getRangeAxis() {
			return new AxisProperties(plot.getRangeAxis());
		}
	}
	
	public class AxisProperties extends PropertySource
	{
		Axis axis;
		
		public AxisProperties(Axis axis) {
			this.axis = axis;
		}
		
		@Property(descriptorClass = TextPropertyDescriptor.class,
				  displayName = "label text",
				  category = "Label")
		public String getLabel() { return axis.getLabel(); }
		public void setLabel(String label) { axis.setLabel(label); }
		
		@Property(descriptorClass = FontPropertyDescriptor.class,
				  displayName = "label font",
				  category = "Label")
		public FontData getLabelFont() { return fontToFontData(axis.getLabelFont()); }
		public void setLabelFont(FontData fd) { axis.setLabelFont(fontDataToFont(fd)); }
		
		@Property(descriptorClass = ColorPropertyDescriptor.class,
				  displayName = "label color",
				  category = "Label")
		public RGB getLabelColor() { return paintToRGB(axis.getLabelPaint()); }
		public void setLabelColor(RGB rgb) { axis.setLabelPaint(rgbToPaint(rgb)); }
		
		@Property(descriptorClass = CheckboxPropertyDescriptor.class,
				  displayName = "tick labels visible",
				  category = "TickLabel")
		public boolean getTickLabelsVisible() { return axis.isTickLabelsVisible(); }
		public void setTickLabelsVisible(boolean value) { axis.setTickLabelsVisible(value); }
		
		@Property(descriptorClass = FontPropertyDescriptor.class,
				  displayName = "tick label font",
				  category = "TickLabel")
		public FontData getTickLabelsFont() { return fontToFontData(axis.getTickLabelFont()); }
		public void setTickLabelsFont(FontData fd) { axis.setTickLabelFont(fontDataToFont(fd)); }
		
		@Property(descriptorClass = CheckboxPropertyDescriptor.class,
				  displayName = "tick label color",
				  category = "TickLabel")
		public RGB getTickLabelsColor() { return paintToRGB(axis.getTickLabelPaint()); }
		public void setTickLabelsColor(RGB rgb) { axis.setTickLabelPaint(rgbToPaint(rgb)); }
		
		@Property(descriptorClass = CheckboxPropertyDescriptor.class,
				  displayName = "tick marks visible",
				  category = "TickMark")
		public boolean getTickMarksVisible() { return axis.isTickMarksVisible(); }
		public void setTickMarksVisible(boolean value) { axis.setTickMarksVisible(value); }

		@Property(descriptorClass = CheckboxPropertyDescriptor.class,
				  displayName = "tick marks color",
				  category = "TickMark")
		public RGB getTickMarksColor() { return paintToRGB(axis.getTickMarkPaint()); }
		public void setTickMarksColor(RGB rgb) { axis.setTickMarkPaint(rgbToPaint(rgb)); }
	}
	
	public class InsetsProperties extends PropertySource
	{
		public double top;
		public double left;
		public double bottom;
		public double right;
		
		public InsetsProperties(RectangleInsets insets)
		{
			top = insets.getTop();
			left = insets.getLeft();
			bottom = insets.getBottom();
			right = insets.getRight();
		}
		
		public RectangleInsets getValue()
		{
			return new RectangleInsets(top, left, bottom, right);
		}
		
		@Property(descriptorClass = NumberPropertyDescriptor.class)
		public double getTop() { return top; }
		public void setTop(double value) { top = value; }
		
		@Property(descriptorClass = NumberPropertyDescriptor.class)
		public double getLeft() { return left; }
		public void setLeft(double value) { left = value; }

		@Property(descriptorClass = NumberPropertyDescriptor.class)
		public double getBottom() { return bottom; }
		public void setBottom(double value) { bottom = value; }

		@Property(descriptorClass = NumberPropertyDescriptor.class)
		public double getRight() { return right; }
		public void setRight(double value) { right = value; }
	}
	
	public class CategoryAxisProperties extends PropertySource
	{
		CategoryAxis axis;

		@Property()
		public double getLowerMargin() { return axis.getLowerMargin(); }
		public void setLowerMargin(double value) { axis.setLowerMargin(value); }
	}

	/*
	 * Conversions between AWT and SWT types. 
	 */
	
	private static RGB paintToRGB(Paint paint)
	{
		if (paint instanceof java.awt.Color) {
			java.awt.Color awtColor = (java.awt.Color)paint;
			return new RGB(awtColor.getRed(), awtColor.getGreen(), awtColor.getBlue());
		}
		return null;
	}
	
	private static Paint rgbToPaint(RGB rgb)
	{
		return new java.awt.Color(rgb.red, rgb.green, rgb.blue);
	}
	
	private static FontData fontToFontData(Font font) {
		return new FontData(font.getName(), font.getSize(), font.getStyle());
	}
	
	private static Font fontDataToFont(FontData fd) {
		return new Font(fd.getName(), fd.getStyle(), fd.getHeight());
	}
}






