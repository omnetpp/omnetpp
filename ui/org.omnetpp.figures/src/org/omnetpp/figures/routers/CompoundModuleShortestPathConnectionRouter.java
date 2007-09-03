package org.omnetpp.figures.routers;

import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.draw2d.AbstractRouter;
import org.eclipse.draw2d.Bendpoint;
import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.ConnectionRouter;
import org.eclipse.draw2d.FigureListener;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LayoutListener;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PointList;
import org.eclipse.draw2d.geometry.PrecisionPoint;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.draw2d.graph.Path;
import org.eclipse.draw2d.graph.ShortestPathRouter;

import org.omnetpp.figures.anchors.CompoundModuleGateAnchor;


/**
 * Copied the ShortestPathConnectionRouter. Modified to handle CompoundModuleGateAnchors specially.
 *
 * @author rhornig
 */
public class CompoundModuleShortestPathConnectionRouter extends AbstractRouter
{

	private class LayoutTracker extends LayoutListener.Stub {
		public void postLayout(IFigure container) {
			processLayout();
		}
		public void remove(IFigure child) {
			removeChild(child);
		}
		public void setConstraint(IFigure child, Object constraint) {
			addChild(child);
		}
	}

	private Map<Connection,Object> constraintMap = new HashMap<Connection,Object>();
	private Map<IFigure,Rectangle> figuresToBounds;
	private Map<Connection,Path> connectionToPaths;
	private boolean isDirty;
	private ShortestPathRouter algorithm = new ShortestPathRouter();
	private IFigure container;
	private Set<Connection> staleConnections = new HashSet<Connection>();
	private LayoutListener listener = new LayoutTracker();

	private FigureListener figureListener = new FigureListener() {
		public void figureMoved(IFigure source) {
			Rectangle newBounds = source.getBounds().getCopy();
			if (algorithm.updateObstacle(figuresToBounds.get(source), newBounds)) {
				queueSomeRouting();
				isDirty = true;
			}

			figuresToBounds.put(source, newBounds);
		}
	};
	private boolean ignoreInvalidate;

	/**
	 * Creates a new shortest path router with the given container. The container
	 * contains all the figure's which will be treated as obstacles for the connections to
	 * avoid. Any time a child of the container moves, one or more connections will be
	 * revalidated to process the new obstacle locations. The connections being routed must
	 * not be contained within the container.
	 *
	 * @param container the container
	 */
	public CompoundModuleShortestPathConnectionRouter(IFigure container) {
		isDirty = false;
		algorithm = new ShortestPathRouter();
		this.container = container;
	}

	void addChild(IFigure child) {
		if (connectionToPaths == null)
			return;
		if (figuresToBounds.containsKey(child))
			return;
		Rectangle bounds = child.getBounds().getCopy();
		algorithm.addObstacle(bounds);
		figuresToBounds.put(child, bounds);
		child.addFigureListener(figureListener);
		isDirty = true;
	}

	private void hookAll() {
		figuresToBounds = new HashMap<IFigure,Rectangle>();
		for (int i = 0; i < container.getChildren().size(); i++)
			addChild((IFigure)container.getChildren().get(i));
		container.addLayoutListener(listener);
	}

	private void unhookAll() {
		container.removeLayoutListener(listener);
		if (figuresToBounds != null) {
			Iterator<IFigure> figureItr = figuresToBounds.keySet().iterator();
			while (figureItr.hasNext()) {
				//Must use iterator's remove to avoid concurrent modification
				IFigure child = (IFigure)figureItr.next();
				figureItr.remove();
				removeChild(child);
			}
			figuresToBounds = null;
		}
	}

	/**
	 * Returns the constraint for the given Connection. The constraint is the paths
	 * list of bend points for this connection.
	 */
	public Object getConstraint(Connection connection) {
		return constraintMap.get(connection);
	}

	/**
	 * Returns the default spacing maintained on either side of a connection. The default
	 * value is 4.
	 */
	public int getSpacing() {
		return algorithm.getSpacing();
	}

	/**
	 * @see ConnectionRouter#invalidate(Connection)
	 */
	public void invalidate(Connection connection) {
		if (ignoreInvalidate)
			return;
		staleConnections.add(connection);
		isDirty = true;
	}

	private void processLayout() {
		if (staleConnections.isEmpty())
			return;
		((Connection)staleConnections.iterator().next()).revalidate();
	}

	@SuppressWarnings("unchecked")
	private void processStaleConnections() {
		Iterator<Connection> iter = staleConnections.iterator();
		if (iter.hasNext() && connectionToPaths == null) {
			connectionToPaths = new HashMap<Connection,Path>();
			hookAll();
		}

		while (iter.hasNext()) {
			Connection conn = (Connection)iter.next();

			Path path = (Path)connectionToPaths.get(conn);
			if (path == null) {
				path = new Path(conn);
				connectionToPaths.put(conn, path);
				algorithm.addPath(path);
			}

			List constraint = (List)getConstraint(conn);
			if (constraint == null)
				constraint = Collections.EMPTY_LIST;

			// CHANGED to support CompoundModuleGateAnchors
			Point start = conn.getSourceAnchor().getReferencePoint().getCopy();
			// if the source anchor is a compound module anchor use the anchor location as reference point
			if (conn.getSourceAnchor() instanceof CompoundModuleGateAnchor)
				start = conn.getSourceAnchor().getLocation(conn.getTargetAnchor().getReferencePoint()).getCopy();
			Point end = conn.getTargetAnchor().getReferencePoint().getCopy();
			// if the target anchor is a compound module anchor use the anchor location as reference point
			if (conn.getTargetAnchor() instanceof CompoundModuleGateAnchor)
				end = conn.getTargetAnchor().getLocation(conn.getSourceAnchor().getReferencePoint());
			// ENDCHANGE

			container.translateToRelative(start);
			container.translateToRelative(end);

			path.setStartPoint(start);
			path.setEndPoint(end);

			if (!constraint.isEmpty()) {
				PointList bends = new PointList(constraint.size());
				for (int i = 0; i < constraint.size(); i++) {
					Bendpoint bp = (Bendpoint)constraint.get(i);
					bends.addPoint(bp.getLocation());
				}
				path.setBendPoints(bends);
			} else
				path.setBendPoints(null);

			isDirty |= path.isDirty;
		}
		staleConnections.clear();
	}

	void queueSomeRouting() {
		if (connectionToPaths == null || connectionToPaths.isEmpty())
			return;
		try {
			ignoreInvalidate = true;
			((Connection)connectionToPaths.keySet().iterator().next())
			.revalidate();
		} finally {
			ignoreInvalidate = false;
		}
	}

	/**
	 * @see ConnectionRouter#remove(Connection)
	 */
	public void remove(Connection connection) {
		staleConnections.remove(connection);
		constraintMap.remove(connection);
		if (connectionToPaths == null)
			return;
		Path path = (Path)connectionToPaths.remove(connection);
		algorithm.removePath(path);
		isDirty = true;
		if (connectionToPaths.isEmpty()) {
			unhookAll();
			connectionToPaths = null;
		}
		else {
			//Make sure one of the remaining is revalidated so that we can re-route again.
			queueSomeRouting();
		}
	}

	void removeChild(IFigure child) {
		if (connectionToPaths == null)
			return;
		Rectangle bounds = child.getBounds().getCopy();
		boolean change = algorithm.removeObstacle(bounds);
		figuresToBounds.remove(child);
		child.removeFigureListener(figureListener);
		if (change) {
			isDirty = true;
			queueSomeRouting();
		}
	}

	/**
	 * @see ConnectionRouter#route(Connection)
	 */
	@SuppressWarnings("unchecked")
	public void route(Connection conn) {
		if (isDirty) {
			ignoreInvalidate = true;
			processStaleConnections();
			isDirty = false;
			List updated = algorithm.solve();
			Connection current;
			for (int i = 0; i < updated.size(); i++) {
				Path path = (Path) updated.get(i);
				current = (Connection)path.data;
				current.revalidate();

				PointList points = path.getPoints().getCopy();
				Point ref1, ref2, start, end;
				ref1 = new PrecisionPoint(points.getPoint(1));
				ref2 = new PrecisionPoint(points.getPoint(points.size() - 2));
				current.translateToAbsolute(ref1);
				current.translateToAbsolute(ref2);

				start = current.getSourceAnchor().getLocation(ref1).getCopy();
				end = current.getTargetAnchor().getLocation(ref2).getCopy();

				current.translateToRelative(start);
				current.translateToRelative(end);
				points.setPoint(start, 0);
				points.setPoint(end, points.size() - 1);

				current.setPoints(points);
			}
			ignoreInvalidate = false;
		}
	}

	/**
	 * @see ConnectionRouter#setConstraint(Connection, Object)
	 */
	public void setConstraint(Connection connection, Object constraint) {
//		Connection.setConstraint() already calls revalidate, so we know that a
//		route() call will follow.
		staleConnections.add(connection);
		constraintMap.put(connection, constraint);
		isDirty = true;
	}

	/**
	 * Sets the default space that should be maintained on either side of a connection. This
	 * causes the connections to be separated from each other and from the obstacles. The
	 * default value is 4.
	 *
	 * @param spacing the connection spacing
	 * @since 3.2
	 */
	public void setSpacing(int spacing) {
		algorithm.setSpacing(spacing);
	}

}
