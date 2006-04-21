/*******************************************************************************
 * Copyright (c) 2000, 2004 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Common Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/cpl-v10.html
 * 
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.omnetpp.ned.editor.graph.figures;

import org.eclipse.draw2d.*;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;

public class NoncentralChocboxAnchor extends ChopboxAnchor {

    // centrum point (relative in owner area ie 0.0 - 1.0)
    private double relRefPointX = 0.5f;
    private double relRefPointY = 0.5f;
    
    /**
     * An anchor where the connections can be directed to any particular point
     * in the owner's area (by default towards the center of the owner). 
     * @param owner Owner figue
     */
    public NoncentralChocboxAnchor(IFigure owner) {
        super(owner);
    }

    /**
     * An anchor where the connections can be directed to any particular point
     * in the owner's area.
     * @param owner Owner figure
     * @param rX Relative place in owner (0.0 - 1.0)
     * @param rY Relative place in owner (0.0 - 1.0)
     */
    public NoncentralChocboxAnchor(IFigure owner, double rX, double rY) {
        super(owner);
        setRelRefPoint(rX, rY);
    }
    /**
     * @see org.eclipse.draw2d.AbstractConnectionAnchor#ancestorMoved(IFigure)
     */
    @Override
    public void ancestorMoved(IFigure figure) {
        if (figure instanceof ScalableFigure) return;
        super.ancestorMoved(figure);
    }

    @Override
    protected Rectangle getBox() {
        // if the owner is a label use only its icon area 
        if(getOwner() instanceof Label) {
            Rectangle ib = ((Label)getOwner()).getIconBounds();
            return ib;
        }
            
        return super.getBox();
    }

    
    // TODO maybe implement a constraint that the anchor should move only on allowed sides of the box
    @Override
    public Point getLocation(Point foreignRef) {

        Rectangle r = new Rectangle(getBox());
        r.translate(-1, -1);
        r.resize(1, 1);

        // get our own reference point (where the connection should head)
        Point o = getReferencePoint();

        // if no foreign reference point is given, use our own default reference point
        // the anchor's location if it is not attached to a connection
        if (foreignRef == null) {
//            long defX = Math.round(r.x + defaultX * r.width);
//            long defY = Math.round(r.y + defaultY * r.height);
//            Point defLoc = new Point(defX, defY);
//            getOwner().translateToAbsolute(defLoc);
//            return defLoc;
            return o;
        }
        

        if (r.isEmpty() || (foreignRef.x == o.x && foreignRef.y == o.y))
            return new Point(o.x, o.y);  //This avoids divide-by-zero

        double dx = foreignRef.x - o.x;
        double dy = foreignRef.y - o.y;
        
        //r.width, r.height, dx, and dy are guaranteed to be non-zero here. 

        double scale = Math.min( Math.abs((dx > 0) ? (1.0f - relRefPointX)*r.width/dx : relRefPointX*r.width/dx),
                      Math.abs((dy > 0) ? (1.0f - relRefPointY)*r.height/dy : relRefPointY*r.height/dy));

        return new Point(Math.round(o.x + dx * scale), Math.round(o.y + dy * scale));
    }

    /**
     * Reference point is the same where the connections are heading 
     * @see org.eclipse.draw2d.ConnectionAnchor#getReferencePoint()
     */
    @Override
    public Point getReferencePoint() {
        Rectangle r = new Rectangle(getBox());
        r.translate(-1, -1);
        r.resize(1, 1);
        getOwner().translateToAbsolute(r);
        double oX = r.x + relRefPointX * r.width;
        double oY = r.y + relRefPointY * r.height;
        return new Point(Math.round(oX), Math.round(oY));
    }

    public double getRelRefPointX() {
        return relRefPointX;
    }
    
    public void setRelRefPointX(double rX) {
        this.relRefPointX = rX;
        fireAnchorMoved();
    }
    
    public double getRelRefPointY() {
        return relRefPointY;
    }
    
    public void setRelRefPointY(double rY) {
        this.relRefPointY = rY;
        fireAnchorMoved();
    }

    public void setRelRefPoint(double rX, double rY) {
        this.relRefPointX = rX;
        this.relRefPointY = rY;
        fireAnchorMoved();
    }

}