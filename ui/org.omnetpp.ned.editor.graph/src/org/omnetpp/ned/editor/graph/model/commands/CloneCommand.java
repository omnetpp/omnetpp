package org.omnetpp.ned.editor.graph.model.commands;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned2.model.CompoundModuleNodeEx;
import org.omnetpp.ned2.model.ConnectionNodeEx;
import org.omnetpp.ned2.model.INedContainer;
import org.omnetpp.ned2.model.INedModule;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.pojo.ConnectionsNode;

/**
 * Clones a set of modules (copy operation)
 * @author rhornig
 *
 */
public class CloneCommand extends Command {

    private List<INedModule> modules, newModules;
    private List<ConnectionNodeEx> newConnections;
    private INedContainer parent;
    private Map<INedModule, Rectangle> bounds;
    private Map<INedModule, Integer> indices;
    private Map<INedModule, INedModule> old2newMapping;

    public CloneCommand() {
        super(MessageFactory.CloneCommand_Label);
        modules = new LinkedList<INedModule> ();
    }

    public void addModule(INedModule mod, Rectangle newBounds) {
        modules.add(mod);
        if (bounds == null) {
            bounds = new HashMap<INedModule, Rectangle>();
        }
        bounds.put(mod, newBounds);
    }

    public void addModule(INedModule mod, int index) {
        modules.add(mod);
        if (indices == null) {
            indices = new HashMap<INedModule, Integer>();
        }
        indices.put(mod, index);
    }

    /**
     * Clone the provided connection
     * @param oldConn
     */
    protected ConnectionNodeEx cloneConnection(ConnectionNodeEx oldConn, INedModule srcModuleRef, INedModule destModuleRef) {
            
        ConnectionsNode connectionParent = null;
        if (parent instanceof CompoundModuleNodeEx)
            connectionParent = ((CompoundModuleNodeEx)parent).getFirstConnectionsChild();
        
        ConnectionNodeEx newConn = (ConnectionNodeEx)oldConn.deepDup(null);
            
        connectionParent.appendChild(newConn);
        newConn.setSrcModuleRef(srcModuleRef);
        newConn.setDestModuleRef(destModuleRef);

        newConnections.add(newConn);

        return newConn;
    }
    
    protected INedModule cloneModule(INedModule oldModule, Rectangle newBounds, int index) {
    	INedModule newModule = null;

        // duplicate the subtree but do not add to the new parent yet
        newModule = (INedModule)((NEDElement)oldModule).deepDup(null);
        newModule.setLocation(newBounds.getLocation());
        newModule.setName(oldModule.getName()+"_copy");

        if (index < 0) {
            parent.addModelChild(newModule);
        } else {
            parent.insertModelChild(index, newModule);
        }

        // keep track of the new modules so we can delete them in undo
        newModules.add(newModule);
        
        // keep track of the newModule -> OldModule map so that we can properly
        // attach all connections later.
        old2newMapping.put(oldModule, newModule);
        
        return newModule;
    }

    public void execute() {
        redo();
    }

    public void setParent(INedContainer parent) {
        this.parent = parent;
    }

    public void redo() {
        old2newMapping = new HashMap<INedModule, INedModule>();
        newConnections = new LinkedList<ConnectionNodeEx>();
        newModules = new LinkedList<INedModule>();

        for (INedModule mod : modules){
            if (bounds != null && bounds.containsKey(mod)) {
                cloneModule(mod, bounds.get(mod), -1);
            } else if (indices != null && indices.containsKey(mod)) {
                cloneModule(mod, null, indices.get(mod));
            } else {
                cloneModule(mod, null, -1);
            }
        }

        // go through all modules that were previously cloned and check all the source connections
        for (INedModule oldSrcMod : modules)
            for (ConnectionNodeEx oldConn : oldSrcMod.getSrcConnections()) {
                INedModule oldDestMod = oldConn.getDestModuleRef();
                // if the destination side was also selected clone this connection connection too 
                // TODO future: clone the connections ONLY if they are selected too
                if (old2newMapping.containsKey(oldDestMod)) {
                    INedModule newSrcMod = old2newMapping.get(oldSrcMod);
                    INedModule newDestMod = old2newMapping.get(oldDestMod);
                    cloneConnection(oldConn, newSrcMod, newDestMod);
                }
            }
    }

    public void undo() {
        for (INedModule mod : newModules)
            mod.removeFromParent();
        
        for (ConnectionNodeEx conn : newConnections)
            conn.removeFromParent();
        
    }

}