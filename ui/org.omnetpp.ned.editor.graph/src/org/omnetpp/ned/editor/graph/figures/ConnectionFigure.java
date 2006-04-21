package org.omnetpp.ned.editor.graph.figures;

import org.eclipse.draw2d.PolygonDecoration;
import org.eclipse.draw2d.PolylineConnection;

public class ConnectionFigure extends PolylineConnection {
    
    public void setArrowEnabled(boolean arrowEnabled) {
        PolygonDecoration arrow = null;

        if (arrowEnabled) {
            arrow = new PolygonDecoration();
            arrow.setTemplate(PolygonDecoration.TRIANGLE_TIP);
            arrow.setScale(6, 3);
            setTargetDecoration(arrow);
        }
    }

}
