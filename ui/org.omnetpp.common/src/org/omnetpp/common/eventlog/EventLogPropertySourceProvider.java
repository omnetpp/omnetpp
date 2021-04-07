package org.omnetpp.common.eventlog;

import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySourceProvider;
import org.omnetpp.eventlog.EventLogEntry;
import org.omnetpp.eventlog.IEvent;

public class EventLogPropertySourceProvider implements IPropertySourceProvider {
    protected EventLogEditor eventLogEditor;

    public EventLogPropertySourceProvider(EventLogEditor eventLogEditor) {
        this.eventLogEditor = eventLogEditor;
    }

    public IPropertySource getPropertySource(Object object) {
        // TODO: kill this case and let the others live as soon as
        // IEventLogSelection does not return the event numbers only and we can get more than that here
        if (object instanceof IPropertySource)
            return (IPropertySource)object;
        else if (object instanceof Long && eventLogEditor != null) {
            IEvent event = eventLogEditor.eventLogInput.getEventLog().getEventForEventNumber((Long)object);
            if (event == null)
                return null;
            else
                return new EventLogEntryPropertySource(event.getEventEntry());
        }
        else if (object instanceof EventLogEntry)
            return new EventLogEntryPropertySource((EventLogEntry)object);
        else if (object instanceof IEvent)
            return new EventLogEntryPropertySource(((IEvent)object).getEventEntry());
        else
            return null;
    }
}