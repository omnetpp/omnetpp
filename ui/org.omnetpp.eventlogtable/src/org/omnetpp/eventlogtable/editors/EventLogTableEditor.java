package org.omnetpp.eventlogtable.editors;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.INavigationLocation;
import org.eclipse.ui.INavigationLocationProvider;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.ide.IGotoMarker;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlogtable.widgets.EventLogTable;

public class EventLogTableEditor extends EventLogEditor implements IResourceChangeListener, INavigationLocationProvider, IGotoMarker {
	protected Runnable locationTimer;

	private int lastLocationEventNumber;

	protected EventLogTable eventLogTable;

	@Override
	public void init(IEditorSite site, IEditorInput input) throws PartInitException {
		super.init(site, input);
		ResourcesPlugin.getWorkspace().addResourceChangeListener(this);
	}

	@Override
	public void dispose() {
		ResourcesPlugin.getWorkspace().removeResourceChangeListener(this);
	}

	@Override
	public void createPartControl(Composite parent) {
		IEditorSite editorSite = (IEditorSite)getSite();
		eventLogTable = new EventLogTable(parent, editorSite.getActionBars());
		eventLogTable.setInput(eventLogInput);

		locationTimer = new Runnable() {
			public void run() {
				markLocation();
			}
		};

		eventLogTable.getTable().addPaintListener(new PaintListener() {
			public void paintControl(PaintEvent e) {
				int eventNumber = eventLogTable.getTopVisibleElement().getEvent().getEventNumber();

				if (eventNumber != lastLocationEventNumber) {
					lastLocationEventNumber = eventNumber;
					Display.getCurrent().timerExec(3000, locationTimer);
				}
			}
		});

	}

	@Override
	public void setFocus() {
		eventLogTable.getControl().setFocus();
	}

	public void markLocation() {
		getSite().getPage().getNavigationHistory().markLocation(this);
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
			EventLogTableLocation eventLogTableLocation = (EventLogTableLocation)currentLocation;
			
			return eventLogTableLocation.eventNumber == eventNumber;	
		}

		public void releaseState() {
			// void
		}

		public void restoreLocation() {
			IEvent event = eventLogInput.getEventLog().getEventForEventNumber(eventNumber);
			EventLogEntry eventLogEntry = event != null ? event.getEventEntry() : null;
			
			if (eventLogEntry != null) {
				lastLocationEventNumber = event.getEventNumber();
				eventLogTable.gotoElement(eventLogEntry);
			}
		}

		public void restoreState(IMemento memento) {
			Integer integer = memento.getInteger("EventNumber");
			
			if (integer != null)
				eventNumber = integer;
		}

		public void saveState(IMemento memento) {
			memento.putInteger("EventNumber", eventNumber);
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
		int eventNumber = marker.getAttribute("EventNumber", -1);
		eventLogTable.gotoElement(eventLogInput.getEventLog().getEventForEventNumber(eventNumber).getEventEntry());
	}

	public void resourceChanged(IResourceChangeEvent event) {
		// TODO: check if the affected resource is ours
		if (event.getDelta().getKind() == IResourceDelta.CHANGED)
			eventLogTable.refresh();
	}
}
