package org.omnetpp.figures;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;

/**
 * @author rhornig
 * Anchor where the reference point can be anything inside the figure and the ancor is placed
 * on the bounding box, or it can be pinned down to any inside point.
 */
public class PinnableNoncentralChopboxAnchor extends NoncentralChopboxAnchor {
    // fixed location if the anchor is pinned (relative in owner area ie 0.0 - 1.0)
    private double pinnedLocationX = 0.5f;
    private double pinnedLocationY = 0.5f;
    
    // is the anchor pinned down? so it cannot move automatically
    private boolean pinnedDown = false;

    /**
     * Create an anchor in unpinned state (default loc is the middle of the owner once it's 
     * pinned down.
     * @param owner
     */
    public PinnableNoncentralChopboxAnchor(IFigure owner) {
        super(owner);
    }


    /**
     * Create an anchor whith given location in pinned down state.
     * @param owner
     * @param pinnedLocationX
     * @param pinnedLocationY
     */
    public PinnableNoncentralChopboxAnchor(IFigure owner, double pinnedLocationX, double pinnedLocationY) {
        super(owner);
        this.pinnedLocationX = pinnedLocationX;
        this.pinnedLocationY = pinnedLocationY;
        this.pinnedDown = true;
    }


    @Override
    public Point getLocation(Point foreignRef) {
        if (!isPinnedDown()) return super.getLocation(foreignRef);
        // if we are pinned, return the current fixed reference point as the location
        return getReferencePoint();
    }

    @Override
    public Point getReferencePoint() {
        if (!isPinnedDown()) return super.getReferencePoint();
        
        Rectangle r = new Rectangle(getBox());
        r.translate(-1, -1);
        r.resize(1, 1);
        getOwner().translateToAbsolute(r);
        double oX = r.x + pinnedLocationX * r.width;
        double oY = r.y + pinnedLocationY * r.height;
        return new Point(Math.round(oX), Math.round(oY));
    }
    
    
    public boolean isPinnedDown() {
        return pinnedDown;
    }
    

    public void setPinnedDown(boolean pinnedDown) {
        if (this.pinnedDown != pinnedDown) {
            this.pinnedDown = pinnedDown;
            // the pinning status has changed, so the anchor point might have moved
            fireAnchorMoved();
        }
    }

    public double getPinnedLocationX() {
        return pinnedLocationX;
    }
    

    public void setPinnedLocationX(double pinnedLocationX) {
        this.pinnedLocationX = pinnedLocationX;
        fireAnchorMoved();
    }
    

    public double getPinnedLocationY() {
        return pinnedLocationY;
    }
    

    public void setPinnedLocationY(double pinnedLocationY) {
        this.pinnedLocationY = pinnedLocationY;
        fireAnchorMoved();
    }
    
    public void setPinnedLocation(double pinnedLocationX, double pinnedLocationY) {
        this.pinnedLocationX = pinnedLocationX;
        this.pinnedLocationY = pinnedLocationY;
        fireAnchorMoved();
    }
}
