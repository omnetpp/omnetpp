package org.omnetpp.experimental.seqchart.widgets;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.ListenerList;
import org.eclipse.draw2d.ColorConstants;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.util.SafeRunnable;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;
import org.omnetpp.experimental.seqchart.editors.EventLogSelection;
import org.omnetpp.experimental.seqchart.editors.IEventLogSelection;
import org.omnetpp.scave.engine.EventEntry;
import org.omnetpp.scave.engine.EventLog;
import org.omnetpp.scave.engine.MessageEntries;
import org.omnetpp.scave.engine.PStringVector;

/**
 * Table to hold the event log. Has to be prepared for very large amounts of 
 * data (several hundred thousand lines).
 * 
 * @author andras
 */
//TODO actually implement expand/collapse! [+] and [-] images to be displayed are there in VirtualTableTreeBase; see also eventLog.collapseEvent/expandEvent
//FIXME on setInput() make sure tableRowIndex is initialized properly
//FIXME cannot scroll down!! it gets stuck on first logmessage line
//FIXME reduce circular notifications
public class EventLogTable extends VirtualTableTreeBase implements ISelectionProvider {

	private static final Color blueColor = ColorConstants.blue;

	private EventLog eventLog;

	private ListenerList selectionChangedListeners = new ListenerList(); // list of selection change listeners (type ISelectionChangedListener).

	private boolean followSelection = true;
	
	public EventLogTable(Composite parent, int style) {
		super(parent, style);
		
		table.setHeaderVisible(true);
		table.setLinesVisible(false);
		TableColumn tableColumn = new TableColumn(table, SWT.NONE);
		tableColumn.setWidth(60);
		tableColumn.setText("Event#");
		TableColumn tableColumn1 = new TableColumn(table, SWT.NONE);
		tableColumn1.setWidth(140);
		tableColumn1.setText("Time");
		TableColumn tableColumn4 = new TableColumn(table, SWT.NONE);
		tableColumn4.setWidth(800);
		tableColumn4.setText("Details / log message");
		
		addSelectionListener(new SelectionListener() {
			public void widgetDefaultSelected(SelectionEvent e) {
				fireSelectionChanged();
			}
			public void widgetSelected(SelectionEvent e) {
				fireSelectionChanged();
			}
		});
	}

	public void setInput(EventLog eventLog) {
		this.eventLog = eventLog;
		table.setItemCount(0);

		if (eventLog!=null) {
			// remember old selected element
			EventEntry oldSelectionEvent = null;
			if (eventLog!=null)
				oldSelectionEvent = getSelectionEvent();

			// make log messages visible
			eventLog.expandAllEvents();

			// calculate number of rows and set table row count
			EventEntry lastEvent = eventLog.getLastEvent();
			if (lastEvent != null) {
				int lastEventRows = 1 + (lastEvent.getIsExpandedInTree() ? lastEvent.getNumLogMessages() : 0);
				table.setItemCount(lastEvent==null ? 0 : lastEvent.getTableRowIndex()+lastEventRows);
			}

			// restore old event number; XXX needed???
			if (oldSelectionEvent!=null)
				gotoEvent(oldSelectionEvent);
		}
		
		// refresh
		table.clearAll();
	}

	/**
	 * Position the table selection to the given event, or if it's not in the current 
	 * filtered event log, to an event near that simulation time.
	 */
	public void gotoEvent(EventEntry event) {
		if (event!=null) {
			if (!eventLog.containsEvent(event))
				event = eventLog.getLastEventBefore(event.getSimulationTime());
			if (event!=null) {
				int index = event.getTableRowIndex();
				table.setSelection(index);
			}
		}
	}

	/**
	 * Called back whenever a new table row gets exposed. 
	 */
	@Override
	protected void fillTableLine(TableItem item, int lineNumber) {
		EventEntry event = eventLog.getEventByTableRowIndex(lineNumber);
		if (event==null) {
			item.setText(0, "null");
			return;
		}

		if (lineNumber == event.getTableRowIndex()) {
			//item.setFont(boldFont);
			item.setForeground(blueColor);
			//item.setImage(0, ImageFactory.getImage(ImageFactory.TOOLBAR_IMAGE_SIMPLE)); // plus/minus sign
			item.setText(0, "#"+event.getEventNumber());
			item.setText(1, ""+event.getSimulationTime()+"s");
			item.setText(2, "module ("+event.getCause().getModule().getModuleClassName()
							+") "+event.getCause().getModule().getModuleFullPath()
							+", on arrival of message ("+event.getCause().getMessageClassName()
							+") "+event.getCause().getMessageName());
		}
		else {
			int logMessageIndex = lineNumber - event.getTableRowIndex()-1;
			//Assert.isTrue(logMessageIndex>=0);
			String logMessage = getLogMessage(event, logMessageIndex);
			if (logMessage==null)
				logMessage = "bad log message index "+logMessageIndex; //XXX debug code
			item.setText(0, "");
			item.setText(1, "");
			item.setText(2, logMessage);
		}
	}

	/**
	 * Utility function to get the ith log message line for an event.
	 * They are stored with the causes[] messages.
	 */
	private String getLogMessage(EventEntry event, int logMessageIndex) {
		MessageEntries causes = event.getCauses();
		for (int i=0; i<causes.size(); i++) {
			PStringVector msgs = causes.get(i).getLogMessages();
			if (logMessageIndex < msgs.size())
				return msgs.get(logMessageIndex);
			else
				logMessageIndex -= msgs.size();
		}
		return null; // bad luck
	}

	/**
	 * Returns the current selection.
	 */
	public EventEntry getSelectionEvent() {
		int sel = getTable().getSelectionIndex();
		return eventLog.getEventByTableRowIndex(sel);
	}

	/**
	 * Returns the current selection.
	 */
	public List<EventEntry> getSelectionEvents() {
		int[] sels = getTable().getSelectionIndices();
		ArrayList<EventEntry> list = new ArrayList<EventEntry>();
		for (int sel : sels) {
			EventEntry event = eventLog.getEventByTableRowIndex(sel);
			list.add(event);
		}
		return list;
	}

	/**
	 * Selects the given events, and goes to the first one.
	 */
	public void setSelectionEvents(List<EventEntry> events) {
		if (events.size()>0)
			gotoEvent(events.get(0));
		int[] selIndices = new int[events.size()];
		for (int i=0; i<events.size(); i++)
			selIndices[i] = events.get(i).getTableRowIndex();
		getTable().setSelection(selIndices);
	}
	
	/**
     * Add a selection change listener.
     */
	public void addSelectionChangedListener(ISelectionChangedListener listener) {
        selectionChangedListeners.add(listener);
	}

	/**
     * Remove a selection change listener.
     */
	public void removeSelectionChangedListener(ISelectionChangedListener listener) {
		selectionChangedListeners.remove(listener);
	}
	
	/**
     * Notifies any selection changed listeners that the viewer's selection has changed.
     * Only listeners registered at the time this method is called are notified.
     */
    protected void fireSelectionChanged() {
        Object[] listeners = selectionChangedListeners.getListeners();
        final SelectionChangedEvent event = new SelectionChangedEvent(this, getSelection());        
        for (int i = 0; i < listeners.length; ++i) {
            final ISelectionChangedListener l = (ISelectionChangedListener) listeners[i];
            SafeRunnable.run(new SafeRunnable() {
                public void run() {
                    l.selectionChanged(event);
                }
            });
        }
    }

	/**
	 * Returns the currently "selected" events as an instance of IEventLogSelection.
	 */
	public ISelection getSelection() {
		return new EventLogSelection(eventLog, getSelectionEvents());
	}

	/**
	 * Sets the currently "selected" events. The selection must be an
	 * instance of IEventLogSelection. See setFollowSelection() too.
	 */
	public void setSelection(ISelection newSelection) {
		System.out.println("EventLogTable got selection: "+newSelection);
		
		if (followSelection) {
			// act as a view: display the event log which comes in the selection, 
			// or display nothing if the selection is not an event log. 
			if (!(newSelection instanceof IEventLogSelection)) {
				setInput(null);
				return;
			}
			IEventLogSelection newEventLogSelection = (IEventLogSelection)newSelection;
			if (newEventLogSelection.getEventLog() != eventLog)
				setInput(newEventLogSelection.getEventLog());

			//FIXME only notify if selection actually changed
			setSelectionEvents(newEventLogSelection.getEvents());
		}
		else {
			// act as an editor: stick to the current event log, and ignore selections 
			// that are about something else 
			if (!(newSelection instanceof IEventLogSelection))
				return; // wrong selection type
			IEventLogSelection newEventLogSelection = (IEventLogSelection)newSelection;
			if (newEventLogSelection.getEventLog() != eventLog)
				return;  // wrong -- refers to another eventLog

			//FIXME only notify if selection actually changed
			setSelectionEvents(newEventLogSelection.getEvents());
		}
	}

	/**
	 * See setFollowSelection().
	 */
	public boolean getFollowSelection() {
		return followSelection;
	}

	/**
	 * Sets whether this widget should always switch to the EventLog which comes in
	 * the selection (=true), or stick to the EventLog set with setInput() (=false).
	 * The proper setting typically depends on whether the widget is used in an
	 * editor (false) or in a view (true).
	 *
	 * Default is true.
	 */
	public void setFollowSelection(boolean followSelection) {
		this.followSelection = followSelection;
	}
}
