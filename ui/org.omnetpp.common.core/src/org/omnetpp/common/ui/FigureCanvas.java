/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LayoutManager;
import org.eclipse.draw2d.LightweightSystem;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;

//XXX was: AnimationCanvas in org.omnetpp.experimental.animation
public class FigureCanvas extends Canvas {
	protected IFigure contents;
	protected LightweightSystem lws;

	public FigureCanvas(Composite parent, int style) {
		super(parent, style);

		init();
	}

	protected void init() {
		LayoutManager contentsLayout = new XYLayout();
	    //LayoutManager contentsLayout = new ToolbarLayout();

		contents = new Figure();
		contents.setLayoutManager(contentsLayout);

		lws = new LightweightSystem(this);
		lws.setContents(contents);
	}
	
	public IFigure getRootFigure() {
		return contents;
	}

}
