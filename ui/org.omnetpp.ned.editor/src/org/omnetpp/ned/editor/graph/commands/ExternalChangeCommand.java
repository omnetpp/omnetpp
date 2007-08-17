package org.omnetpp.ned.editor.graph.commands;

import java.util.ArrayList;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.notification.NEDAttributeChangeEvent;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.notification.NEDStructuralChangeEvent;

public class ExternalChangeCommand extends Command {
	private ArrayList<NEDModelEvent> events = new ArrayList<NEDModelEvent>();

	public ExternalChangeCommand() {
		super("External change");
	}

	public void addEvent(NEDModelEvent event) {
		events.add(event);
	}
	
	@Override
	public void execute() {
		// intentionally left blank, because the changes are already applied
		// that's why it is an external command
	}
	
	@Override
	public void redo() {
		for (NEDModelEvent event : events)
			redoEvent(event);
	}
	
	private void redoEvent(NEDModelEvent event) {
		if (event instanceof NEDAttributeChangeEvent) {
			NEDAttributeChangeEvent attributeChangeEvent = (NEDAttributeChangeEvent)event;
			attributeChangeEvent.getSource().setAttribute(attributeChangeEvent.getAttribute(), (String)attributeChangeEvent.getNewValue());
		}
		else if (event instanceof NEDStructuralChangeEvent) {
			NEDStructuralChangeEvent structuralChangeEvent = (NEDStructuralChangeEvent)event;
			switch (structuralChangeEvent.getType()) {
				case INSERTION:
					structuralChangeEvent.getSource().insertChildBefore(structuralChangeEvent.getNewLocation(), structuralChangeEvent.getChild());
					break;
				case REMOVAL:
					structuralChangeEvent.getChild().removeFromParent();
					break;
				default:
					throw new RuntimeException("Unknown structural event type");
			}
		}
		else
			throw new RuntimeException("Unknown event type");
	}
	
	@Override
	public void undo() {
		for (int i = events.size() - 1; i >= 0; i--)
			undoEvent(events.get(i));
	}

	private void undoEvent(NEDModelEvent event) {
		if (event instanceof NEDAttributeChangeEvent) {
			NEDAttributeChangeEvent attributeChangeEvent = (NEDAttributeChangeEvent)event;
			attributeChangeEvent.getSource().setAttribute(attributeChangeEvent.getAttribute(), (String)attributeChangeEvent.getOldValue());
		}
		else if (event instanceof NEDStructuralChangeEvent) {
			NEDStructuralChangeEvent structuralChangeEvent = (NEDStructuralChangeEvent)event;
			switch (structuralChangeEvent.getType()) {
				case INSERTION:
					structuralChangeEvent.getChild().removeFromParent();
					break;
				case REMOVAL:
					structuralChangeEvent.getSource().insertChildBefore(structuralChangeEvent.getOldLocation(), structuralChangeEvent.getChild());
					break;
				default:
					throw new RuntimeException("Unknown structural event type");
			}
		}
		else
			throw new RuntimeException("Unknown event type");
	}
}
