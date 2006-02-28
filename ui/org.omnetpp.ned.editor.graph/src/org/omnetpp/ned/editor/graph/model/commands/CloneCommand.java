package org.omnetpp.ned.editor.graph.model.commands;

import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.CompoundModuleNodeEx;
import org.omnetpp.ned.editor.graph.model.ConnectionNodeEx;
import org.omnetpp.ned.editor.graph.model.INedContainer;
import org.omnetpp.ned.editor.graph.model.INedModelElement;
import org.omnetpp.ned.editor.graph.model.INedNode;
import org.omnetpp.ned.editor.graph.model.NEDElementFactoryEx;
import org.omnetpp.ned.editor.graph.model.NedElementExUtil;
import org.omnetpp.ned.editor.graph.model.SubmoduleNodeEx;
import org.omnetpp.ned2.model.NEDElement;

/**
 * Clones a set of parts (copy operation)
 * @author rhornig
 *
 */
public class CloneCommand extends Command {

    private List parts, newTopLevelParts, newConnections;
    private INedContainer parent;
    private Map bounds, indices, connectionPartMap;

    public CloneCommand() {
        super(MessageFactory.CloneCommand_Label);
        parts = new LinkedList();
    }

    public void addPart(INedModelElement part, Rectangle newBounds) {
        parts.add(part);
        if (bounds == null) {
            bounds = new HashMap();
        }
        bounds.put(part, newBounds);
    }

    public void addPart(INedModelElement part, int index) {
        parts.add(part);
        if (indices == null) {
            indices = new HashMap();
        }
        indices.put(part, new Integer(index));
    }

    protected void clonePart(INedModelElement oldPart, INedContainer newParent, Rectangle newBounds,
            List newConnections, Map connectionPartMap, int index) {
    	NEDElement newPart = null;

        if (oldPart instanceof SubmoduleNodeEx) {
            newPart = NEDElementFactoryEx.getInstance().createNodeWithTag(NedElementExUtil.NED_SUBMODULE);
        } else if (oldPart instanceof CompoundModuleNodeEx) {
            newPart = NEDElementFactoryEx.getInstance().createNodeWithTag(NedElementExUtil.NED_COMPOUND_MODULE);
        } 

        if (oldPart instanceof INedContainer) {
            Iterator i = ((INedContainer)oldPart).getModelChildren().iterator();
            while (i.hasNext()) {
                // for children they will not need new bounds
                clonePart((INedModelElement) i.next(), (INedContainer) newPart, 
                		null, newConnections, connectionPartMap, -1);
            }
        }

        if (index < 0) {
            newParent.addModelChild((INedNode)newPart);
        } else {
            newParent.insertModelChild(index, (INedNode)newPart);
        }

        // keep track of the new parts so we can delete them in undo
        // keep track of the oldpart -> newpart map so that we can properly
        // attach
        // all connections.
        if (newParent == parent) newTopLevelParts.add(newPart);
        connectionPartMap.put(oldPart, newPart);
    }

    public void execute() {
        connectionPartMap = new HashMap();
        newConnections = new LinkedList();
        newTopLevelParts = new LinkedList();

        Iterator i = parts.iterator();

        INedModelElement part = null;
        while (i.hasNext()) {
            part = (INedModelElement) i.next();
            if (bounds != null && bounds.containsKey(part)) {
                clonePart(part, parent, (Rectangle) bounds.get(part), newConnections, connectionPartMap, -1);
            } else if (indices != null && indices.containsKey(part)) {
                clonePart(part, parent, null, newConnections, connectionPartMap,
                        ((Integer) indices.get(part)).intValue());
            } else {
                clonePart(part, parent, null, newConnections, connectionPartMap, -1);
            }
        }

        // go through and set the source of each connection to the proper
        // source.
        Iterator c = newConnections.iterator();

        while (c.hasNext()) {
            ConnectionNodeEx conn = (ConnectionNodeEx) c.next();
            INedModelElement source = conn.getSrcModuleRef();
            if (connectionPartMap.containsKey(source)) {
                conn.setSrcModuleRef((INedNode)connectionPartMap.get(source));
            }
        }

    }

    public void setParent(INedContainer parent) {
        this.parent = parent;
    }

    public void redo() {
        for (Iterator iter = newTopLevelParts.iterator(); iter.hasNext();)
            parent.addModelChild((INedNode) iter.next());
        for (Iterator iter = newConnections.iterator(); iter.hasNext();) {
            ConnectionNodeEx conn = (ConnectionNodeEx) iter.next();
            INedNode source = conn.getSrcModuleRef();
            if (connectionPartMap.containsKey(source)) {
                conn.setSrcModuleRef(((INedNode) connectionPartMap.get(source)));
            }
        }
    }

    public void undo() {
        for (Iterator iter = newTopLevelParts.iterator(); iter.hasNext();)
            parent.removeModelChild((INedNode) iter.next());
    }

}