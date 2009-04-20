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