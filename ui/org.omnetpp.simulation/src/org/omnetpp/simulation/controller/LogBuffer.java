package org.omnetpp.simulation.controller;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.ISafeRunnable;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.core.runtime.SafeRunner;
import org.omnetpp.simulation.SimulationPlugin;


/**
 * TODO
 *
 * @author Andras
 */
//TODO LogBuffer should notify ModuleOutputContent when it changes!
//FIXME making it possible to change module name and parent after creation is a REALLY BAD IDEA -- makes our job much more difficult in the IDE!!!
public class LogBuffer {
    private List<EventEntry> eventEntries = new ArrayList<EventEntry>();
    private ListenerList changeListeners = new ListenerList();

    public interface ILogBufferChangedListener {
        void changed(LogBuffer logBuffer);
    }
    
    public LogBuffer() {
    }
    
    public void addEventEntry(EventEntry e) {
        eventEntries.add(e);
    }

    public int getNumEntries() {
        return eventEntries.size();
    }
    
    public EventEntry getEventEntry(int index) {
        return eventEntries.get(index);
    }
    
    public EventEntry getLastEventEntry() {
        return eventEntries.isEmpty() ? null : eventEntries.get(eventEntries.size()-1);
    }
    
    // to be called manually after a batch of addEventEntry() calls
    public void fireChangeNotification() {
        for (Object o: changeListeners.getListeners()) {
            final ILogBufferChangedListener listener = (ILogBufferChangedListener) o;
            SafeRunner.run(new ISafeRunnable() {
                @Override
                public void run() throws Exception {
                    listener.changed(LogBuffer.this);
                }

                @Override
                public void handleException(Throwable e) {
                    SimulationPlugin.logError(e);
                }
            });
        }
    }
    
    public void addChangeListener(ILogBufferChangedListener listener) {
        changeListeners.add(listener);
    }

    public void removeChangeListener(ILogBufferChangedListener listener) {
        changeListeners.remove(listener);
    }

}
