/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.resource.JFaceResources;
import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.controller.AnimationPosition;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.figures.anchors.NoncentralChopboxAnchor;

public class ModuleMethodCallAnimation extends AbstractInstantaneousAnimation {
    protected Label methodName;

    protected int fromModuleId;

	protected int toModuleId;

    protected ConnectionFigure methodCallArrow;

	public ModuleMethodCallAnimation(AnimationController animationController, long eventNumber, BigDecimal simulationTime, String methodNameString, int fromModuleId, int toModuleId) {
		super(animationController, eventNumber, simulationTime);
		this.fromModuleId = fromModuleId;
        this.toModuleId = toModuleId;
		methodName = new Label(methodNameString);
        methodName.setFont(JFaceResources.getDialogFont());
        methodName.setForegroundColor(ColorFactory.RED);
        methodCallArrow = new ConnectionFigure();
        methodCallArrow.setArrowHeadEnabled(true);
        methodCallArrow.setForegroundColor(ColorFactory.RED);
        methodCallArrow.setLineDash(new float[] {4, 4});
	}

	@Override
	public void activate() {
	    super.activate();
        Layer decorationLayer = getDecorationLayer();
        decorationLayer.add(methodName);
        SubmoduleFigure fromModuleFigure = getSubmoduleFigure(fromModuleId);
        SubmoduleFigure toModuleFigure = getSubmoduleFigure(toModuleId);
        if (fromModuleFigure != null && toModuleFigure != null) {
            methodCallArrow.setSourceAnchor(new NoncentralChopboxAnchor(fromModuleFigure));
            methodCallArrow.setTargetAnchor(new NoncentralChopboxAnchor(toModuleFigure));
            decorationLayer.add(methodCallArrow);
        }
	}

	@Override
	public void deactivate() {
        super.deactivate();
        Layer decorationLayer = getDecorationLayer();
        decorationLayer.remove(methodName);
        if (methodCallArrow.getParent() != null)
            decorationLayer.remove(methodCallArrow);
	}

	@Override
	public void refreshAnimation(AnimationPosition animtionPosition) {
	    Point location;
	    if (methodCallArrow.getParent() != null) {
	        methodCallArrow.validate();
	        location = methodCallArrow.getBounds().getCenter();
	    }
	    else
	        location = getSubmoduleFigure(toModuleId).getBounds().getLocation().translate(0, -20);
        methodName.setBounds(new Rectangle(location, methodName.getPreferredSize().getCopy().expand(10, 0)));
	}

    protected Layer getDecorationLayer() {
        return getEnclosingCompoundModuleFigure(toModuleId).getForegroundDecorationLayer();
    }
}
