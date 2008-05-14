package org.omnetpp.eventlogtable.editors;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.INavigationLocation;
import org.eclipse.ui.INavigationLocationProvider;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.ide.IGotoMarker;
import org.eclipse.ui.part.IShowInSource;
import org.eclipse.ui.part.IShowInTargetList;
import org.eclipse.ui.part.ShowInContext;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.eventlog.EventLogEditor;
import org.omnetpp.common.eventlog.EventLogEntryReference;
import org.omnetpp.common.eventlog.IEventLogSelection;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlogtable.EventLogTablePlugin;
import org.omnetpp.eventlogtable.widgets.EventLogTable;

/**
 * Event log table display tool. (It is not actually an editor; it is only named so
 * because it extends EditorPart).
 * 
 * @author levy
 */
public class EventLogTableEditor extends EventLogEditor implements INavigationLocationProvider, IGotoMarker, IShowInSource, IShowInTargetList {
	private ResourceChangeListener resourceChangeListener = new ResourceChangeListener();

	private EventLogTable eventLogTable;

	private ISelectionListener selectionListener;

	public EventLogTable getEventLogTable() {
		return eventLogTable;
	}
	
	@Override
	public void init(IEditorSite site, IEditorInput input) throws PartInitException {
		super.init(site, input);
		ResourcesPlugin.getWorkspace().addResourceChangeListener(resourceChangeListener);

		// try to open the sequence chart view
		try {
			// Eclipse feature: during startup, showView() throws "Abnormal Workbench Condition" because perspective is null
			if (getSite().getPage().getPerspective() != null)
				getSite().getPage().showView("org.omnetpp.sequencechart.editors.SequenceChartView");
		}
		catch (PartInitException e) {
			EventLogTablePlugin.getDefault().logException(e);					
		}
	}

	@Override
	public void dispose() {
		if (resourceChangeListener != null)
			ResourcesPlugin.getWorkspace().removeResourceChangeListener(resourceChangeListener);
		
		if (selectionListener != null)
			getSite().getPage().removeSelectionListener(selectionListener);

		super.dispose();
	}

	@Override
	public void createPartControl(Composite parent) {
		eventLogTable = new EventLogTable(parent, SWT.NONE);
		eventLogTable.setFollowSelection(false);
		eventLogTable.setInput(eventLogInput);
		eventLogTable.setEventLogTableContributor(EventLogTableContributor.getDefault());

		getSite().setSelectionProvider(eventLogTable);
		addLocationProviderPaintListener(eventLogTable.getCanvas());

		// follow selection
		selectionListener = new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				if (part != EventLogTableEditor.this && selection instanceof IEventLogSelection) {
					IEventLogSelection eventLogSelection = (IEventLogSelection)selection;
					
					if (eventLogSelection.getEventLogInput() == eventLogTable.getInput()) {
						eventLogTable.setSelection(selection);
						markLocation();
					}
				}
			}
		};
		getSite().getPage().addSelectionListener(selectionListener);
	}

	@Override
	public void setFocus() {
		eventLogTable.setFocus();
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
			if (currentLocation instanceof EventLogTableLocation) {
				EventLogTableLocation eventLogTableLocation = (EventLogTableLocation)currentLocation;
				
				return eventLogTableLocation.eventNumber == eventNumber;
			}
			else
				return false;
		}

		public void releaseState() {
			// void
		}

		public void restoreLocation() {
			IEvent event = eventLogInput.getEventLog().getEventForEventNumber(eventNumber);
			EventLogEntry eventLogEntry = event != null ? event.getEventEntry() : null;
			
			if (eventLogEntry != null)
				eventLogTable.scrollToElement(new EventLogEntryReference(eventLogEntry));
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

		@Override
		public int hashCode() {
			final int PRIME = 31;
			int result = 1;
			result = PRIME * result + eventNumber;
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
			final EventLogTableLocation other = (EventLogTableLocation) obj;
			if (eventNumber != other.eventNumber)
				return false;
			return true;
		}
	}

	public INavigationLocation createEmptyNavigationLocation() {
		return new EventLogTableLocation(0);
	}

	public INavigationLocation createNavigationLocation() {
		EventLogEntryReference eventLogEntryReference = eventLogTable.getTopVisibleElement();
		
		if (eventLogEntryReference == null)
			return null;
		else
			return new EventLogTableLocation(eventLogEntryReference.getEventLogEntry(eventLogInput).getEvent().getEventNumber());
	}

	public void gotoMarker(IMarker marker) {
		int eventNumber = marker.getAttribute("EventNumber", -1);
		IEvent event = eventLogInput.getEventLog().getEventForEventNumber(eventNumber);
		
		if (event != null)
			eventLogTable.gotoElement(new EventLogEntryReference(event.getEventEntry()));
	}

	private class ResourceChangeListener implements IResourceChangeListener, IResourceDeltaVisitor {
		public void resourceChanged(IResourceChangeEvent event) {
            try {
                IResourceDelta delta = event.getDelta();

                if (delta != null)
                	delta.accept(this);
            }
            catch (CoreException e) {
            	throw new RuntimeException(e);
            }
		}
		
        public boolean visit(IResourceDelta delta) {
            if (delta != null && delta.getResource() != null && delta.getResource().equals(eventLogInput.getFile())) {
        		Display.getDefault().asyncExec(new Runnable() {
					public void run() {
					    if (!eventLogTable.isDisposed())
					        eventLogTable.redraw();
					}            		
            	});
            }

            return true;
        }	
	}

	/* (non-Javadoc)
     * Method declared on IShowInSource
     */
    public ShowInContext getShowInContext() {
        return new ShowInContext(getEditorInput(), getSite().getSelectionProvider().getSelection());
    }

    /* (non-Javadoc)
     * Method declared on IShowInTargetList
     */
    public String[] getShowInTargetIds() {
        // contents of the "Show In..." context menu
        return new String[] {
                IConstants.SEQUENCECHART_VIEW_ID, 
                IConstants.MODULEHIERARCHY_VIEW_ID,
                };
    }
}
