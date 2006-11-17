package org.omnetpp.eventlogtable.editors;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.INavigationLocation;
import org.eclipse.ui.INavigationLocationProvider;
import org.eclipse.ui.IPathEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.ide.IGotoMarker;
import org.eclipse.ui.part.EditorPart;
import org.omnetpp.eventlog.engine.EventLog;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.FileReader;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlogtable.widgets.EventLogTable;

public class EventLogTableEditor extends EditorPart implements INavigationLocationProvider, IGotoMarker {
	private IEventLog eventLog;

	private EventLogTable eventLogTable;
	
	@Override
	public void init(IEditorSite site, IEditorInput input) throws PartInitException {
		setSite(site);
		setInput(input);
		setPartName(input.getName());

		String logFileName;
		if (input instanceof IFileEditorInput) {
			IFileEditorInput fileInput = (IFileEditorInput)input;
			logFileName = fileInput.getFile().getLocation().toFile().getAbsolutePath();
		}
		else if (input instanceof IPathEditorInput) {
			IPathEditorInput pathFileInput = (IPathEditorInput)input;
			logFileName = pathFileInput.getPath().toFile().getAbsolutePath();
		}
		else 
			throw new PartInitException("Unsupported input type");

		eventLog = new EventLog(new FileReader(logFileName, /* EventLog will delete it */false));
	}

	@Override
	public void createPartControl(Composite parent) {
		eventLogTable = new EventLogTable(parent);
	}

	@Override
	public void doSave(IProgressMonitor monitor) {
		// void
	}

	@Override
	public void doSaveAs() {
		// void
	}

	@Override
	public boolean isDirty() {
		return false;
	}

	@Override
	public boolean isSaveAsAllowed() {
		return false;
	}

	@Override
	public void setFocus() {
		eventLogTable.getControl().setFocus();
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
		return null;
	}

	public INavigationLocation createNavigationLocation() {
		return null;
	}

	public void gotoMarker(IMarker marker) {
	}
}
