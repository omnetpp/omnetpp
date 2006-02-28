package org.omnetpp.ned.editor.graph.model.commands;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.INedContainer;
import org.omnetpp.ned.editor.graph.model.INedNode;
import org.omnetpp.ned.editor.graph.model.old.ContainerModel;
import org.omnetpp.ned.editor.graph.model.old.NedNodeModel;
import org.omnetpp.ned.editor.graph.model.old.WireModel;

/**
 * Deletes an object from the model and also removes all associated connections
 * @author rhornig
 *
 */
public class DeleteCommand extends Command {

    private INedNode child;
    private INedContainer parent;
    private int vAlign, hAlign;
    private int index = -1;
    private List sourceConnections = new ArrayList();
    private List targetConnections = new ArrayList();

    public DeleteCommand() {
        super(MessageFactory.DeleteCommand_Label);
    }

    private void deleteConnections(INedNode part) {
        if (part instanceof INedContainer) {
            List<INedNode> children = ((INedContainer) part).getModelChildren();
            for (int i = 0; i < children.size(); i++)
                deleteConnections((NedNodeModel) children.get(i));
        }
        sourceConnections.addAll(part.getSourceConnections());
        for (int i = 0; i < sourceConnections.size(); i++) {
            WireModel wire = (WireModel) sourceConnections.get(i);
            wire.detachSource();
            wire.detachTarget();
        }
        targetConnections.addAll(part.getTargetConnections());
        for (int i = 0; i < targetConnections.size(); i++) {
            WireModel wire = (WireModel) targetConnections.get(i);
            wire.detachSource();
            wire.detachTarget();
        }
    }

    public void execute() {
        primExecute();
    }

    protected void primExecute() {
        deleteConnections(child);
        index = parent.getChildren().indexOf(child);
        parent.removeChild(child);
    }

    public void redo() {
        primExecute();
    }

    private void restoreConnections() {
        for (int i = 0; i < sourceConnections.size(); i++) {
            WireModel wire = (WireModel) sourceConnections.get(i);
            wire.attachSource();
            wire.attachTarget();
        }
        sourceConnections.clear();
        for (int i = 0; i < targetConnections.size(); i++) {
            WireModel wire = (WireModel) targetConnections.get(i);
            wire.attachSource();
            wire.attachTarget();
        }
        targetConnections.clear();
    }

    public void setChild(NedNodeModel c) {
        child = c;
    }

    public void setParent(ContainerModel p) {
        parent = p;
    }

    public void undo() {
        parent.addChild(child, index);
        restoreConnections();
    }

}
