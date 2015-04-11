/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.test.gui.access;

import org.eclipse.swt.graphics.Point;
import org.omnetpp.sequencechart.widgets.SequenceChart;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.MenuAccess;
import com.simulcraft.test.gui.core.UIStep;

public class SequenceChartAccess
    extends CompositeAccess
{
    public SequenceChartAccess(SequenceChart sequenceChart) {
        super(sequenceChart);
    }

    @Override
    public SequenceChart getControl() {
        return (SequenceChart)widget;
    }

    @UIStep
    public MenuAccess activateContextMenuWithMouseClick(int eventNumber) {
        activateContextMenuWithMouseClick(getEventLocation(getEventCPtr(eventNumber)));

        return new MenuAccess(getContextMenu());
    }

    @UIStep
    public void selectEventWithMouseClick(int eventNumber) {
        clickAbsolute(Access.LEFT_MOUSE_BUTTON, getControl().toDisplay(getEventLocation(getEventCPtr(eventNumber))));
    }

    protected long getEventCPtr(int eventNumber) {
        return getControl().getEventLog().getEventForEventNumber(eventNumber).getCPtr();
    }

    protected Point getEventLocation(long eventPtr) {
        SequenceChart sequenceChart = getControl();
        int x = (int)sequenceChart.getEventXViewportCoordinate(eventPtr);
        int y = sequenceChart.getEventYViewportCoordinate(eventPtr) + sequenceChart.getGutterHeight(null);
        return new Point(x, y);
    }
}
