package org.omnetpp.figures.routers;

import org.eclipse.draw2d.AbstractRouter;
import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PointList;

import org.omnetpp.figures.anchors.CompoundModuleGateAnchor;

/**
 * Very basic straight connection router. It handles a CompoundModuleGateAnchor
 * differently so reference point can be calculated correctly.
 *
 * @author rhornig
 */
public class CompoundModuleConnectionRouter extends AbstractRouter
{

	/**
	 * Constructs a new NullConnectionRouter.
	 */
	public CompoundModuleConnectionRouter() { }

	/**
	 * Routes the given Connection directly between the source and target anchors.
	 * Handles CompoundModuleGateAnchors differently, because they cannor provide a
	 * reference point without knowing the other end's reference point
	 * @param conn the connection to be routed
	 */
	public void route(Connection conn) {
		PointList points = conn.getPoints();
		points.removeAllPoints();
		Point p;
		conn.translateToRelative(p = getStartPoint(conn));
		points.addPoint(p);
		conn.translateToRelative(p = getEndPoint(conn));
		points.addPoint(p);
		conn.setPoints(points);
	}

	protected Point getEndPoint(Connection conn) {
		Point ref = conn.getSourceAnchor().getReferencePoint();
		// if the source anchor is a compound module anchor use the anchor location as reference point
		if (conn.getSourceAnchor() instanceof CompoundModuleGateAnchor)
			ref = conn.getSourceAnchor().getLocation(conn.getTargetAnchor().getReferencePoint());
		return new Point((conn.getTargetAnchor().getLocation(ref)));
	}

	protected Point getStartPoint(Connection conn) {
		Point ref = conn.getTargetAnchor().getReferencePoint();
		// if the target anchor is a compound module anchor use the anchor location as reference point
		if (conn.getTargetAnchor() instanceof CompoundModuleGateAnchor)
			ref = conn.getTargetAnchor().getLocation(conn.getSourceAnchor().getReferencePoint());

		return new Point(conn.getSourceAnchor().getLocation(ref));
	}
}


