/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.eclipse.draw2d.ImageFigure;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Rectangle;
import org.omnetpp.animation.AnimationPlugin;
import org.omnetpp.animation.editors.AnimationContributor;
import org.omnetpp.animation.widgets.AnimationController;
import org.omnetpp.animation.widgets.AnimationPosition;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.eventlog.EventLogMessage;

public class ScheduleDirectMessageAnimation extends AbstractInstantaneousAnimation {
    protected static Image image = new Image(null, AnimationPlugin.getImageDescriptor(AnimationContributor.IMAGE_SCHEDULE_MESSAGE).getImageData());

    protected int moduleId;

    protected EventLogMessage message;

    protected ImageFigure imageFigure;

    public ScheduleDirectMessageAnimation(AnimationController animationEnvironment, long eventNumber, BigDecimal simulationTime, int moduleId, EventLogMessage message) {
		super(animationEnvironment, eventNumber, simulationTime);
        this.message = message;
		this.moduleId = moduleId;
        Rectangle bounds = image.getBounds();
        imageFigure = new ImageFigure(image);
        imageFigure.setSize(bounds.width, bounds.height);
	}

	@Override
	public void activate() {
        super.activate();
	    getDecorationLayer().add(imageFigure);
	}

	@Override
	public void deactivate() {
        super.deactivate();
        getDecorationLayer().remove(imageFigure);
	}

	@Override
	public void refreshAnimation(AnimationPosition animtionPosition) {
        Point location = getSubmoduleFigure(moduleId).getBounds().getLocation();
        Dimension size = imageFigure.getSize();
        imageFigure.setLocation(location.translate(0, -size.height * 2));
	}

    protected Layer getDecorationLayer() {
        return getEnclosingCompoundModuleFigure(moduleId).getForegroundDecorationLayer();
    }
}
