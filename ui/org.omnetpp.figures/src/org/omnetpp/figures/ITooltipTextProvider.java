/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/
package org.omnetpp.figures;

/**
 * Tooltip for figures.
 * @author Andras
 */
public interface ITooltipTextProvider {
    /**
     * Receives canvas coordinates.
     */
    String getTooltipText(int x, int y);
}