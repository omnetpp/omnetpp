package org.omnetpp.ned.editor.graph.figures;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.PolygonDecoration;
import org.eclipse.draw2d.PolylineConnection;
import org.eclipse.draw2d.RoutingAnimator;
import org.omnetpp.ned.editor.graph.model.ConnectionNodeEx;

public class FigureFactory {

public static PolylineConnection createNewWire(ConnectionNodeEx wire) {

		PolylineConnection conn = new PolylineConnection();
		conn.addRoutingListener(RoutingAnimator.getDefault());
//		PolygonDecoration arrow;
//
//		if (wire == null )
//			arrow = null;
//		else {
//			arrow = new PolygonDecoration();
//			arrow.setTemplate(PolygonDecoration.INVERTED_TRIANGLE_TIP);
//			arrow.setScale(5, 2.5);
//		}
//		conn.setSourceDecoration(arrow);

//		if (wire == null)
//			arrow = null;
//		else {
//			arrow = new PolygonDecoration();
//			arrow.setTemplate(PolygonDecoration.INVERTED_TRIANGLE_TIP);
//			arrow.setScale(5, 2.5);
//		}
//		conn.setTargetDecoration(arrow);

		return conn;
	}	

    public static IFigure createModule() {
		ModuleFigure f = new ModuleFigure();
		return f;
	}

}