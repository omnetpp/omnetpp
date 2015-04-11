/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LayoutListener;
import org.eclipse.draw2d.LightweightSystem;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;

/**
 * This is a simple canvas containing a lightweight system that displays figures.
 *
 * @author levy
 */
public class FigureCanvas extends Canvas {
    protected IFigure rootFigure;
    protected LightweightSystem lws;

    public FigureCanvas(Composite parent, int style) {
        super(parent, style);
        rootFigure = new Figure() {
            @Override
            public void paint(org.eclipse.draw2d.Graphics graphics) {
                graphics.setAntialias(SWT.ON);
                super.paint(graphics);
            };
        };
        rootFigure.setLayoutManager(new XYLayout());
        rootFigure.addLayoutListener(new LayoutListener.Stub() {
            @Override
            public void postLayout(IFigure container) {
                Dimension size = rootFigure.getPreferredSize();
                setSize(size.width, size.height);
            }
        });

        lws = new LightweightSystem(this);
        lws.setContents(rootFigure);
    }

    public IFigure getRootFigure() {
        return rootFigure;
    }
}
