/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.runtimeenv.animation.widgets;

import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LightweightSystem;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.figures.CompoundModuleFigure;

public class AnimationCanvas extends Canvas {
	protected IFigure contents;
	protected LightweightSystem lws;

	public AnimationCanvas(Composite parent, int style) {
		super(parent, style);

		init();
	}

	protected void init() {
		XYLayout contentsLayout = new XYLayout();

		contents = new Figure();
		contents.setLayoutManager(contentsLayout);

		lws = new LightweightSystem(this);
		lws.setContents(contents);
	}
	
	public IFigure getRootFigure() {
		return contents;
	}

	public Point getPreferredSize() {
		Dimension dimension = ((CompoundModuleFigure)contents.getChildren().get(0)).getPreferredSize();
		return new Point(dimension.width, dimension.height);
	}
}
