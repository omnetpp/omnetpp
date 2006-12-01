package org.omnetpp.eventlogtable.editors;

import org.eclipse.core.resources.IMarker;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.INavigationLocation;
import org.eclipse.ui.INavigationLocationProvider;
import org.eclipse.ui.ide.IGotoMarker;
import org.omnetpp.eventlog.editors.EventLogEditor;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlogtable.widgets.EventLogTable;

public class EventLogTableEditor extends EventLogEditor implements INavigationLocationProvider, IGotoMarker {
	protected EventLogTable eventLogTable;

	@Override
	public void createPartControl(Composite parent) {
		eventLogTable = new EventLogTable(parent);
		eventLogTable.setInput(eventLog);
	}

	@Override
	public void setFocus() {
		eventLogTable.getControl().setFocus();
	}

	public void markLocation() {
		getSite().getPage().getNavigationHistory().markLocation(EventLogTableEditor.this);
	}
	
	public class EventLogTableLocation implements INavigationLocation {
		private int eventNumber;
		
		public EventLogTableLocation(int eventNumber) {
			this.eventNumber = eventNumber;
		}

		public void dispose() {
			// void
		}

		public Object getInput() {
			return EventLogTableEditor.this.getEditorInput();
		}

		public String getText() {
			return EventLogTableEditor.this.getPartName() + ": #" + eventNumber;
		}

		public boolean mergeInto(INavigationLocation currentLocation) {
			return false;
		}

		public void releaseState() {
			// void
		}

		public void restoreLocation() {
			IEvent event = eventLog.getEventForEventNumber(eventNumber);
			EventLogEntry eventLogEntry = event == null ? event.getEventEntry() : null;
			
			if (eventLogEntry != null)
				eventLogTable.gotoElement(eventLogEntry);
		}

		public void restoreState(IMemento memento) {
			// TODO: implement
		}

		public void saveState(IMemento memento) {
			// TODO: implement
		}

		public void setInput(Object input) {
			EventLogTableEditor.this.setInput((IFileEditorInput)input);
		}

		public void update() {
			// void
		}
	}

	public INavigationLocation createEmptyNavigationLocation() {
		return new EventLogTableLocation(0);
	}

	public INavigationLocation createNavigationLocation() {
		return new EventLogTableLocation(eventLogTable.getTopVisibleElement().getEvent().getEventNumber());
	}

	public void gotoMarker(IMarker marker) {
		// TODO:
	}
}
