/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.LayoutManager;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.animation.widgets.AnimationController;
import org.omnetpp.animation.widgets.AnimationPosition;

public class TextAnimation extends AbstractAnimationPrimitive {
	protected Label label;

	protected String text;

	public TextAnimation(AnimationController animationController, String text) {
		super(animationController);
		this.text = text;
		this.label = new Label(text);
	}

	@Override
	public void activate() {
        super.activate();
		addFigure(label);
		Dimension size = new Dimension(10 * text.length(), 20);
        Rectangle r = new Rectangle(new Point(0, 0), size);
        LayoutManager layoutManager = getLayoutManager();
        layoutManager.setConstraint(label, r);
	}

	@Override
	public void deactivate() {
        super.deactivate();
        removeFigure(label);
	}

	protected void addFigure(IFigure figure) {
		getRootFigure().add(figure);
	}

	protected void removeFigure(IFigure figure) {
		getRootFigure().remove(figure);
	}

	@Override
	public void refreshAnimation(AnimationPosition animationPosition) {

	}
}
