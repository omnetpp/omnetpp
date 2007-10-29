package org.omnetpp.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Menu;
import org.omnetpp.common.eventlog.EventLogEntryReference;
import org.omnetpp.eventlogtable.widgets.EventLogTable;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.MenuAccess;
import com.simulcraft.test.gui.access.ShellAccess;
import com.simulcraft.test.gui.core.UIStep;
import com.simulcraft.test.gui.core.InBackgroundThread;

public class EventLogTableAccess
    extends CompositeAccess
{
    public EventLogTableAccess(EventLogTable eventLogTable) {
        super(eventLogTable);
    }
    
    @Override
    public EventLogTable getControl() {
        return (EventLogTable)widget;
    }
    
    @Override @UIStep
    protected Menu getContextMenu() {
        return getControl().getCanvas().getMenu();
    }
    
    @UIStep
    public void assertTopVisibleEventNumber(int eventNumber) {
        EventLogTable eventLogTable = getControl();
        Assert.assertTrue(eventLogTable.getTopVisibleElement().getEventNumber() == eventNumber);
    }

    @UIStep
    public void assertBottomFullyVisibleEventNumber(int eventNumber) {
        EventLogTable eventLogTable = getControl();
        Assert.assertTrue(eventLogTable.getBottomFullyVisibleElement().getEventNumber() == eventNumber);
    }

    @UIStep
    public void assertSelectedEventNumber(int eventNumber) {
        EventLogTable eventLogTable = getControl();
        Assert.assertTrue(eventLogTable.getSelectionElement().getEventNumber() == eventNumber);
    }

    @UIStep
    public int getPageJumpCount() {
        return getControl().getPageJumpCount();
    }
    
    @UIStep
    public MenuAccess activateContextMenuWithMouseClickAtSelectedElement() {
        EventLogTable eventLogTable = getControl();
        Point point = eventLogTable.getVisibleElementLocation(eventLogTable.getSelectionElement());
        clickAbsolute(RIGHT_MOUSE_BUTTON, eventLogTable.toDisplay(point.x, point.y));

        return new MenuAccess(getContextMenu());
    }

    @InBackgroundThread
    public MenuAccess activateContextMenuWithMouseClick(int eventNumber) {
        if (isEventNumberVisible(eventNumber))
            activateContextMenuWithMouseClickForVisibleEvent(eventNumber);
        else {
            gotoEventNumber(eventNumber);
            activateContextMenuWithMouseClickForVisibleEvent(eventNumber);
        }
        
        return new MenuAccess(getContextMenu());
    }
    
    @UIStep
    public boolean isEventNumberVisible(int eventNumber) {
        EventLogTable eventLogTable = getControl();
        return eventLogTable.getTopVisibleElement().getEventNumber() <= eventNumber &&
               eventNumber <= eventLogTable.getBottomFullyVisibleElement().getEventNumber();
    }

    @UIStep
    public void activateContextMenuWithMouseClickForVisibleEvent(int eventNumber) {
        EventLogTable eventLogTable = getControl();
        
        for (int i = 0; i < eventLogTable.getVisibleElementCount(); i++) {
            EventLogEntryReference reference = eventLogTable.getVisibleElementAt(i);

            if (reference.getEventNumber() == eventNumber) {
                Point point = eventLogTable.getVisibleElementLocation(eventLogTable.getSelectionElement());
                clickAbsolute(RIGHT_MOUSE_BUTTON, eventLogTable.toDisplay(point.x, point.y));
                return;
            }
        }

        Assert.fail("Event number " + eventNumber + " not visible");
    }
    
    @InBackgroundThread
    public void gotoEventNumber(int eventNumber) {
        activateContextMenuWithMouseClick().activateMenuItemWithMouse("Goto event...");
        ShellAccess shell = Access.findShellWithTitle("Goto event");
        shell.findTextAfterLabel("Please enter.*").typeIn(String.valueOf(eventNumber));
        shell.findButtonWithLabel("OK").selectWithMouseClick();
    }
}
