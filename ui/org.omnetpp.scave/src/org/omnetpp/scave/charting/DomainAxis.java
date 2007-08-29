package org.omnetpp.scave.charting;

import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_AXIS_TITLE_FONT;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_LABELS_FONT;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_X_LABELS_ROTATED_BY;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.TextLayout;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.GeomUtils;
import org.omnetpp.scave.charting.ScalarChart.BarPlot;
import org.omnetpp.scave.charting.dataset.IScalarDataset;

/**
 * Domain axis for bar chart.
 */
class DomainAxis {
	ScalarChart chart;
	Rectangle rect; // strip below the plotArea where the axis text etc goes
	GroupLabelLayoutData[] layoutData;
	int labelsHeight;
	String title = DEFAULT_X_AXIS_TITLE;
	Font titleFont = DEFAULT_AXIS_TITLE_FONT;
	Font labelsFont = DEFAULT_LABELS_FONT;
	double rotation = DEFAULT_X_LABELS_ROTATED_BY;
	int gap = 4;  // between chart and axis 
	
	static class GroupLabelLayoutData {
		TextLayout textLayout;
		Dimension size;
		Dimension rotatedSize;
	}
	
	public DomainAxis(ScalarChart chart) {
		this.chart = chart;
	}
	
	public void dispose() {
		for (GroupLabelLayoutData data : layoutData)
			if (data != null && data.textLayout != null)
				data.textLayout.dispose();
	}
	
	public LinearTicks getTicks() {
		return new LinearTicks(1.0, 0.0, 1.0); // TODO
	}

	/**
	 * Modifies insets to accomodate room for axis title, ticks, tick labels etc.
	 * Also returns insets for convenience. 
	 */
	public Insets layoutHint(GC gc, Rectangle rect, Insets insets) {

		// measure title height and labels height
		gc.setFont(titleFont);
		int titleHeight = title.equals("") ? 0 : gc.textExtent(title).y;
		gc.setFont(labelsFont);
		labelsHeight = 0;
		IScalarDataset dataset = chart.getDataset();
		BarPlot plot = chart.getPlot();
		if (dataset != null) {
			int cColumns = dataset.getColumnCount();
			int cRows = dataset.getRowCount();
			if (layoutData != null) {
				for (GroupLabelLayoutData data : layoutData)
					data.textLayout.dispose();
			}
			layoutData = new GroupLabelLayoutData[cRows];
			for (int row = 0; row < cRows; ++row) {
				int left = plot.getBarRectangle(row, 0).x;
				int right = plot.getBarRectangle(row, cColumns - 1).right();
				int maxWidth = Math.max(right - left, 10);
				int maxHeight = Math.max(rect.height / 2, 10);
				layoutData[row] = layoutGroupLabel(dataset.getRowKey(row),
										labelsFont, rotation, gc, maxWidth, maxHeight);
				labelsHeight = Math.max(labelsHeight, layoutData[row].rotatedSize.height);
				//System.out.println("labelsheight: "+labelsHeight);
			}
		}
		
		// modify insets with space required
		insets.top = Math.max(insets.top, 10); // leave a few pixels at the top
		insets.bottom = Math.max(insets.bottom, gap + labelsHeight + titleHeight + 8);
		
		return insets;
	}
	
	private GroupLabelLayoutData layoutGroupLabel(String label, Font font, double rotation , GC gc, int maxWidth, int maxHeight) {
		GroupLabelLayoutData data = new GroupLabelLayoutData();
		data.textLayout = new TextLayout(gc.getDevice());
		data.textLayout.setText(label);
		data.textLayout.setFont(font);
		data.textLayout.setAlignment(SWT.CENTER);
		data.textLayout.setWidth(maxWidth);
		//System.out.format("width=%s%n", maxWidth);
		if (data.textLayout.getLineCount() > 1) {
			// TODO soft hyphens are visible even when no break at them 
			data.textLayout.setText(label.replace(';', '\u00ad'));
		}
		org.eclipse.swt.graphics.Rectangle bounds = data.textLayout.getBounds(); 
		data.size = new Dimension(bounds.width, Math.min(bounds.height, maxHeight)); //new Dimension(gc.textExtent(data.label));
		data.rotatedSize = GeomUtils.rotatedSize(data.size, rotation);
		return data;
	}

	/**
	 * Sets geometry info used for drawing. Plot area = bounds minus insets.
	 */
	public void setLayout(Rectangle bounds, Insets insets) {
		rect = bounds.getCopy();
		int bottom = rect.bottom();
		rect.height = insets.bottom;
		rect.y = bottom - rect.height;
		rect.x += insets.left;
		rect.width -= insets.getWidth();
	}
	
	public void draw(GC gc) {
		org.eclipse.swt.graphics.Rectangle oldClip = gc.getClipping(); // graphics.popState() doesn't restore it!
		Graphics graphics = new SWTGraphics(gc);
		graphics.pushState();

		graphics.setClip(rect);
		
		graphics.setLineStyle(SWT.LINE_SOLID);
		graphics.setLineWidth(1);
		graphics.setForegroundColor(ColorFactory.BLACK);


		// draw labels
		IScalarDataset dataset = chart.getDataset();
		BarPlot plot = chart.getPlot();
		Rectangle plotRect = plot.getRectangle();
		if (dataset != null) {
			int cColumns = dataset.getColumnCount();
			graphics.setFont(labelsFont);
			graphics.drawText("", 0, 0); // force Graphics push the font setting into GC
			graphics.pushState();
			for (int row = 0; row < dataset.getRowCount(); ++row) {
				int left = plot.getBarRectangle(row, 0).x;
				int right = plot.getBarRectangle(row, cColumns - 1).right();

				graphics.restoreState();
				graphics.drawLine(left, rect.y + gap, right, rect.y + gap);
				
				GroupLabelLayoutData data = layoutData[row];
				//String label = data.label;
				Dimension size = data.size;
				Dimension rotatedSize = data.rotatedSize;
				
				graphics.translate((left + right) / 2 - rotatedSize.width / 2, rect.y + gap + 1 + size.height/2);
				graphics.rotate((float)rotation);
				graphics.drawTextLayout(data.textLayout, 0, -size.height/2);
				//graphics.drawText(label, 0, -size.height/2);
			}
			graphics.popState();
		}
		
		// draw axis title
		graphics.setFont(titleFont);
		graphics.drawText("", 0, 0); // force Graphics push the font setting into GC
		Point size = gc.textExtent(title);
		graphics.drawText(title, plotRect.x + (plotRect.width - size.x) / 2, rect.bottom() - size.y - 1);

		graphics.popState();
		graphics.dispose();
		gc.setClipping(oldClip); // graphics.popState() doesn't restore it!
	}
}