package org.omnetpp.experimental.seqchart.widgets;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.graphics.PaletteData;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableItem;

/**
 * Base class for VIRTUAL Tables with some Tree capacity. 
 * 
 * IMPLEMENTATION NOTE. We use plain Table with SWT.VIRTUAL because
 * we have to be prepared for very large amounts of data (several hundred thousand
 * lines). TableViewer doesn't scale to this amount (ContentProvider has to 
 * instantiate data for all rows, and laziness doesn't help much), and 
 * seems like VIRTUAL Tree is not really virtual either, and dies on a 
 * few thousand elements already. This code is similar to TableTree,
 * but that one is not prepared to work well with SWT.VIRTUAL.
 * 
 * @author andras
 */
public abstract class VirtualTableTreeBase extends Composite {
	
	protected Table table;
	protected Image plusImage, minusImage, sizeImage;

	public VirtualTableTreeBase(Composite parent, int style) {
		super(parent, SWT.NONE);
		setLayout(new FillLayout());

		table = new Table(this, style | SWT.VIRTUAL | SWT.FULL_SELECTION);

		table.addListener(SWT.SetData, new Listener() {
			public void handleEvent(Event e) {
				TableItem item = (TableItem)e.item;
				int lineNumber = table.indexOf(item);
				configureTableItem(item, lineNumber);
			}
		});
	}

	/**
	 * Subclasses should define this to put table column data into item,
	 * by calling item.setText(int column, String text), etc.
	 */
	protected abstract void configureTableItem(TableItem item, int lineNumber);
	
	protected Table getTable() {
		return table;
	}

	/**
	 * Utility, copied from TableTree. 
	 */
	private void createImages () {
		int itemHeight = sizeImage.getBounds().height;
		// Calculate border around image. 
		// At least 9 pixels are needed to draw the image
		// Leave at least a 6 pixel border.
		int indent = Math.min(6, (itemHeight - 9) / 2);
		indent = Math.max(0, indent);
		int size = Math.max (10, itemHeight - 2 * indent); 
		size = ((size + 1) / 2) * 2; // size must be an even number
		int midpoint = indent + size / 2;
		
		Color foreground = getForeground();
		Color plusMinus = getDisplay().getSystemColor(SWT.COLOR_WIDGET_NORMAL_SHADOW);
		Color background = getBackground();
		
		/* Plus image */
		PaletteData palette = new PaletteData(new RGB[]{foreground.getRGB(), background.getRGB(), plusMinus.getRGB()});
		ImageData imageData = new ImageData(itemHeight, itemHeight, 4, palette);
		imageData.transparentPixel = 1;
		plusImage = new Image(getDisplay(), imageData);
		GC gc = new GC(plusImage);
		gc.setBackground(background);
		gc.fillRectangle(0, 0, itemHeight, itemHeight);
		gc.setForeground(plusMinus);
		gc.drawRectangle(indent, indent, size, size);
		gc.setForeground(foreground);
		gc.drawLine(midpoint, indent + 2, midpoint, indent + size - 2);
		gc.drawLine(indent + 2, midpoint, indent + size - 2, midpoint);
		gc.dispose();
		
		/* Minus image */
		palette = new PaletteData(new RGB[]{foreground.getRGB(), background.getRGB(), plusMinus.getRGB()});
		imageData = new ImageData(itemHeight, itemHeight, 4, palette);
		imageData.transparentPixel = 1;
		minusImage = new Image(getDisplay(), imageData);
		gc = new GC(minusImage);
		gc.setBackground(background);
		gc.fillRectangle(0, 0, itemHeight, itemHeight);
		gc.setForeground(plusMinus);
		gc.drawRectangle(indent, indent, size, size);
		gc.setForeground(foreground);
		gc.drawLine(indent + 2, midpoint, indent + size - 2, midpoint);
		gc.dispose();
	}

	protected Image getPlusImage() {
		if (plusImage == null) createImages();
		return plusImage;
	}

	protected Image getMinusImage() {
		if (minusImage == null) createImages();
		return minusImage;
	}

	public void setBackground(Color color) {
		super.setBackground(color);
		table.setBackground(color);
		if (sizeImage != null) {
			GC gc = new GC (sizeImage);
			gc.setBackground(getBackground());
			Rectangle size = sizeImage.getBounds();
			gc.fillRectangle(size);
			gc.dispose();
		}
	}

	public void addSelectionListener(SelectionListener listener) {
		table.addSelectionListener(listener);
	}

	public void removeSelectionListener(SelectionListener listener) {
		table.removeSelectionListener(listener);
	}

}
