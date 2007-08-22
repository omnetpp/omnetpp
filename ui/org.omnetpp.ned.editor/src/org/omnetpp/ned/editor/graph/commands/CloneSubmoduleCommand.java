package org.omnetpp.ned.editor.graph.commands;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.NEDElementUtilEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IConnectableElement;
import org.omnetpp.ned.model.pojo.ConnectionsElement;

/**
 * Clones a set of submodules (copy operation)
 *
 * @author rhornig
 */
public class CloneSubmoduleCommand extends Command {

    private List<SubmoduleElementEx> modules, newModules;
    private List<ConnectionElementEx> newConnections;
    private CompoundModuleElementEx parent;
    private Map<SubmoduleElementEx, Rectangle> bounds;
    private Map<SubmoduleElementEx, Integer> indices;
    private Map<SubmoduleElementEx, SubmoduleElementEx> old2newMapping;
    private float scale = 1.0f;

    public CloneSubmoduleCommand(CompoundModuleElementEx parent, float scale) {
        super("Clone");
        this.scale = scale;
        this.parent = parent;
        modules = new LinkedList<SubmoduleElementEx> ();
    }

    public void addModule(SubmoduleElementEx mod, Rectangle newBounds) {
        modules.add(mod);
        if (bounds == null) {
            bounds = new HashMap<SubmoduleElementEx, Rectangle>();
        }
        bounds.put(mod, newBounds);
    }

    public void addModule(SubmoduleElementEx mod, int index) {
        modules.add(mod);
        if (indices == null) {
            indices = new HashMap<SubmoduleElementEx, Integer>();
        }
        indices.put(mod, index);
    }

    /**
     * Clone the provided connection
     * @param oldConn
     */
    protected ConnectionElementEx cloneConnection(ConnectionElementEx oldConn, SubmoduleElementEx srcModuleRef, SubmoduleElementEx destModuleRef) {

        ConnectionsElement connectionParent = null;
        if (parent instanceof CompoundModuleElementEx)
            connectionParent = parent.getFirstConnectionsChild();

        ConnectionElementEx newConn = (ConnectionElementEx)oldConn.deepDup();

        connectionParent.appendChild(newConn);
        newConn.setSrcModuleRef(srcModuleRef);
        newConn.setDestModuleRef(destModuleRef);

        newConnections.add(newConn);

        return newConn;
    }

    protected SubmoduleElementEx cloneModule(SubmoduleElementEx oldModule, Rectangle newBounds, int index) {
    	SubmoduleElementEx newModule = null;

        // duplicate the subtree but do not add to the new parent yet
        newModule = (SubmoduleElementEx)oldModule.deepDup();

        newModule.getDisplayString().setLocation(newBounds.getLocation(), scale);

        // make the cloned submodule's name unique within the parent, before inserting into the model
        // so it wont generate unnecessary notifications
        newModule.setName(NEDElementUtilEx.getUniqueNameFor(newModule, parent.getSubmodules()));

        if (index < 0) {
            parent.addSubmodule(newModule);
        }
        else {
            parent.insertSubmodule(index, newModule);
        }

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
        old2newMapping = new HashMap<SubmoduleElementEx, SubmoduleElementEx>();
        newConnections = new LinkedList<ConnectionElementEx>();
        newModules = new LinkedList<SubmoduleElementEx>();

        for (SubmoduleElementEx mod : modules){
            if (bounds != null && bounds.containsKey(mod)) {
                cloneModule(mod, bounds.get(mod), -1);
            }
            else if (indices != null && indices.containsKey(mod)) {
                cloneModule(mod, null, indices.get(mod));
            }
            else {
                cloneModule(mod, null, -1);
            }
        }

        // go through all modules that were previously cloned and check all the source connections
        for (SubmoduleElementEx oldSrcMod : modules)
            for (ConnectionElementEx oldConn : oldSrcMod.getSrcConnections()) {
            	IConnectableElement oldDestMod = oldConn.getDestModuleRef();
                // if the destination side was also selected clone this connection connection too
                // TODO future: clone the connections ONLY if they are selected too
                if (old2newMapping.containsKey(oldDestMod)) {
                    SubmoduleElementEx newSrcMod = old2newMapping.get(oldSrcMod);
                    SubmoduleElementEx newDestMod = old2newMapping.get(oldDestMod);
                    cloneConnection(oldConn, newSrcMod, newDestMod);
                }
            }
    }

    @Override
    public void undo() {
        for (SubmoduleElementEx mod : newModules)
            mod.removeFromParent();

        for (ConnectionElementEx conn : newConnections)
            conn.removeFromParent();
    }

}