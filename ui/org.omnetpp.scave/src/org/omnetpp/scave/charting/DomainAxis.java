package org.omnetpp.scave.charting;

import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_AXIS_TITLE_FONT;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_LABELS_FONT;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_WRAP_LABELS;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_X_LABELS_ROTATED_BY;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Vector;

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
	Vector<LineData> lines = new Vector<LineData>();
	String title = DEFAULT_X_AXIS_TITLE;
	Font titleFont = DEFAULT_AXIS_TITLE_FONT;
	Font labelsFont = DEFAULT_LABELS_FONT;
	double rotation = DEFAULT_X_LABELS_ROTATED_BY;
	boolean wrapLabels = DEFAULT_WRAP_LABELS;
	int gap = 4;  // between chart and axis 
	
	static class LabelData {
		TextLayout textLayout;
		int row;
		int centerY;
		Dimension size;
		Dimension rotatedSize;
		
		public void dispose() {
			if (textLayout != null)
				textLayout.dispose();
		}
	}
	
	static class LineData implements Comparable<LineData> {
		int height;
		int right;
		List<LabelData> labels = new ArrayList<LabelData>();
		
		public LineData() {
			height = 0;
			right = Integer.MIN_VALUE;
		}
		
		public int compareTo(LineData o) {
			return this.right == o.right ? 0 :
					this.right < o.right ? -1 : 1;
		}
	}
	
	public DomainAxis(ScalarChart chart) {
		this.chart = chart;
	}
	
	public void dispose() {
		for (LineData line : lines)
			for (LabelData label : line.labels)
				if (label != null)
					label.dispose();
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
		lines.add(new LineData());
		IScalarDataset dataset = chart.getDataset();
		BarPlot plot = chart.getPlot();
		Dimension maxSize = new Dimension(-1, Math.max(rect.height / 2, 10));
		if (dataset != null) {
			// dispose previous TextLayouts
			for (LineData line : lines)
				for (LabelData label : line.labels)
					label.dispose();
			lines.clear();
			lines.add(new LineData());

			int cColumns = dataset.getColumnCount();
			int cRows = dataset.getRowCount();
			for (int row = 0; row < cRows; ++row) {
				int left = plot.getBarRectangle(row, 0).x;
				int right = plot.getBarRectangle(row, cColumns - 1).right();
				
				LineData line = Collections.min(lines);
				LabelData label;
				
				if (wrapLabels) {
					maxSize.width = rotation == 0.0 ? Math.max(right - left, 10) : -1;
					label = layoutGroupLabel(row, labelsFont, rotation, gc, maxSize);
				}
				else {
					label = layoutGroupLabel(row, labelsFont, rotation, gc, maxSize);
					
					if ((left + right) / 2 - label.rotatedSize.width / 2 < line.right) {
						line = new LineData();
						lines.add(line);
					}
				}
				
				line.labels.add(label);
				line.height = Math.max(line.height, label.rotatedSize.height);
				line.right = (left + right) / 2 + label.rotatedSize.width / 2;
			}
		}
		
		// position labels vertically
		int labelsHeight = 0;
		for (LineData line : lines) {
			for (LabelData label : line.labels)
				label.centerY = labelsHeight + line.height / 2;
			labelsHeight += line.height;
		}
		labelsHeight = Math.min(labelsHeight, maxSize.height);

		// modify insets with space required
		insets.top = Math.max(insets.top, 10); // leave a few pixels at the top
		insets.bottom = Math.max(insets.bottom, gap + labelsHeight + titleHeight + 8);
		
		return insets;
	}
	
	private LabelData layoutGroupLabel(int row, Font font, double rotation , GC gc, Dimension maxSize) {
		LabelData data = new LabelData();
		String label = chart.getDataset().getRowKey(row);
		data.row = row;
		data.textLayout = new TextLayout(gc.getDevice());
		data.textLayout.setText(label.replace(';', '\u00ad')); // hyphenate at ';'
		data.textLayout.setFont(font);
		data.textLayout.setAlignment(SWT.CENTER);
		if (maxSize.width > 0)
			data.textLayout.setWidth(maxSize.width);
		//System.out.format("width=%s%n", maxWidth);
		org.eclipse.swt.graphics.Rectangle bounds = data.textLayout.getBounds(); 
		data.size = new Dimension(bounds.width, Math.min(bounds.height, maxSize.height)); //new Dimension(gc.textExtent(data.label));
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
			int cRows = dataset.getRowCount();
			
			// draw lines
			for (int row = 0; row < cRows; ++row) {
				int left = plot.getBarRectangle(row, 0).x;
				int right = plot.getBarRectangle(row, cColumns - 1).right();
				graphics.drawLine(left, rect.y + gap, right, rect.y + gap);
			}
			
			// draw labels
			graphics.setFont(labelsFont);
			graphics.drawText("", 0, 0); // force Graphics push the font setting into GC
			graphics.pushState(); // for restoring the transform
			for (LineData line : lines) {
				for (LabelData label : line.labels) {
					graphics.restoreState();
					int left = plot.getBarRectangle(label.row, 0).x;
					int right = plot.getBarRectangle(label.row, cColumns - 1).right();
					Dimension size = label.size;
					Dimension rotatedSize = label.rotatedSize;
					
					if (isRectangularAngle(rotation)) // center into the cell
						graphics.translate((left + right) / 2, rect.y + gap + 1 + label.centerY);
					else // left at the bottom of the bar
						graphics.translate((left + right) / 2 + rotatedSize.width / 2, rect.y + gap + 1 + label.centerY);
					graphics.rotate((float)rotation);
					graphics.drawTextLayout(label.textLayout, - size.width / 2, - size.height/2);
				}
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
	
	private static boolean isRectangularAngle(double degree) {
		return Math.abs(Math.IEEEremainder(degree, 90.0)) < 10.0;
	}
}