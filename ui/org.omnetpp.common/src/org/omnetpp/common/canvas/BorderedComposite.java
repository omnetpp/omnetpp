/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.canvas;

import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;

/**
 * A composite with a border drawn around it.
 *
 * @author Andras
 */
public class BorderedComposite extends Composite {
    protected static final Color BORDER_BACKGROUND_COLOR = new Color(null, 228, 228, 228);
    protected static final Color BORDER_FOREGROUND_COLOR = new Color(null, 128, 128, 128);

    private int borderSize = 4;

    public BorderedComposite(Composite parent, int style) {
        super(parent, style);
        FillLayout fillLayout = new FillLayout();
        fillLayout.marginWidth = borderSize;
        fillLayout.marginHeight = borderSize;
        setLayout(fillLayout);
        this.addPaintListener(new PaintListener() {
            public void paintControl(PaintEvent e) {
                int halfBorderSize = borderSize / 2;
                Point size = getSize();
                GC gc = e.gc;
                gc.setLineWidth(borderSize);
                gc.setForeground(BORDER_BACKGROUND_COLOR);
                gc.drawRectangle(halfBorderSize, halfBorderSize, size.x - borderSize, size.y - borderSize);
                gc.setForeground(BORDER_FOREGROUND_COLOR);
                gc.setLineWidth(1);
                gc.drawRoundRectangle(1, 1, size.x - 2, size.y - 2, borderSize * 2, borderSize *2);
            }
        });
    }

    public int getBorderSize() {
        return borderSize;
    }

    public Control getChild() {
        return getChildren()[0];
    }
}
