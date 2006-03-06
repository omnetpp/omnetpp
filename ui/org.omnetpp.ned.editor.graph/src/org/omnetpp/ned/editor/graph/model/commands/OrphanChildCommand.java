package org.omnetpp.ned.editor.graph.model.commands;

import java.util.List;

import org.eclipse.draw2d.geometry.Point;
import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.INedContainer;
import org.omnetpp.ned.editor.graph.model.INedModule;

/**
 * This command removes a model element from its parent.
 * @author rhornig
 *
 */

public class OrphanChildCommand extends Command {

    private Point oldLocation;
    private INedContainer diagram;
    private INedModule child;
    private int index;

    public OrphanChildCommand() {
        super(MessageFactory.OrphanChildCommand_Label);
    }

    public void execute() {
        List children = diagram.getModelChildren();
        index = children.indexOf(child);
        oldLocation = child.getLocation();
        diagram.removeModelChild(child);
    }

    public void redo() {
        diagram.removeModelChild(child);
    }

    public void setChild(INedModule child) {
        this.child = child;
    }

    public void setParent(INedContainer parent) {
        diagram = parent;
    }

    public void undo() {
        child.setLocation(oldLocation);
        diagram.insertModelChild(index, child);
    }

}
