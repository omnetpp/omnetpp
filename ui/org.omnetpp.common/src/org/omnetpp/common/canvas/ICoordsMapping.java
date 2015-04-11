/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.canvas;

/**
 * Abstracts out coordinate mapping for charts.
 *
 * @author Andras
 */
public interface ICoordsMapping {
    /**
     * We use this to return NaN as a pixel coordinate.
     */
    public static final long NAN_PIX = Long.MIN_VALUE;

    /**
     * We use +-MAX_PIX to return +-Infinite as a pixel coordinate.
     */
    public static final long MAX_PIX = Long.MAX_VALUE;

    public double fromCanvasX(long x);

    public double fromCanvasY(long y);

    public double fromCanvasDistX(long x);

    public double fromCanvasDistY(long y);

    /**
     * Returns canvas coordinate. If result is out of the range GC can
     * handle, MAXPIX is returned AND an overflow counter is incremented;
     * for +-INF, +-MAXPIX is returned; for NaN, NANPIX is returned.
     */
    public long toCanvasX(double xCoord);

    /** See toCanvasX() documentation */
    public long toCanvasY(double yCoord);

    /** See toCanvasX() documentation */
    public long toCanvasDistX(double xCoord);

    /** See toCanvasX() documentation */
    public long toCanvasDistY(double yCoord);
}

