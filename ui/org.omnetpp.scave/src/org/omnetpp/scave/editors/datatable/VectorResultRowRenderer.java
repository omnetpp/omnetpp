/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.datatable;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.StyledString;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.StyleRange;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.TextLayout;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.virtualtable.IVirtualTableRowRenderer;
import org.omnetpp.scave.engine.EnumType;
import org.omnetpp.scave.engine.OutputVectorEntry;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.ResultItem.DataType;
import org.omnetpp.scave.model2.ResultItemRef;
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

    protected DataType type;
    protected EnumType enumType;
    protected ResultItemValueFormatter valueLabelProvider = new ResultItemValueFormatter();
    protected int fontHeight;

    public void setInput(Object input) {
        if (input instanceof ResultItemRef) {
            valueLabelProvider.setResultItem((ResultItemRef)input);
        }
        else {
            valueLabelProvider.setResultItem((ResultItem)null);
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

    public void drawCell(GC gc, OutputVectorEntry entry, int index, boolean isSelected) {
        if (isSelected) {
            gc.setForeground(Display.getCurrent().getSystemColor(SWT.COLOR_LIST_SELECTION_TEXT));
            gc.setBackground(Display.getCurrent().getSystemColor(SWT.COLOR_LIST_SELECTION));
        }
        else
            gc.setForeground(DATA_COLOR);

        StyledString styledString = getStyledText(entry, index, isSelected);
        int indent = getIndentation(entry, index);
        drawStyledString(gc, styledString, indent, 0);
    }

    protected void drawStyledString(GC gc, StyledString styledString, int x, int y) {
        TextLayout textLayout = new TextLayout(gc.getDevice());
        textLayout.setText(styledString.getString());
        for (StyleRange styleRange : styledString.getStyleRanges()) {
            textLayout.setStyle(styleRange, styleRange.start, styleRange.start + styleRange.length);
        }
        textLayout.draw(gc, x, y);
        textLayout.dispose();
    }

    public String getTooltipText(OutputVectorEntry element) {
        return null;
    }

    @Override
    public int getIndentation(OutputVectorEntry element, int index) {
        return HORIZONTAL_SPACING;
    }

    @Override
    public Image getImage(OutputVectorEntry element, int index) {
        return null;
    }

    @Override
    public StyledString getStyledText(OutputVectorEntry element, int index, boolean isSelected) {
        String text = null;
        switch (index) {
        case 0:
            text = String.valueOf(element.getSerial());
            break;
        case 1:
            BigDecimal time = element.getSimtime();
            text = (time != null ? time.toString() : "");
            break;
        case 2:
            text = valueLabelProvider.format(element.getValue());
            break;
        case 3:
            text = String.valueOf(element.getEventNumber());
            break;
        default:
            Assert.isTrue(false);
        }
        return new StyledString(text);
    }
}
