package org.omnetpp.common.ui;

/**
 * Label provider for the Lever widget.
 *
 * @author andras
 */
public interface ILeverLabelProvider {
    String getTextForLever(double value);
    String getTextForMark(double value);
}