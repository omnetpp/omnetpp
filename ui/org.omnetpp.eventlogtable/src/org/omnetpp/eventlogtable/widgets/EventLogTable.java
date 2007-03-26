package org.omnetpp.eventlogtable.widgets;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.QualifiedName;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.TableColumn;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.EventLogSelection;
import org.omnetpp.common.virtualtable.VirtualTable;
import org.omnetpp.common.virtualtable.VirtualTableSelection;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.EventLogTableFacade;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlogtable.editors.EventLogTableContributor;

public class EventLogTable extends VirtualTable<EventLogEntry> {
	public EventLogTable(Composite parent, int style) {
		super(parent, style);

		setContentProvider(new EventLogTableContentProvider());
		setLineRenderer(new EventLogTableLineRenderer());

		TableColumn tableColumn = createColumn();
		tableColumn.setWidth(60);
		tableColumn.setText("Event #");

		tableColumn = createColumn();
		tableColumn.setWidth(140);
		tableColumn.setText("Time");

		tableColumn = createColumn();
		tableColumn.setWidth(2000);
		tableColumn.setText("Details");
	}

	public void setEventLogTableContributor(EventLogTableContributor eventLogTableContributor) {
		MenuManager menuManager = new MenuManager();
		eventLogTableContributor.contributeToPopupMenu(menuManager);
		setMenu(menuManager.createContextMenu(this));
	}

	@Override
	public ISelection getSelection() {
		List<EventLogEntry> selectionElements = getSelectionElements();
		
		if (selectionElements == null)
			return null;
		else {
			ArrayList<IEvent> selectionEvents = new ArrayList<IEvent>();
			
			for (EventLogEntry selectionElement : selectionElements)
				selectionEvents.add(selectionElement.getEvent());
	
			return new EventLogSelection(getEventLogInput(), selectionEvents);
		}
	}

	@Override
	public void setSelection(ISelection selection) {
		if (selection instanceof EventLogSelection) {
			EventLogSelection eventLogSelection = (EventLogSelection)selection;
			List<EventLogEntry> eventLogEntries = new ArrayList<EventLogEntry>();

			for (IEvent event : eventLogSelection.getEvents())
				eventLogEntries.add(event.getEventEntry());
			
			super.setSelection(new VirtualTableSelection<EventLogEntry>(eventLogSelection.getEventLogInput(), eventLogEntries));
		}
	}
	
	@Override
	public void setInput(Object input) {
		EventLogInput eventLogInput = getEventLogInput();

		// store current position
		if (eventLogInput != null) {
			EventLogEntry eventLogEntry = getTopVisibleElement();
			
			if (eventLogEntry != null) {
				String eventNumber = String.valueOf(eventLogEntry.getEvent().getEventNumber());
				try {
					eventLogInput.getFile().setPersistentProperty(getEventLogTableEventNumberPropertyName(), eventNumber);
				}
				catch (CoreException e) {
					throw new RuntimeException(e);
				}
			}
		}

		super.setInput(input);

		// restore last known position
		eventLogInput = (EventLogInput)input;

		if (eventLogInput != null) {
			try {
				String eventNumber = eventLogInput.getFile().getPersistentProperty(getEventLogTableEventNumberPropertyName());

				if (eventNumber != null)
					gotoElement(getEventLog().getEventForEventNumber(Integer.parseInt(eventNumber)).getEventEntry());
			}
			catch (CoreException e) {
				throw new RuntimeException(e);
			}
		}
	}

	private QualifiedName getEventLogTableEventNumberPropertyName() {
		return new QualifiedName("EventLogTable", "EventNumber");
	}
	
	public EventLogInput getEventLogInput() {
		return (EventLogInput)getInput();		
	}

	public IEventLog getEventLog() {
		EventLogInput eventLogInput = getEventLogInput();
		
		if (eventLogInput == null)
			return null;
		else
			return eventLogInput.getEventLog();
	}

	public EventLogTableFacade getEventLogTableFacade() {
		return ((EventLogInput)getInput()).getEventLogTableFacade();
	}

	public EventLogTableContentProvider getEventLogTableContentProvider() {
		return (EventLogTableContentProvider)getContentProvider();
	}

	public int getDisplayMode() {
		return getEventLogTableFacade().getDisplayMode();
	}

	public void setDisplayMode(int i) {
		getEventLogTableFacade().setDisplayMode(i);
	}

	public int getFilterMode() {
		return getEventLogTableFacade().getFilterMode();
	}

	public void setFilterMode(int i) {
		getEventLogTableFacade().setFilterMode(i);
		stayNear();
	}
}
