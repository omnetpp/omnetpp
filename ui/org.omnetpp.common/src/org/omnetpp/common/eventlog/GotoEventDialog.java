package org.omnetpp.common.eventlog;

import org.eclipse.jface.dialogs.IInputValidator;
import org.omnetpp.common.ui.InputDialog;
import org.omnetpp.eventlog.IEvent;
import org.omnetpp.eventlog.IEventLog;

public class GotoEventDialog extends InputDialog {
    private IEventLog eventLog;

    public GotoEventDialog(final IEventLog eventLog) {
        super(null, "Go to Event", "Enter the event number to go to:", null, new IInputValidator() {
            public String isValid(String newText) {
                try {
                    long eventNumber = Long.parseLong(newText);
                    if (eventNumber >= 0) {
                        if (eventLog == null)
                            return null;
                        else {
                            IEvent event = eventLog.getEventForEventNumber(eventNumber);
                            if (event == null)
                                return "No such event";
                            else
                                return null;
                        }
                    }
                    else
                        return "Negative event number";
                }
                catch (Exception e) {
                    e.printStackTrace();;
                    return "Not a number";
                }
            }
        });
        this.eventLog = eventLog;
    }

    public long getEventNumber() {
        try {
            return Long.parseLong(getValue());
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    public IEvent getEvent() {
        return eventLog.getEventForEventNumber(getEventNumber());
    }
}
