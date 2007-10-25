package org.omnetpp.test.gui.access;

import org.eclipse.swt.graphics.Point;
import org.omnetpp.sequencechart.widgets.SequenceChart;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.MenuAccess;
import com.simulcraft.test.gui.core.InUIThread;

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

    @InUIThread
    public MenuAccess activateContextMenuWithMouseClick(int eventNumber) {
        activateContextMenuWithMouseClick(getEventLocation(getEventCPtr(eventNumber)));

        return new MenuAccess(getContextMenu());
    }

    @InUIThread
    public void selectEventWithMouseClick(int eventNumber) {
        clickAbsolute(Access.LEFT_MOUSE_BUTTON, getControl().toDisplay(getEventLocation(getEventCPtr(eventNumber))));
    }

    protected long getEventCPtr(int eventNumber) {
        return getControl().getEventLog().getEventForEventNumber(eventNumber).getCPtr();
    }

    protected Point getEventLocation(long eventPtr) {
        SequenceChart sequenceChart = getControl();
        int x = sequenceChart.getEventXViewportCoordinate(eventPtr);
        int y = sequenceChart.getEventYViewportCoordinate(eventPtr) + SequenceChart.GUTTER_HEIGHT;
        return new Point(x, y);
    }
}
