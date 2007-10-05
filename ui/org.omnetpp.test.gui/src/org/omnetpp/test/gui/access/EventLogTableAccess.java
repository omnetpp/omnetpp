package org.omnetpp.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.widgets.Composite;
import org.omnetpp.eventlogtable.widgets.EventLogTable;

import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.MenuAccess;
import com.simulcraft.test.gui.core.InUIThread;

public class EventLogTableAccess
    extends CompositeAccess
{
    public EventLogTableAccess(Composite composite) {
        super(composite);
    }
    
    @Override
    public EventLogTable getControl() {
        return (EventLogTable)widget;
    }

    @InUIThread
    public MenuAccess activateContextMenuWithMouseClick(int eventNumber) {
        EventLogTable eventLogTable = getControl();
        
        if (eventLogTable.getTopVisibleElement().getEventNumber() <= eventNumber &&
            eventNumber <= eventLogTable.getBottomFullyVisibleElement().getEventNumber())
        {
            // TODO: factor out
            int rowNumber = eventNumber - eventLogTable.getTopVisibleElement().getEventNumber();
            int rowHeight = eventLogTable.getRowHeight();
            int x = eventLogTable.getSize().x / 2;
            int y = eventLogTable.getHeaderHeight() + rowHeight * rowNumber + rowHeight / 2;
            clickAbsolute(RIGHT_MOUSE_BUTTON, eventLogTable.toDisplay(x, y));
        }
        else
            Assert.fail("Not yet implemented");
        
        return new MenuAccess(eventLogTable.getCanvas().getMenu());
    }
}
