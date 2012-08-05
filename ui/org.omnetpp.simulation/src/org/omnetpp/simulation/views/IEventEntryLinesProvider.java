package org.omnetpp.simulation.views;

import org.eclipse.swt.graphics.Color;
import org.omnetpp.simulation.controller.EventEntry;

/**
 * 
 * @author Andras
 */
public interface IEventEntryLinesProvider {
    
    int getNumLines(EventEntry entry);
    
    String getLineText(EventEntry entry, int lineIndex);
    
    Color getLineColor(EventEntry entry, int lineIndex);
}
