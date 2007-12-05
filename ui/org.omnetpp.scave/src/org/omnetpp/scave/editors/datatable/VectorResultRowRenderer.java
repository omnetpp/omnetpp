package org.omnetpp.scave.editors.datatable;

import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.common.virtualtable.IVirtualTableRowRenderer;
import org.omnetpp.scave.engine.EnumType;
import org.omnetpp.scave.engine.OutputVectorEntry;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.engine.ResultItem.Type;
import org.omnetpp.scave.model2.ResultItemValueFormatter;

/**
 * Implementation of the IVirtualTableItemProvier interface for
 * the table of vector data.
 *
 * @author tomi
 */
public class VectorResultRowRenderer extends LabelProvider implements IVirtualTableRowRenderer<OutputVectorEntry> {

	private static final int HORIZONTAL_SPACING = 4;

	private static final Color DATA_COLOR = ColorFactory.BLACK;

	protected Font font = JFaceResources.getDefaultFont();

	protected Type type;
	protected EnumType enumType;
	protected ResultItemValueFormatter valueLabelProvider = new ResultItemValueFormatter();
	protected int fontHeight;
	
	public void setInput(Object input) {
		if (input instanceof VectorResult) {
			valueLabelProvider.setResultItem((VectorResult)input);
		}
		else {
			valueLabelProvider.setResultItem(null);
		}
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

	public void drawCell(GC gc, OutputVectorEntry entry, int index) {
		if (!gc.getForeground().equals(Display.getCurrent().getSystemColor(SWT.COLOR_LIST_SELECTION_TEXT)))
			gc.setForeground(DATA_COLOR);

		switch (index) {
			case 0:
				gc.drawText(String.valueOf(entry.getSerial()), HORIZONTAL_SPACING, 0);
				break;
			case 1:
				BigDecimal time = entry.getSimtime();
				gc.drawText((time != null ? time.toString() : ""), HORIZONTAL_SPACING, 0);
				break;
			case 2:
				gc.drawText(valueLabelProvider.format(entry.getValue()), HORIZONTAL_SPACING, 0);
				break;
			case 3:
				gc.drawText(String.valueOf(entry.getEventNumber()), HORIZONTAL_SPACING, 0);
				break;
		}
	}

	public String getTooltipText(OutputVectorEntry element, SizeConstraint outSizeConstraint) {
		return null;
	}
}
