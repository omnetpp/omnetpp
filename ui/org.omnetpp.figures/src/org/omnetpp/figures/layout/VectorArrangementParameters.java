/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures.layout;


/**
 * Contains the vector arrangement parameters, that is, the 3rd, 4th and 5th args
 * of the "p" tag in a submodule display string, in an organized form.
 *
 * See LAYOUT_PAR1, LAYOUT_PAR2, LAYOUT_PAR3 in IDisplayString.
 *
 * @author andras
 */
public class VectorArrangementParameters {
    public float x = Float.NaN, y = Float.NaN;  // coordinates for the "x" (exact) arrangement
    public float dx = Float.NaN, dy = Float.NaN; // spacing for the matrix, row and column layouts; radii for the ring layout
    public int n = -1; // number of icons per row in the matrix layout
    public float scale;  // scale for translating distances to pixels
}
