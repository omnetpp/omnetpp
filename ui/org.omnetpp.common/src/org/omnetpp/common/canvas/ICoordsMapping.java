/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

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
	 * The largest coordinate the underlying platform graphics can handle.
	 * This is supposed to be the full 32-bit signed range, but e.g. Windows XP
	 * has been seen to overflow with coords > ~2 million. (Try zooming a chart.)
	 */
	public static final int MAXPIX = 2000000;  //XXX until we have more info

	/**
	 * We use this to return NaN as a pixel coordinate.
	 */
	public static final int NAN_PIX = Integer.MAX_VALUE;

	public double fromCanvasX(int x);

	public double fromCanvasY(int y);

	public double fromCanvasDistX(int x);

	public double fromCanvasDistY(int y);

	/**
	 * Returns canvas coordinate. If result is out of the range GC can
	 * handle, MAXPIX is returned AND an overflow counter is incremented;
	 * for +-INF, +-MAXPIX is returned; for NaN, NANPIX is returned.
	 */
	public int toCanvasX(double xCoord);

	/** See toCanvasX() documentation */
	public int toCanvasY(double yCoord);

	/** See toCanvasX() documentation */
	public int toCanvasDistX(double xCoord);

	/** See toCanvasX() documentation */
	public int toCanvasDistY(double yCoord);

	public int getNumCoordinateOverflows();

	public void resetCoordinateOverflowCount();
}

