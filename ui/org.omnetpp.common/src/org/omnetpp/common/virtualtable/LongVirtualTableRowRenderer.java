/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.virtualtable;

import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.viewers.StyledString;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.StyleRange;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.TextLayout;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.color.ColorFactory;

/**
 * This class serves debugging purposes.
 */
public class LongVirtualTableRowRenderer implements IVirtualTableRowRenderer<Long> {
    protected Font font = JFaceResources.getDefaultFont();

    protected int fontHeight;

    public void setInput(Object input) {
        // void
    }

    public void drawCell(GC gc, Long element, int index, boolean isSelected) {
        if (isSelected) {
            gc.setForeground(Display.getCurrent().getSystemColor(SWT.COLOR_LIST_SELECTION_TEXT));
            gc.setBackground(Display.getCurrent().getSystemColor(SWT.COLOR_LIST_SELECTION));
        }
        else
            gc.setForeground(ColorFactory.BLACK);

        StyledString styledString = getStyledText(element, index, isSelected);
        int indent = getIndentation(element, index);
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

    public int getRowHeight(GC gc) {
        if (fontHeight == 0) {
            Font oldFont = gc.getFont();
            gc.setFont(font);
            fontHeight = gc.getFontMetrics().getHeight();
            gc.setFont(oldFont);
        }

        return fontHeight + 2;
    }

    public String getTooltipText(Long element) {
        return null;
    }

    @Override
    public int getIndentation(Long element, int index) {
        return 5;
    }

    @Override
    public Image getImage(Long element, int index) {
        return null;
    }

    @Override
    public StyledString getStyledText(Long element, int index, boolean isSelected) {
        return new StyledString("the number " + element);
    }
}