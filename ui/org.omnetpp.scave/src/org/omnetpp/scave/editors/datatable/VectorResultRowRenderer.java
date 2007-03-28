package org.omnetpp.scave.editors.datatable;

import java.math.BigDecimal;

import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.virtualtable.IVirtualTableRowRenderer;
import org.omnetpp.scave.engine.OutputVectorEntry;

/**
 * Implementation of the IVirtualTableItemProvier interface for
 * the table of vector data.
 *
 * @author tomi
 */
public class VectorResultRowRenderer extends LabelProvider implements IVirtualTableRowRenderer<OutputVectorEntry> {

	private static final int HORIZONTAL_SPACING = 4;

	private static final Color DATA_COLOR = ColorFactory.asColor("black");

	protected Font font = JFaceResources.getDefaultFont();

	protected int fontHeight;
	
	public void setInput(Object input) {
	}

	public int getRowHeight(GC gc) {
		if (fontHeight == 0) {
			Font oldFont = gc.getFont();
			gc.setFont(font);
			fontHeight = gc.getFontMetrics().getHeight();
			gc.setFont(oldFont);
		}

		return fontHeight + 3;
	}

	public void drawCell(GC gc, OutputVectorEntry element, int index) {
		OutputVectorEntry entry = (OutputVectorEntry)element;

		if (!gc.getForeground().equals(Display.getCurrent().getSystemColor(SWT.COLOR_LIST_SELECTION_TEXT)))
			gc.setForeground(DATA_COLOR);

		switch (index) {
			case 0:
				gc.drawText(String.valueOf(entry.getSerial()), HORIZONTAL_SPACING, 0);
				break;
			case 1:
				BigDecimal time = entry.getSimtime();
				gc.drawText((time != null ? time.toPlainString() : ""), HORIZONTAL_SPACING, 0);
				break;
			case 2:
				gc.drawText(String.valueOf(entry.getValue()), HORIZONTAL_SPACING, 0);
				break;
			case 3:
				gc.drawText(String.valueOf(entry.getEventNumber()), HORIZONTAL_SPACING, 0);
				break;
		}
	}
}
