/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.commands;

import java.util.ArrayList;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.model.notification.NedAttributeChangeEvent;
import org.omnetpp.ned.model.notification.NedModelEvent;
import org.omnetpp.ned.model.notification.NedStructuralChangeEvent;

public class ExternalChangeCommand extends Command {
    private ArrayList<NedModelEvent> events = new ArrayList<NedModelEvent>();

    public ExternalChangeCommand() {
        super("External change");
    }

    public void addEvent(NedModelEvent event) {
        events.add(event);
    }

    @Override
    public void execute() {
        // intentionally left blank, because the changes are already applied
        // that's why it is an external command
    }

    @Override
    public void redo() {
        for (NedModelEvent event : events)
            redoEvent(event);
    }

    private void redoEvent(NedModelEvent event) {
        if (event instanceof NedAttributeChangeEvent) {
            NedAttributeChangeEvent attributeChangeEvent = (NedAttributeChangeEvent)event;
            attributeChangeEvent.getSource().setAttribute(attributeChangeEvent.getAttribute(), (String)attributeChangeEvent.getNewValue());
        }
        else if (event instanceof NedStructuralChangeEvent) {
            NedStructuralChangeEvent structuralChangeEvent = (NedStructuralChangeEvent)event;
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

    private void undoEvent(NedModelEvent event) {
        if (event instanceof NedAttributeChangeEvent) {
            NedAttributeChangeEvent attributeChangeEvent = (NedAttributeChangeEvent)event;
            attributeChangeEvent.getSource().setAttribute(attributeChangeEvent.getAttribute(), (String)attributeChangeEvent.getOldValue());
        }
        else if (event instanceof NedStructuralChangeEvent) {
            NedStructuralChangeEvent structuralChangeEvent = (NedStructuralChangeEvent)event;
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
