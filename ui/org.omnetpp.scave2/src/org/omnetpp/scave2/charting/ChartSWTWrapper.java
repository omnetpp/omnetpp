package org.omnetpp.scave2.charting;

import java.awt.Font;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.awt.image.DataBuffer;
import java.awt.image.DataBufferInt;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.BusyIndicator;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.graphics.PaletteData;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.jfree.chart.ChartRenderingInfo;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.Axis;
import org.jfree.chart.event.ChartChangeEvent;
import org.jfree.chart.event.ChartChangeListener;
import org.jfree.chart.plot.CategoryPlot;
import org.jfree.chart.plot.Plot;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.title.TextTitle;
import org.omnetpp.common.util.Converter;

import static org.omnetpp.scave2.model.ChartProperties.*;

/**
 * Allows a JFreeChart chart to be displayed in SWT.
 * Uses off-screen rendering, and caches the image for efficiency.
 * It also does double buffering, to avoid flicker.
 *
 * Automatically refreshes when chart changes; refresh manually when
 * widget size changes.
 *
 * @author andras
 */
public class ChartSWTWrapper extends Canvas {

	// the chart
	protected JFreeChart chart;

	// listener that refreshes the display when the chart changes
	private ChartChangeListener listener;

	// geometry info about the chart
	protected ChartRenderingInfo renderingInfo;

	// cached image of the chart
	private Image image;

	//private boolean renderingScheduled = false;

	private Rectangle rubberBand = null;

	// stores the bounds of the data area, to which the rubber band has to be clipped
	private Rectangle rubberBandBounds = null;

	public ChartSWTWrapper(Composite parent, int style) {
		super(parent, style | SWT.NO_BACKGROUND);

		renderingInfo = new ChartRenderingInfo();
		renderingInfo.setEntityCollection(null); // otherwise it would eat up heaps of memory for large charts!

		addPaintListener(new PaintListener(){
			   public void paintControl(PaintEvent event){
			     repaintChart(event);
			   }
			});
	}
	
	/*=============================================
	 *               Properties   
	 *=============================================*/
	public void setProperty(String name, String value) {
		if (chart == null)
			return;
		
		if (PROP_GRAPH_TITLE.equals(name))
			chart.setTitle(value);
		else if (PROP_GRAPH_TITLE_FONT.equals(name))
			setTitleFont(Converter.stringToAwtfont(value));
		else if (PROP_X_AXIS_TITLE.equals(name))
			setXAxisTitle(value);
		else if (PROP_Y_AXIS_TITLE.equals(name))
			setYAxisTitle(value);
		else if (PROP_AXIS_TITLE_FONT.equals(name))
			setAxisTitleFont(Converter.stringToAwtfont(value));
		else if (PROP_LABEL_FONT.equals(name))
			setLabelFont(Converter.stringToAwtfont(value));
		else if (PROP_X_LABELS_ROTATE_BY.equals(name))
			setXAxisRotatedBy(value);
	}
	
	public void setTitle(String title) {
		if (chart != null)
			chart.setTitle(title);
	}
	
	public void setTitleFont(Font font) {
		if (chart == null || font == null)
			return;
		TextTitle mainTitle = chart.getTitle();
		if (mainTitle != null)
			mainTitle.setFont(font);
		else
			chart.setTitle(new TextTitle("", font));
	}
	
	public void setXAxisTitle(String title) {
		Axis xAxis = getDomainAxis();
		if (xAxis != null)
			xAxis.setLabel(title);
	}
	
	public void setYAxisTitle(String title) {
		Axis yAxis = getRangeAxis();
		if (yAxis != null)
			yAxis.setLabel(title);
	}
	
	public void setAxisTitleFont(Font font) {
		if (chart == null || font == null)
			return;

		Axis xAxis = getDomainAxis();
		Axis yAxis = getRangeAxis();
		if (xAxis != null)
			xAxis.setLabelFont(font);
		if (yAxis != null)
			yAxis.setLabelFont(font);
	}
	
	public void setLabelFont(Font font) {
		if (chart == null || font == null)
			return;
		
		Axis xAxis = getDomainAxis();
		Axis yAxis = getRangeAxis();
		if (xAxis != null)
			xAxis.setTickLabelFont(font);
		if (yAxis != null)
			yAxis.setTickLabelFont(font);
	}
	
	public void setXAxisRotatedBy(String value) {
		Double angle = Converter.stringToDouble(value);
		if (chart == null || angle == null)
			return;
		
		Axis axis = getDomainAxis();
		if (axis != null)
			axis.setLabelAngle(Math.toRadians(angle));
	}
	
	public void setSize(int width, int height) {
		if (width != SWT.DEFAULT || height != SWT.DEFAULT) {
			Point size = getSize();
			super.setSize(width != SWT.DEFAULT ? width : size.x, height != SWT.DEFAULT ? height : size.y);
		}
	}
	
	private Axis getDomainAxis() {
		if (chart != null) {
			Plot plot = chart.getPlot();
			if (plot instanceof CategoryPlot)
				return ((CategoryPlot)plot).getDomainAxis();
			else if (plot instanceof XYPlot)
				return ((XYPlot)plot).getDomainAxis();
		}
		return null;
	}

	private Axis getRangeAxis() {
		if (chart != null) {
			Plot plot = chart.getPlot();
			if (plot instanceof CategoryPlot)
				return ((CategoryPlot)plot).getRangeAxis();
			else if (plot instanceof XYPlot)
				return ((XYPlot)plot).getRangeAxis();
		}
		return null;
	}
	
	/**
	 * Called when an area was dragged out in the chart.
	 * This default implementation does nothing.
	 */
	public void rubberBandSelectionMade(Rectangle r) {}

	/**
	 * Called when the chart was single-clicked with the left mouse button.
	 * This default implementation does nothing.
	 */
	public void clicked(int x, int y) {}

	/**
	 * Implements rubber band selection. Successful selections
	 * will cause rubberBandSelectionMade() to be called.
	 */
	public void enableRubberBand() {
		final Canvas self = this;

		addMouseListener(new MouseListener() {
	    	public void mouseDown(MouseEvent e) {
	    		if (rubberBand==null && e.button==1 && rubberBandBounds.contains(e.x, e.y)) {
					// start selection
					rubberBand = new Rectangle(e.x, e.y, 0, 0);
					GC gc = new GC(self);
					drawRubberBand(gc, rubberBand);
				}
	    		if (rubberBand!=null && e.button!=1) {
	    			// cancel selection
		    		rubberBand = null;
		    		redraw();
	    		}
	    	}
	    	public void mouseUp(MouseEvent e) {
				if (rubberBand!=null) {
	    			// finish/cancel selection
					redraw();
					if (e.button==1) {
						// note: firing a SelectionEvent wouldn't work (width,
						// height won't make it through Listener.handleEvent())
						fixNegativeSizes(rubberBand);
						if (rubberBand.width<=2 && rubberBand.height<=2)
							clicked(e.x, e.y);
						else
							rubberBandSelectionMade(rubberBand);
					}
					rubberBand = null;
	    		}
	    	}
			public void mouseDoubleClick(MouseEvent e) {
			}
	    });

		addMouseMoveListener(new MouseMoveListener() {
			public void mouseMove(MouseEvent e) {
				if (rubberBand!=null) {
					// erase, then draw new with updated coordinates
					GC gc = new GC(self);
					gc.setForeground(getDisplay().getSystemColor(SWT.COLOR_RED));
					drawRubberBand(gc, rubberBand);
					rubberBand.width = e.x - rubberBand.x;
					rubberBand.height = e.y - rubberBand.y;
					clipToBounds(rubberBand, rubberBandBounds);
					drawRubberBand(gc, rubberBand);
				}
			}
	    });
	}

	private static void drawRubberBand(GC gc, Rectangle rect) {
		// needed because gc.drawFocus() doesn't accept negative width/height
		Rectangle r = new Rectangle(rect.x, rect.y, rect.width, rect.height);
		fixNegativeSizes(r);
		gc.drawFocus(r.x, r.y, r.width, r.height);
	}

	private static void fixNegativeSizes(Rectangle r) {
		if (r.width<0) {
			r.width = -r.width;
			r.x -= r.width;
		}
		if (r.height<0) {
			r.height = -r.height;
			r.y -= r.height;
		}
	}

	private static void clipToBounds(Rectangle r, Rectangle r2) {
		// r may have negative height/width which intersect() doesn't like.
		// so first fix this, then calculate intersect, then restore negative width/height
		boolean widthNegative = r.width<0;
		boolean heightNegative = r.height<0;
		if (r.width<0) {
			r.width = -r.width;
			r.x -= r.width;
		}
		if (r.height<0) {
			r.height = -r.height;
			r.y -= r.height;
		}
		r.intersect(r2);
		if (widthNegative) {
			r.x += r.width;
			r.width = -r.width;
		}
		if (heightNegative) {
			r.y += r.height;
			r.height = -r.height;
		}
	}

	public void setChart(JFreeChart newChart) {
		// deregister from old chart
		if (listener!=null && chart!=null) {
			chart.removeChangeListener(listener);
		}

		// set chart and register ourselves with it.
		chart = newChart;
		if (listener==null) {
			listener = new ChartChangeListener() {
				public void chartChanged(ChartChangeEvent arg0) {
					refresh();
				}};
		}
		chart.addChangeListener(listener);
        refresh();
	}

	public JFreeChart getChart() {
		return chart;
	}

	public void refresh() {
		renderChart();
		redraw();
	}

	private void renderChart() {
		final Rectangle b = getBounds();

		// during the first call, the canvas size is zero yet
		if (b.width==0)
	    	b.width = 600;
	    if (b.height==0)
	    	b.height = 400;
		if (image!=null)
			image.dispose();

        // show busy mouse pointer while we render the chart
		final Canvas self = this;
		BusyIndicator.showWhile(getDisplay(), new Thread() {
			public void run() {
				image = createChartImage(self, chart, b.width, b.height, renderingInfo);
				Rectangle2D r = renderingInfo.getPlotInfo().getDataArea();
				// TODO check: BUG in JFreeChart: returned height/width is 1-2 pixels smaller than actual?
				rubberBandBounds = new Rectangle((int)r.getX(), (int)r.getY(),
						(int)r.getWidth()+2, (int)r.getHeight()+1);
			}
		});

	}

	private void scheduleChartRendering() {
        // This code doesn't work perfectly -- there'll bere too many redraws scheduled
		//		// this is something like Tcl's "after idle".
		//		if (!renderingScheduled) { // FIXME do we need some locking etc?
		//			renderingScheduled = true;
		//			Display.getDefault().asyncExec(new Runnable(){
		//				public void run() {
		//					renderChart();
		//					redraw();
		//					renderingScheduled = false;
		//				}
		//			});
		//		}
	}

	private void repaintChart(PaintEvent event) {
		if (image==null) {
			 // no chart, just delete the background
			event.gc.drawRectangle(event.x, event.y, event.width, event.height);
		}
		else {
			 // adjust background
			
			 // if sizes differ, we should redraw it sometime
			 Rectangle b = getBounds();
			 Rectangle ib = image.getBounds();
			 if (b.height!=0 && b.width!=0 && !b.equals(ib))
				 scheduleChartRendering();

			 // transfer image to screen
			 event.gc.drawImage(image,0,0);

			// System.out.println("repaint():canvas bounds: " + b.width + "," + b.height);
			// System.out.println("repaint():image  bounds: " + ib.width + "," + ib.height);

			 // clear the left and bottom strips
			 if (b.width>ib.width)
				 event.gc.fillRectangle(ib.width, 0, b.width-ib.width, b.height);
			 if (b.height>ib.height)
				 event.gc.fillRectangle(0, ib.height, b.width, b.height-ib.height);
		}
	}

	/**
	 * Render a JFreeChart to an SWT Image.
	 *
	 * @param parent
	 * @param chart
	 * @param width
	 * @param height
	 * @return
	 */
	public static Image createChartImage(Control parent, JFreeChart chart,
			int width, int height, ChartRenderingInfo renderingInfo)
	{
		// Color adjustment
		Color swtBackground = parent.getBackground();
		java.awt.Color awtBackground = new java.awt.Color(swtBackground.getRed(),
				swtBackground.getGreen(),
				swtBackground.getBlue());
		chart.setBackgroundPaint(awtBackground);

		// Draw the chart in an AWT buffered image
		long time = System.currentTimeMillis();
		BufferedImage bufferedImage = chart.createBufferedImage(width, height, renderingInfo);
		System.out.println("Chart rendering took " + (System.currentTimeMillis() - time) + " ms.");
		// Get the data buffer of the image
		DataBuffer buffer = bufferedImage.getRaster().getDataBuffer();
		DataBufferInt intBuffer = (DataBufferInt) buffer;

		// Copy the data from the AWT buffer to a SWT buffer
		PaletteData paletteData = new PaletteData(0x00FF0000, 0x0000FF00, 0x000000FF);
		ImageData imageData = new ImageData(width, height, 32, paletteData);
		for (int bank = 0; bank < intBuffer.getNumBanks(); bank++) {
			int[] bankData = intBuffer.getData(bank);
			imageData.setPixels(0, bank, bankData.length, bankData, 0);
		}

		// Create an SWT image
		return new Image(parent.getDisplay(), imageData);
	}
}
