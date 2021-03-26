package org.omnetpp.eventlogtable.editors;

import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.INavigationLocation;
import org.eclipse.ui.NavigationLocation;
import org.omnetpp.common.eventlog.EventLogEntryReference;
import org.omnetpp.eventlog.IEvent;

public class EventLogTableLocation extends NavigationLocation {
    private long eventNumber;
    private String text;

    public EventLogTableLocation(EventLogTableEditor eventLogTableEditor, long eventNumber) {
        super(eventLogTableEditor);
        this.eventNumber = eventNumber;
        this.text = eventLogTableEditor.getTitle() + ": #" + eventNumber;
    }

    @Override
    public String getText() {
        return text;
    }

    public boolean mergeInto(INavigationLocation currentLocation) {
        return equals(currentLocation);
    }

    public void restoreLocation() {
        IEditorPart editorPart = getEditorPart();
        if (editorPart instanceof EventLogTableEditor) {
            EventLogTableEditor eventLogTableEditor = (EventLogTableEditor)editorPart;
            IEvent event = eventLogTableEditor.getEventLog().getEventForEventNumber(eventNumber);
            if (event != null)
                eventLogTableEditor.getEventLogTable().reveal(new EventLogEntryReference(event.getEventEntry()));
        }
    }

    public void restoreState(IMemento memento) {
        Long value = Long.parseLong(memento.getString("eventNumber"));
        if (value != null)
            eventNumber = value;
    }

    public void saveState(IMemento memento) {
        memento.putString("eventNumber", String.valueOf(eventNumber));
    }

    public void update() {
        // void
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + (int) (eventNumber ^ (eventNumber >>> 32));
        return result;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        EventLogTableLocation other = (EventLogTableLocation) obj;
        if (eventNumber != other.eventNumber)
            return false;
        return true;
    }
}