/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.simulation.liveanimation;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.ImageFigure;
import org.eclipse.draw2d.geometry.Point;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;

public class DummyMoveMessageAnimation extends AnimationPrimitive {
    private final double SPEED = 50;  // pixels per second
    private final int FRAMERATE = 50;  //XXX
    private IFigure figure;
    private Point beginLocation;
    private Point endLocation;
    
    private int frameNo, numFrames;
    private double dx, dy;

	public DummyMoveMessageAnimation(IFigure parent, Point beginLocation, Point endLocation) {
	    this.beginLocation = beginLocation;
	    this.endLocation = endLocation;
	    
	    double distance = endLocation.getDistance(beginLocation);
	    double time = distance/SPEED;
	    numFrames = (int) Math.ceil(time * FRAMERATE);
	    frameNo = 0;

	    figure = new ImageFigure(SimulationPlugin.getCachedImage(SimulationUIConstants.IMG_OBJ_MESSAGE));
	    //figure.setSize(figure.getPreferredSize()); TODO
	    parent.add(figure);
	}

	@Override
	public boolean update() {
	    int x = beginLocation.x + frameNo*(endLocation.x - beginLocation.x)/numFrames;
	    int y = beginLocation.y + frameNo*(endLocation.y - beginLocation.y)/numFrames;
	    Point location = new Point(x - figure.getSize().width/2, y - figure.getSize().height/2);
	    System.out.print(" " + location.x + "," + location.y + " "); //XXX
	    
	    //figure.getParent().getLayoutManager().setConstraint(figure, new Rectangle(location, new Dimension(-1, -1)));
	    figure.setLocation(location);
	    figure.setSize(10,10);
	    figure.getParent().revalidate();
	    
	    if (++frameNo == numFrames) {
	        figure.getParent().remove(figure);
	        return false;
	    }
	    else {
	        return true;
	    }
	    
	    
//	    double alpha;
//        if (getSimulationTimeDuration().doubleValue() != 0)
//            alpha = animationController.getCurrentSimulationTime().subtract(getBeginSimulationTime()).doubleValue() / getSimulationTimeDuration().doubleValue();
//        else
//            alpha = (animationPosition.getOriginRelativeAnimationTime() - getOriginRelativeBeginAnimationTime()) / getAnimationTimeDuration();
//        alpha = Math.min(1.0, Math.max(0.0, alpha));
//        IFigure figure = getFigure(message, PassengerFigure.class);
//        Point location = beginLocation.getScaled(1 - alpha).translate(endLocation.getScaled(alpha));
//        figure.getParent().getLayoutManager().setConstraint(figure, new Rectangle(location, new Dimension(-1, -1)));
//        figure.getParent().revalidate();
	}
}
