/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures;

import static java.lang.Math.*;

import org.eclipse.draw2d.Polygon;
import org.eclipse.draw2d.geometry.*;

/**
 * Draws a regular N-gon. The shape is determined by a a bounding box, rotation angle and
 * the number of sides.
 *
 * @author rhornig
 */
public class RegularPolygon extends Polygon {
    private PointList templatePointList;
    private static final int TEMPLATE_RADIUS = 100000;
    // preferred angle of start point
    protected int prefAngle = 0;
    // number of sides
    protected int noOfSides = 3;
    
    /**
     * Creates a polygon with given geometry.
     * @param sides Number of sides (>2)
     */
    public RegularPolygon(int sides) {
        setGeometry(sides, 0);
    }
    
    /**
     * Creates a polygon with given geometry.
     * @param sides Number of sides (>2)
     * @param angle Rotation of figure (0-360)
     */
    public RegularPolygon(int sides,int angle) {
        setGeometry(sides, angle);
    }
    
    /**
     * Sets the geometry of the polyline
     * @param sides Number of sides (must be greater than 2 otherwise an IllegalArgumentException is thrown)
     * @param angle Rotation angle. (0-360) Note that the figure first gets rotated and then the scaling takes
     * place.
     */
    public void setGeometry(int sides, int angle) {
        if (sides < 3)
            throw new IllegalArgumentException("A polygon must have at least 3 sides");
        this.noOfSides = sides;
        this.prefAngle = angle;
        templatePointList = createPointList(sides, TEMPLATE_RADIUS, angle);
    }

    /**
     * @see org.eclipse.draw2d.IFigure#setBounds(org.eclipse.draw2d.geometry.Rectangle)
     * Sets the bounds of the figure. Note that unlike the Polyline and Polygon classes
     * setBounds can be used to set the coordinates and size of the figure. Passing negative
     * values as width or hight leaves that dimension free, allowing a regular polygon to
     * keep its aspect ratio. Setting both width and height will scretch the figure this size.
     * Note also that getBounds will return the REAL bounds of the figure. Dimensions that was
     * filled with negative values in setBounds will be correctly calculated in getBounds call.
     * If both dimension is negative, the figure will use its preferred size, or request the size
     * from the underlying layout manager.  
     */
    @Override
    public void setBounds(Rectangle rect) {
        setPoints(createScaledAndTranslatedPointList(templatePointList, rect));
    }

    // TODO sclaling is still not perfect. in some linelength settings the actual shape is smaller 
    // than the calculated bounding box
    // FIXME check and test for 0 width or hight (div by zero!!!)
    protected PointList createScaledAndTranslatedPointList(PointList templList, Rectangle requestedBounds) {
        // calculate the width of the destModule boundary
        Rectangle targetBounds = requestedBounds.getCopy();
        // if both dimension is free (negative) use the preferred size
        if (targetBounds.width < 0 && targetBounds.height < 0 )
            targetBounds.setSize(getPreferredSize());
        // correct the width with the line length so thicker lines will not be clipped
        targetBounds.resize(-getLineWidth(), -getLineWidth());

        // calculate the required horizontal and verical scaling to scale the template
        // to the requested size
        Rectangle templBounds = templList.getBounds().getCopy();
        double wScale = (double)targetBounds.width / templBounds.width;
        double hScale = (double)targetBounds.height / templBounds.height;
        // if one of the requested dimensions are less than 0 
        // keep the aspect ratio and use the same scaling factor for both dimension
        if (requestedBounds.width < 0) wScale = hScale;
        if (requestedBounds.height < 0) hScale = wScale;
        // scale the bounds to the destModule size
        templBounds.scale(wScale, hScale);
        // calculate the required translation taking into account the linewidth too
        double dx = targetBounds.x - templBounds.x + (double)getLineWidth()/2;
        double dy = targetBounds.y - templBounds.y + (double)getLineWidth()/2;
     
        // scale and translate all the points like the bounds 
        PointList scaledPl = new PointList(templList.size());
        for (int i=0; i<templList.size(); ++i) {
            Point templP = templList.getPoint(i);
            scaledPl.addPoint((int)Math.floor(templP.x * wScale + dx), 
                              (int)Math.floor(templP.y * hScale + dy));
        }
        return scaledPl;
    }
    
    // create a template point list, so we have to recreate this only if the base geometry
    // has been changed (number of sides or angle)
    protected PointList createPointList(int sides, int radius, int angle) {
        PointList pl = new PointList(sides);
        
        double angleStart = toRadians(angle);
        double angleStep = 2 * PI / sides;
        for (int i=0; i<sides; ++i) {
            double currAngle = angleStart + angleStep * i;
            pl.addPoint((int)Math.round(-radius*sin(currAngle)), 
                        (int)Math.round(-radius*cos(currAngle)));
        }
        return pl;
    }

}
