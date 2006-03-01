package org.omnetpp.ned.editor.graph.model.commands;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.ConnectionNodeEx;
import org.omnetpp.ned.editor.graph.model.INedContainer;
import org.omnetpp.ned.editor.graph.model.INedNode;

/**
 * Deletes an object from the model and also removes all associated connections
 * @author rhornig
 *
 */
public class DeleteCommand extends Command {

    private INedNode child;
    private INedContainer parent;
    private int index = -1;
    private List<ConnectionNodeEx> srcConns = new ArrayList<ConnectionNodeEx>();
    private List<ConnectionNodeEx> destConns = new ArrayList<ConnectionNodeEx>();

    public DeleteCommand() {
        super(MessageFactory.DeleteCommand_Label);
    }

    private void deleteConnections(INedNode part) {
        if (part instanceof INedContainer) {
        	// delete all children
            for (INedNode currChild : ((INedContainer) part).getModelChildren())
                deleteConnections(currChild);
        }
        srcConns.addAll(part.getSrcConnections());
        for (int i = 0; i < srcConns.size(); i++) {
            ConnectionNodeEx wire = srcConns.get(i);
            wire.setSrcModuleRef(null);		// remove the connections
//            wire.setDestModuleRef(null);
        }
        destConns.addAll(part.getDestConnections());
        for (int i = 0; i < destConns.size(); i++) {
        	ConnectionNodeEx wire = destConns.get(i);
//            wire.setSrcModuleRef(null);		// remove the connections
            wire.setDestModuleRef(null);
        }
    }

    public void execute() {
        primExecute();
    }

    protected void primExecute() {
        deleteConnections(child);
        index = parent.getModelChildren().indexOf(child);
        parent.removeModelChild(child);
    }

    public void redo() {
        primExecute();
    }

    private void restoreConnections() {
        for (ConnectionNodeEx wire : srcConns) {
            wire.setSrcModuleRef(child);
//            wire.setDestModuleRef(null);
        }
        srcConns.clear();
        for (ConnectionNodeEx wire : destConns) {
//            wire.setSrcModuleRef(null);
            wire.setDestModuleRef(child);;
        }
        destConns.clear();
    }

    public void setChild(INedNode c) {
        child = c;
    }

    public void setParent(INedContainer p) {
        parent = p;
    }

    public void undo() {
        parent.insertModelChild(index, child);
        restoreConnections();
    }

}
