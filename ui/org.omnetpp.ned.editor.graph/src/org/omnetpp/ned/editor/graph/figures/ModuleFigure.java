package org.omnetpp.ned.editor.graph.figures;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

import org.eclipse.draw2d.*;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.swt.SWT;
import org.omnetpp.ned.editor.graph.figures.properties.LayerSupport;

/**
 * Supports adding / removing conections to a node. It is a generic 'Module' (anything that can have connections)
 * @author rhornig
 *
 */
public class ModuleFigure extends Figure {

    private Hashtable<String, ConnectionAnchor> connectionAnchors 
                = new Hashtable<String, ConnectionAnchor>(7);
    private Vector<ConnectionAnchor> inputConnectionAnchors 
                = new Vector<ConnectionAnchor>(2, 2);
    private Vector<ConnectionAnchor> outputConnectionAnchors 
                = new Vector<ConnectionAnchor>(2, 2);

    /**
     * Returns a layer from any ancestor that supports multiple layers for decorations
     * @param id Layer id
     * @return The layer with teh given id from any ancestor that inpmelements tha LayerSupport IF
     */
    // TODO implement in a way, that if a layer is not found, it should search furter in the ancestor list
    // and return null only if eached the root figure
    public Layer getAncestorLayer(Object id) {
        IFigure figureIter = getParent();
        // look for a parent who is an instance of LayerSupport and get the layer from it
        while (!(figureIter == null || (figureIter instanceof LayerSupport)))
            figureIter = figureIter.getParent();
        if(figureIter instanceof LayerSupport) return ((LayerSupport)figureIter).getLayer(id); 
        return null;
    }
    
    public ConnectionAnchor connectionAnchorAt(Point p) {
        ConnectionAnchor closest = null;
        long min = Long.MAX_VALUE;

        Enumeration e = getSourceConnectionAnchors().elements();
        while (e.hasMoreElements()) {
            ConnectionAnchor c = (ConnectionAnchor) e.nextElement();
            Point p2 = c.getLocation(null);
            long d = p.getDistance2(p2);
            if (d < min) {
                min = d;
                closest = c;
            }
        }
        e = getTargetConnectionAnchors().elements();
        while (e.hasMoreElements()) {
            ConnectionAnchor c = (ConnectionAnchor) e.nextElement();
            Point p2 = c.getLocation(null);
            long d = p.getDistance2(p2);
            if (d < min) {
                min = d;
                closest = c;
            }
        }
        return closest;
    }

    public ConnectionAnchor getSourceConnectionAnchorAt(Point p) {
        ConnectionAnchor closest = null;
        long min = Long.MAX_VALUE;

        Enumeration e = getSourceConnectionAnchors().elements();
        while (e.hasMoreElements()) {
            ConnectionAnchor c = (ConnectionAnchor) e.nextElement();
            Point p2 = c.getLocation(null);
            long d = p.getDistance2(p2);
            if (d < min) {
                min = d;
                closest = c;
            }
        }
        return closest;
    }

    public ConnectionAnchor getTargetConnectionAnchorAt(Point p) {
        ConnectionAnchor closest = null;
        long min = Long.MAX_VALUE;

        Enumeration e = getTargetConnectionAnchors().elements();
        while (e.hasMoreElements()) {
            ConnectionAnchor c = (ConnectionAnchor) e.nextElement();
            Point p2 = c.getLocation(null);
            long d = p.getDistance2(p2);
            if (d < min) {
                min = d;
                closest = c;
            }
        }
        return closest;
    }

    public Vector getTargetConnectionAnchors() {
        return inputConnectionAnchors;
    }

    /**
     * Adds a connection anchor belongig to an input gate
     * @param ca Connection anchor to be added
     * @param gate The gate associated with this anchor
     */
    public void addTargetConnectionAnchor(ConnectionAnchor ca, String gate) {
        inputConnectionAnchors.addElement(ca);
        connectionAnchors.put(gate, ca);
    }

    public Vector getSourceConnectionAnchors() {
        return outputConnectionAnchors;
    }

    /**
     * Adds a connection anchor belongig to an output gate
     * @param ca Connection anchor to be added
     * @param gate The gate associated with this anchor
     */
    public void addSourceConnectionAnchor(ConnectionAnchor ca, String gate) {
        outputConnectionAnchors.addElement(ca);
        connectionAnchors.put(gate, ca);
    }

    /**
     * Returns the associated gate
     * @param c the connection anchor
     * @return
     */
    public String getGate(ConnectionAnchor c) {
        Enumeration keys = connectionAnchors.keys();
        String key;
        while (keys.hasMoreElements()) {
            key = (String) keys.nextElement();
            if (connectionAnchors.get(key) == c) return key;
        }
        return null;
    }

    /**
     * Returns a conn anchor registered for the given gate
     * @param gate
     * @return
     */
    public ConnectionAnchor getConnectionAnchor(String gate) {
        return connectionAnchors.get(gate);
    }
    
    /* (non-Javadoc)
     * @see org.eclipse.draw2d.IFigure#paint(org.eclipse.draw2d.Graphics)
     * Enable antialiasing for all derived figures
     */
    @Override
    public void paint(Graphics graphics) {
        graphics.setAntialias(SWT.ON);
        super.paint(graphics);
    }
}