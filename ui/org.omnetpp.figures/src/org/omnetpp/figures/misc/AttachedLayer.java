package org.omnetpp.figures.misc;

import org.eclipse.draw2d.*;
import org.eclipse.draw2d.geometry.*;

/**
 * Container layer that hooks itself to its reference figure and relatively locates itself
 * to it using the provided locator. By default it has a StackLayout and is not opaque.
 *
 * @author rhornig
 */
public class AttachedLayer extends Layer implements AncestorListener {

    protected IFigure refFigure;
    protected PrecisionPoint refPoint;
    protected IFigure attachedFigure;
    protected PrecisionPoint attachedPoint;
    protected Point translation = new Point(0,0);

    public AttachedLayer(IFigure refFig, PrecisionPoint refPoint,
                           IFigure attachedFig, PrecisionPoint attachedPoint,
                           Point translation) {
        super();
        this.refFigure = refFig;
        this.refPoint = refPoint;
        this.attachedFigure = attachedFig;
        this.attachedPoint = attachedPoint;
        this.translation = (translation == null) ? new Point(0,0) : translation;
        setLayoutManager(new StackLayout());

        // add the attached fig as our child
        add(attachedFig);

        // XXX NOTE that it would be much better to register ourselves in addNotify
        // and deregister  in removeNotify, however this causes a weird bug, that prevents
        // sending a notification to the next ancestor listener. Seems that we SHOULD NOT
        // modify (add/remove) to a listener list when directly in a notification processing.
        // This MAY cause that this object is still kept in memory even after removing it from
        // its parent because the referenceFigure still keep a a reference to it in the ancestorListener list
        // general problem: is it possible to manipulate a listenerList while processing an event fired for
        // this listener?
        // FIXME should be fixed once the issue is resolved in draw2d
        // (should provide a way of firing to all listener, even if the listener list is being modified)
        refFigure.addAncestorListener(this);
    }


//    @Override
//    public void addNotify() {
//        super.addNotify();
//        refFigure.addAncestorListener(this);
//    }
//
//
//    @Override
//    public void removeNotify() {
//        refFigure.removeAncestorListener(this);
//        super.removeNotify();
//    }


    /**
     * Static factory method to attach a figure to an other. It creates a wrapper LocatableFigure around
     * the provided figure and adds it as a child. The wrapper figure is added to the parentFig as
     * a child. The two figure is attached in a way that the two reference point matches.
     * @param refFig Figure to attach to
     * @param refLoc Reference point locator on the figure attach to
     * @param attachedFig The figure to be attached
     * @param attachedLoc Reference point locator for the attached figure
     * @param parentFig A figure to be used as a parent of the newly created wrapper figure
     * @return the wrapper figure used the attach the two figures
     */
    public AttachedLayer(IFigure refFig, int refLoc,
                                         IFigure attachedFig, int attachedLoc) {
        this(refFig, locToPrecPoint(refLoc), attachedFig, locToPrecPoint(attachedLoc), null);
    }

    public AttachedLayer(IFigure refFig, int refLoc, IFigure attachedFig, int attachedLoc, int dx, int dy) {
        this(refFig, locToPrecPoint(refLoc), attachedFig, locToPrecPoint(attachedLoc), new Point(dx,dy));
    }

    public static PrecisionPoint locToPrecPoint(int location) {
        PrecisionPoint p = new PrecisionPoint();
        switch (location & PositionConstants.NORTH_SOUTH) {
        case PositionConstants.NORTH:
            p.preciseY = 0;
            break;
        case PositionConstants.SOUTH:
            p.preciseY = 1.0;
            break;
        default:
            p.preciseY = 0.5;
        }

        switch (location & PositionConstants.EAST_WEST) {
        case PositionConstants.WEST:
            p.preciseX = 0;
            break;
        case PositionConstants.EAST:
            p.preciseX = 1.0;
            break;
        default:
            p.preciseX = 0.5;
        }
        return p;
    }

    public void setRefPoints(PrecisionPoint refPoint, PrecisionPoint attachedPoint) {
        this.refPoint = refPoint;
        this.attachedPoint = attachedPoint;
    }

    public void setRefPoints(int refLoc, int attachedLoc) {
        this.refPoint = locToPrecPoint(refLoc);
        this.attachedPoint = locToPrecPoint(attachedLoc);
    }

    public void setDeltaXY(int dx, int dy) {
        this.translation = new Point(dx,dy);
    }

    public void ancestorAdded(IFigure ancestor) {
    }

    public void ancestorMoved(IFigure ancestor) {
        // revalidate ourselves if our reference figure has moved
        revalidate();
    }

    public void ancestorRemoved(IFigure ancestor) {
        // remove ourselves too from our parent if the reference figure is removed
        if (getParent() != null)
            getParent().remove(this);
    }

    /**
     * Returns the figure's reference point in absolute coordinates
     * @param figure the figure whose reference point is requested
     * @param relRefPoint The relative placement of the refPoint in the bounding rectangle
     * @return the reference point relative to the figure's upper left corner
     */
    protected Point getRefPoint(IFigure figure, PrecisionPoint relRefPoint) {
        Rectangle bounds = figure.getBounds();
        return new Point((bounds.width -1 )* relRefPoint.preciseX,
                         (bounds.height -1 ) * relRefPoint.preciseY);
    }

    // set the bounds of this figure based on the relative data and the place of the reference figure
    protected void relocate() {
          Rectangle bounds = refFigure.getBounds().getCopy();

          bounds.setSize(attachedFigure.getPreferredSize());
          // set the size, so the calculation of the reference point will be correct
          attachedFigure.setBounds(bounds);

          // translate it to the correct position
          // REVIEW: these two are not needed since the coordinate systems of ref figure and attached figure are the same
          //refFigure.translateToAbsolute(bounds);
          //attachedFigure.translateToRelative(bounds);
          bounds.translate(getRefPoint(refFigure, refPoint));
          bounds.translate(getRefPoint(attachedFigure, attachedPoint).negate());
          bounds.translate(translation);
          attachedFigure.setBounds(bounds);
          // set the container position too (no border/inset calculation)
          setBounds(bounds);
  }

    @Override
    public void validate() {
        if (isValid())
            return;
        relocate();
        super.validate();
    }
}
