package org.omnetpp.scave.charting;

import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_ANTIALIAS;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_CANVAS_CACHING;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_DISPLAY_LEGEND;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_INSETS_BACKGROUND_COLOR;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_INSETS_LINE_COLOR;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_LEGEND_ANCHOR;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_LEGEND_BORDER;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_LEGEND_FONT;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_LEGEND_POSITION;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_TITLE;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_TITLE_FONT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_ANTIALIAS;
import static org.omnetpp.scave.charting.ChartProperties.PROP_CACHING;
import static org.omnetpp.scave.charting.ChartProperties.PROP_DISPLAY_LEGEND;
import static org.omnetpp.scave.charting.ChartProperties.PROP_GRAPH_TITLE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_GRAPH_TITLE_FONT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LEGEND_ANCHORING;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LEGEND_BORDER;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LEGEND_FONT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LEGEND_POSITION;

import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.ClipboardOwner;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.ColorConstants;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ControlAdapter;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.canvas.ZoomableCachingCanvas;
import org.omnetpp.common.canvas.ZoomableCanvasMouseSupport;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.image.ImageConverter;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.util.Converter;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.ChartProperties.LegendAnchor;
import org.omnetpp.scave.charting.ChartProperties.LegendPosition;
import org.omnetpp.scave.charting.dataset.IDataset;
import org.omnetpp.scave.charting.plotter.IChartSymbol;

/**
 * Base class for all chart widgets.
 * 
 * @author tomi, andras
 */
public abstract class ChartCanvas extends ZoomableCachingCanvas {

	protected boolean antialias = DEFAULT_ANTIALIAS;
	protected Title title = new Title(DEFAULT_TITLE, DEFAULT_TITLE_FONT);
	protected Legend legend = new Legend(DEFAULT_DISPLAY_LEGEND, DEFAULT_LEGEND_BORDER, DEFAULT_LEGEND_FONT, DEFAULT_LEGEND_POSITION, DEFAULT_LEGEND_ANCHOR);
	protected LegendTooltip legendTooltip = new LegendTooltip();
	
	private String statusText = "No data available."; // displayed when there's no dataset 

	private ZoomableCanvasMouseSupport mouseSupport;
	private Color insetsBackgroundColor = DEFAULT_INSETS_BACKGROUND_COLOR;
	private Color insetsLineColor = DEFAULT_INSETS_LINE_COLOR;
	
	public ChartCanvas(Composite parent, int style) {
		super(parent, style);
		setCaching(DEFAULT_CANVAS_CACHING);
		setBackground(ColorConstants.white);

		mouseSupport = new ZoomableCanvasMouseSupport(this); // add mouse handling; may be made optional
		
		addControlListener(new ControlAdapter() {
			@Override
			public void controlResized(ControlEvent e) {
				layoutChart();
			}
		});
	}

	/**
	 * Sets the data to be visualized by the chart.
	 */
	abstract void setDataset(IDataset dataset);
	
	/**
	 * Calculate positions of chart elements such as title, legend, axis labels, plot area. 
	 */
	abstract protected void layoutChart();
	
	/**
	 * Switches between zoom and pan mode. 
	 * @param mouseMode should be ZoomableCanvasMouseSupport.PAN_MODE or ZoomableCanvasMouseSupport.ZOOM_MODE
	 */
	public void setMouseMode(int mouseMode) {
		mouseSupport.setMouseMode(mouseMode);
	}

	public int getMouseMode() {
		return mouseSupport.getMouseMode();
	}

	public String getStatusText() {
		return statusText;
	}

	public void setStatusText(String statusText) {
		this.statusText = statusText;
		chartChanged();
	}
	
	public void setProperty(String name, String value) {
		// Titles
		if (PROP_GRAPH_TITLE.equals(name))
			setTitle(value);
		else if (PROP_GRAPH_TITLE_FONT.equals(name))
			setTitleFont(Converter.stringToSwtfont(value));
		// Legend
		else if (PROP_DISPLAY_LEGEND.equals(name))
			setDisplayLegend(Converter.stringToBoolean(value));
		else if (PROP_LEGEND_BORDER.equals(name))
			setLegendBorder(Converter.stringToBoolean(value));
		else if (PROP_LEGEND_FONT.equals(name))
			setLegendFont(Converter.stringToSwtfont(value));
		else if (PROP_LEGEND_POSITION.equals(name))
			setLegendPosition(Converter.stringToEnum(value, LegendPosition.class));
		else if (PROP_LEGEND_ANCHORING.equals(name))
			setLegendAnchor(Converter.stringToEnum(value, LegendAnchor.class));
		// Plot
		else if (PROP_ANTIALIAS.equals(name))
			setAntialias(Converter.stringToBoolean(value));
		else if (PROP_CACHING.equals(name))
			setCaching(Converter.stringToBoolean(value));
		else
			ScavePlugin.logError(new RuntimeException("unrecognized chart property: "+name));
	}
	
	public boolean getAntialias() {
		return antialias;
	}

	public void setAntialias(Boolean antialias) {
		this.antialias = antialias != null && antialias.booleanValue();
		chartChanged();
	}
	
	public void setCaching(Boolean caching) {
		super.setCaching(caching != null && caching.booleanValue());
		chartChanged();
	}
	
	public void setTitle(String value) {
		if (value == null)
			value = DEFAULT_TITLE;
		title.setText(value);
		chartChanged();
	}

	public void setTitleFont(Font value) {
		if (value == null)
			value = DEFAULT_TITLE_FONT;
		title.setFont(value);
		chartChanged();
	}
	
	public void setDisplayLegend(Boolean value) {
		if (value == null)
			value = DEFAULT_DISPLAY_LEGEND;
		legend.setVisible(value);
		chartChanged();
	}
	
	public void setLegendBorder(Boolean value) {
		if (value == null)
			value = DEFAULT_LEGEND_BORDER;
		legend.setDrawBorder(value);
		chartChanged();
	}
	
	public void setLegendFont(Font value) {
		if (value == null)
			value = DEFAULT_LEGEND_FONT;
		legend.setFont(value);
		chartChanged();
	}
	
	public void setLegendPosition(LegendPosition value) {
		if (value == null)
			value = DEFAULT_LEGEND_POSITION;
		legend.setPosition(value);
		chartChanged();
	}
	
	public void setLegendAnchor(LegendAnchor value) {
		if (value == null)
			value = DEFAULT_LEGEND_ANCHOR;
		legend.setAnchor(value);
		chartChanged();
	}
	
	/**
	 * Resets all GC settings except clipping and transform.
	 */
	public static void resetDrawingStylesAndColors(GC gc) {
		gc.setAntialias(SWT.DEFAULT);
		gc.setAlpha(255);
		gc.setBackground(ColorConstants.white);
		gc.setBackgroundPattern(null);
		//gc.setFillRule();
		gc.setFont(null);
		gc.setForeground(ColorConstants.black);
		gc.setForegroundPattern(null);
		gc.setInterpolation(SWT.DEFAULT);
		//gc.setLineCap();
		gc.setLineDash(null);
		//gc.setLineJoin();
		gc.setLineStyle(SWT.LINE_SOLID);
		gc.setLineWidth(1);
		//gc.setXORMode(false);
		gc.setTextAntialias(SWT.DEFAULT);
	}
	
	protected void chartChanged() {
		layoutChart();
		clearCanvasCacheAndRedraw();
	}
	
	/**
	 * Copies the image of the chart to the clipboard.
	 * Uses AWT functionality, because SWT does not support ImageTransfer yet.
	 * See: https://bugs.eclipse.org/bugs/show_bug.cgi?id=78856.
	 */
	public void copyToClipboard() {
		Clipboard cp = java.awt.Toolkit.getDefaultToolkit().getSystemClipboard();
		ClipboardOwner owner = new java.awt.datatransfer.ClipboardOwner() {
			public void lostOwnership(Clipboard clipboard, Transferable contents) {
			}
		};
		
		class ImageTransferable implements Transferable {
			public java.awt.Image image;

			public ImageTransferable(java.awt.Image image) {
				this.image = image;
			}
			
			public Object getTransferData(DataFlavor flavor) throws UnsupportedFlavorException, IOException {
				if (flavor == DataFlavor.imageFlavor)
					return image;
				else
					throw new UnsupportedFlavorException(flavor);
			}

			public DataFlavor[] getTransferDataFlavors() {
				return new DataFlavor[] {DataFlavor.imageFlavor};
			}

			public boolean isDataFlavorSupported(DataFlavor flavor) {
				return flavor == DataFlavor.imageFlavor;
			}
		};
		
		cp.setContents(new ImageTransferable(ImageConverter.convertToAWT(getImage())), owner);
	}
	
	/**
	 * Returns the image of the chart.
	 */
	public Image getImage() {
		Image image = new Image(getDisplay(), getClientArea().width, getClientArea().height);
		GC gc = new GC(image);
		paint(gc);
		gc.dispose();
		return image;
	}
	
	protected void paintInsets(GC gc) {
		// draw insets border
		Insets insets = getInsets();
		Rectangle canvasRect = new Rectangle(getClientArea());
		gc.setForeground(insetsBackgroundColor);
		gc.setBackground(insetsBackgroundColor);
		gc.fillRectangle(0, 0, canvasRect.width, insets.top); // top
		gc.fillRectangle(0, canvasRect.bottom()-insets.bottom, canvasRect.width, insets.bottom); // bottom
		gc.fillRectangle(0, 0, insets.left, canvasRect.height); // left
		gc.fillRectangle(canvasRect.right()-insets.right, 0, insets.right, canvasRect.height); // right
		gc.setForeground(insetsLineColor);
		gc.drawRectangle(insets.left, insets.top, getViewportWidth(), getViewportHeight());
	}

	protected void drawStatusText(GC gc) {
		if (getStatusText() != null) {
			resetDrawingStylesAndColors(gc);
			org.eclipse.swt.graphics.Rectangle rect = getViewportRectangle();
			gc.drawText(getStatusText(), rect.x+10, rect.y+10);
		}
	}

	protected static class PlotArea {
		public double minX;
		public double maxX;
		public double minY;
		public double maxY;

		public PlotArea(double minX, double maxX, double minY, double maxY) {
			this.minX = minX;
			this.maxX = maxX;
			this.minY = minY;
			this.maxY = maxY;
		}
		
	}
	
	/**
	 * Legend tooltip.
	 */
	class LegendTooltip
	{
		Image icon = ImageFactory.getImage(ImageFactory.TOOLBAR_IMAGE_LEGEND);
		Rectangle rect; // rectangle of the area
		List<Item> items = new ArrayList<Item>();
		
		class Item
		{
			Color color;
			String label;
			IChartSymbol symbol;
			String imageFile;

			public Item(Color color, String label, IChartSymbol symbol) {
				this.color = color;
				this.label = label;
				this.symbol = symbol;
				this.imageFile = createImageFile();
			}
			
			private String createImageFile() {
				Image image = null;
				try {
					image = createSymbolImage(symbol, color);
					String symbolName = StringUtils.removeStart(symbol.getClass().getName(), symbol.getClass().getPackage().getName()+".");
					String imageName = String.format("%s_%02X%02X%02X", symbolName, color.getRed(), color.getGreen(), color.getBlue());
					return ImageFactory.createTemporaryImageFile(imageName+".png", image, SWT.IMAGE_PNG);
				} catch (Exception e) {
					return null;
				}
				finally {
					if (image != null)
						image.dispose();
				}
			}
			
			private Image createSymbolImage(IChartSymbol symbol, Color color) {
				int size = symbol.getSizeHint();
				Image image = null;
				GC gc = null;
				try {
					symbol.setSizeHint(6);
					image = new Image(null, 15, 9);
					gc = new GC(image);
					gc.setAntialias(SWT.ON);
					gc.setForeground(color);
					gc.setLineWidth(1);
					gc.setLineStyle(SWT.LINE_SOLID);
					gc.drawLine(0, 4, 14, 4);
					symbol.drawSymbol(gc, 7, 4);
				}
				finally {
					symbol.setSizeHint(size);
					if (gc != null)
						gc.dispose();
				}
				return image;
			}
		}
		
		public LegendTooltip() {
			HoverSupport hoverSupport = new HoverSupport();
			hoverSupport.setHoverSizeConstaints(new Point(320,400));
			hoverSupport.adapt(ChartCanvas.this, new IHoverTextProvider() {
				public String getHoverTextFor(Control control, int x, int y, Point preferedSize) {
					return getTooltipText(x, y, preferedSize);
				}
			});
		}
		
		public void clearItems() {
			items.clear();
		}
		
		public void addItem(Color color, String label, IChartSymbol symbol) {
			items.add(new Item(color, label, symbol));
		}
		
		public Rectangle layout(GC gc, Rectangle rect) {
			int width = icon.getBounds().width;
			int height = icon.getBounds().height;
			this.rect = new Rectangle(rect.right() - width - 2, rect.y + 2,  width, height);
			return rect;
		}
		
		public void draw(GC gc) {
			System.out.println("Legend tooltip: " + rect);
			Graphics graphics = new SWTGraphics(gc);
			graphics.pushState();
			graphics.drawImage(icon, rect.x, rect.y);
			graphics.popState();
		}
		
		public String getTooltipText(int x, int y, Point preferedSize) {
			if (rect.contains(x, y) && items.size() > 0) {
				StringBuffer sb = new StringBuffer();
				int height = 25;
				sb.append("<b>Legend:</b>"); height += 10;
				sb.append("<table style='margin-left: 1em'>");
				for (Item item : items) {
					sb.append("<tr>");
					if (item.imageFile != null)
						sb.append("<td style='vertical-align: middle'>").
							append("<img src='file://").append(item.imageFile).append("'></td>"); // XXX URLEncoded filename does not work in IE
					sb.append("<td style='vectical-align: middle; color: ").append(htmlColor(item.color)).append("'>").
						append(htmlText(item.label)).append("</td>"); // XXX quote
					sb.append("</tr>");
					height += 10;
				}
				sb.append("</table>");
				preferedSize.y = Math.max(height, 80);
				return HoverSupport.addHTMLStyleSheet(sb.toString());
			}
			else
				return null;
		}
		
		public String htmlColor(Color color) {
			return String.format("#%02X%02X%02X", color.getRed(), color.getGreen(), color.getBlue());
		}
		
		public String htmlText(String text) {
			text = text.replace("<", "&lt;");
			text = text.replace("&", "&amp;");
			text = text.replace(">", "&gt;");
			return text;
		}
	}
}
