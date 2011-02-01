package org.omnetpp.common.eventlog;

import org.eclipse.jface.dialogs.IInputValidator;
import org.omnetpp.common.ui.InputDialog;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;

public class GotoEventDialog extends InputDialog {
    private IEventLog eventLog;

    public GotoEventDialog(final IEventLog eventLog) {
        super(null, "Goto Event", "Please enter the event number to go to:", null, new IInputValidator() {
            public String isValid(String newText) {
                try {
                    int eventNumber = Integer.parseInt(newText);
                    if (eventNumber >= 0) {
                        IEvent event = getEvent(eventLog, newText);
                        if (event == null)
                            return "No such event";
                        else
                            return null;
                    }
                    else
                        return "Negative event number";
                }
                catch (Exception e) {
                    return "Not a number";
                }
            }
        });
        this.eventLog = eventLog;
    }

    public IEvent getEvent() {
        return getEvent(eventLog, getValue());
    }

    private static IEvent getEvent(IEventLog eventLog, String text) {
        int eventNumber = Integer.parseInt(text);
        return eventLog.getEventForEventNumber(eventNumber);
    }
}
