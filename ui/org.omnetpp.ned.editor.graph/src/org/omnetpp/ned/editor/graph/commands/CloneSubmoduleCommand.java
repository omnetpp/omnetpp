package org.omnetpp.ned.editor.graph.commands;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.commands.Command;
import org.omnetpp.ned2.model.CompoundModuleNodeEx;
import org.omnetpp.ned2.model.ConnectionNodeEx;
import org.omnetpp.ned2.model.IConnectable;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.SubmoduleNodeEx;
import org.omnetpp.ned2.model.pojo.ConnectionsNode;

/**
 * Clones a set of submodules (copy operation)
 * @author rhornig
 */
public class CloneSubmoduleCommand extends Command {

    private List<SubmoduleNodeEx> modules, newModules;
    private List<ConnectionNodeEx> newConnections;
    private CompoundModuleNodeEx parent;
    private Map<SubmoduleNodeEx, Rectangle> bounds;
    private Map<SubmoduleNodeEx, Integer> indices;
    private Map<SubmoduleNodeEx, SubmoduleNodeEx> old2newMapping;
    private float scale = 1.0f;

    public CloneSubmoduleCommand(CompoundModuleNodeEx parent, float scale) {
        super("Clone");
        this.scale = scale;
        this.parent = parent;
        modules = new LinkedList<SubmoduleNodeEx> ();
    }

    public void addModule(SubmoduleNodeEx mod, Rectangle newBounds) {
        modules.add(mod);
        if (bounds == null) {
            bounds = new HashMap<SubmoduleNodeEx, Rectangle>();
        }
        bounds.put(mod, newBounds);
    }

    public void addModule(SubmoduleNodeEx mod, int index) {
        modules.add(mod);
        if (indices == null) {
            indices = new HashMap<SubmoduleNodeEx, Integer>();
        }
        indices.put(mod, index);
    }

    /**
     * Clone the provided connection
     * @param oldConn
     */
    protected ConnectionNodeEx cloneConnection(ConnectionNodeEx oldConn, SubmoduleNodeEx srcModuleRef, SubmoduleNodeEx destModuleRef) {
            
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
    
    protected SubmoduleNodeEx cloneModule(SubmoduleNodeEx oldModule, Rectangle newBounds, int index) {
    	SubmoduleNodeEx newModule = null;

        // duplicate the subtree but do not add to the new parent yet
        newModule = (SubmoduleNodeEx)((NEDElement)oldModule).deepDup(null);
        // FIXME is this working ok if we clone inside a scaled compound module???
        // if not, we should provide a correct scaling factor
//             float scale = ((CompoundModuleNodeEx)parent).getDisplayString().getScale();

        newModule.getDisplayString().setLocation(newBounds.getLocation(), scale);

        if (index < 0) {
            parent.addSubmodule(newModule);
        } else {
            parent.insertSubmodule(index, newModule);
        }

        newModule.setName(oldModule.getName());
        // make the cloned submodule's name unique within the parent
        ((SubmoduleNodeEx)newModule).makeNameUnique();

        // keep track of the new modules so we can delete them in undo
        newModules.add(newModule);
        
        // keep track of the newModule -> OldModule map so that we can properly
        // attach all connections later.
        old2newMapping.put(oldModule, newModule);
        
        return newModule;
    }

    @Override
    public void execute() {
        redo();
    }

    @Override
    public void redo() {
        old2newMapping = new HashMap<SubmoduleNodeEx, SubmoduleNodeEx>();
        newConnections = new LinkedList<ConnectionNodeEx>();
        newModules = new LinkedList<SubmoduleNodeEx>();

        for (SubmoduleNodeEx mod : modules){
            if (bounds != null && bounds.containsKey(mod)) {
                cloneModule(mod, bounds.get(mod), -1);
            } else if (indices != null && indices.containsKey(mod)) {
                cloneModule(mod, null, indices.get(mod));
            } else {
                cloneModule(mod, null, -1);
            }
        }

        // go through all modules that were previously cloned and check all the source connections
        for (SubmoduleNodeEx oldSrcMod : modules)
            for (ConnectionNodeEx oldConn : oldSrcMod.getSrcConnections()) {
            	IConnectable oldDestMod = oldConn.getDestModuleRef();
                // if the destination side was also selected clone this connection connection too 
                // TODO future: clone the connections ONLY if they are selected too
                if (old2newMapping.containsKey(oldDestMod)) {
                    SubmoduleNodeEx newSrcMod = old2newMapping.get(oldSrcMod);
                    SubmoduleNodeEx newDestMod = old2newMapping.get(oldDestMod);
                    cloneConnection(oldConn, newSrcMod, newDestMod);
                }
            }
    }

    @Override
    public void undo() {
        for (SubmoduleNodeEx mod : newModules)
            mod.removeFromParent();
        
        for (ConnectionNodeEx conn : newConnections)
            conn.removeFromParent();
    }

}