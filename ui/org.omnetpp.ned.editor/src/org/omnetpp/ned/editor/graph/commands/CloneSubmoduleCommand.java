/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
 *--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.commands;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.eclipse.gef.commands.Command;
import org.omnetpp.common.displaymodel.RectangleF;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.NedElementFactoryEx;
import org.omnetpp.ned.model.ex.NedElementUtilEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.pojo.ConnectionGroupElement;
import org.omnetpp.ned.model.pojo.ConnectionsElement;
import org.omnetpp.ned.model.pojo.SubmodulesElement;

/**
 * Clones a set of submodules (copy operation)
 *
 * @author rhornig
 */
public class CloneSubmoduleCommand extends Command {

    private List<SubmoduleElementEx> modules, newModules;
    private List <INedElement> newConnectionsChildren;
    private ConnectionsElement newConnectionsElement; // set this only, if this element was created by the redo() function (so it must be undone)
    private SubmodulesElement newSubmodulesElement; // set this only, if this element was created by the redo() function (so it must be undone)
    private CompoundModuleElementEx parent;
    private Map<SubmoduleElementEx, RectangleF> bounds;
    private Map<SubmoduleElementEx, Integer> indices;
    private Map<String, SubmoduleElementEx> old2newMapping;

    public CloneSubmoduleCommand(CompoundModuleElementEx parent) {
        super("Clone");
        this.parent = parent;
        modules = new LinkedList<SubmoduleElementEx> ();
    }

    public void addModule(SubmoduleElementEx mod, RectangleF newBounds) {
        modules.add(mod);
        if (bounds == null)
            bounds = new HashMap<SubmoduleElementEx, RectangleF>();
        bounds.put(mod, newBounds);
    }

    public void addModule(SubmoduleElementEx mod, int index) {
        modules.add(mod);
        if (indices == null)
            indices = new HashMap<SubmoduleElementEx, Integer>();
        indices.put(mod, index);
    }

    @Override
    public boolean canExecute() {
        return modules.size() > 0;
    }

    protected SubmoduleElementEx cloneModule(SubmoduleElementEx oldModule, RectangleF newBounds, int index) {
        SubmoduleElementEx newModule = null;

        // duplicate the subtree but do not add to the new parent yet
        newModule = (SubmoduleElementEx)oldModule.deepDup();


        // make the cloned submodule's name unique within the parent, before inserting into the model
        // so it wont generate unnecessary notifications
        newModule.setName(NedElementUtilEx.getUniqueNameFor(newModule, parent.getSubmodules()));

        // check if the submodules element is missing and add it manual + store the element so undo can remove it
        if (parent.getFirstSubmodulesChild() == null)
            newSubmodulesElement = (SubmodulesElement)NedElementFactoryEx.getInstance().createElement(NedElementFactoryEx.NED_SUBMODULES, parent);

        if (index < 0)
            parent.addSubmodule(newModule);
        else
            parent.insertSubmodule(index, newModule);

        newModule.getDisplayString().setLocation(newBounds.getLocation());

        // keep track of the new modules so we can delete them in undo
        newModules.add(newModule);

        // keep track of the newModule -> OldModule map so that we can properly
        // attach all connections later.
        old2newMapping.put(oldModule.getName(), newModule);

        return newModule;
    }

    @Override
    public void execute() {
        redo();
    }

    @Override
    public void redo() {
        old2newMapping = new HashMap<String, SubmoduleElementEx>();
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

        // we have to iterate through the inheritance chain and gather all inherited connections and connection groups
        newConnectionsChildren = new ArrayList<INedElement>();
        for(INedTypeInfo nedTypeInfo : parent.getNedTypeInfo().getInheritanceChain()) {
            CompoundModuleElementEx currentModule = (CompoundModuleElementEx)nedTypeInfo.getNedElement();
            ConnectionsElement connsElement = currentModule.getFirstConnectionsChild();
            if (connsElement != null) {
                for(int i=0; i<connsElement.getNumChildren(); ++i) {
                    newConnectionsChildren.add(connsElement.getChild(i).deepDup());
                }
            }
        }

        // inserting all the elements that were found in the inheritance chain
        ConnectionsElement connectionParent = null;
        if (parent instanceof CompoundModuleElementEx)
            connectionParent = parent.getFirstConnectionsChild();
        if (connectionParent == null) // if we have to create a new element, we must store so we can undo this operation
            newConnectionsElement = connectionParent = (ConnectionsElement)NedElementFactoryEx.getInstance().createElement(NedElementFactoryEx.NED_CONNECTIONS, parent);

        for(INedElement elem : newConnectionsChildren)
            connectionParent.appendChild(elem);

        // filter out connections where one of the ends are not connected to a selected module
        // (must be called after all potential children are added to the tree,
        // because filtering may remove some of them using removeFromParent)
        filterConnections(newConnectionsChildren);

        // remove the "just created" connections element if it is empty after the filtering (nothing remained)
        if (newConnectionsElement!=null && newConnectionsElement.getNumChildren()==0) {
            newConnectionsElement.removeFromParent();
            newConnectionsElement = null;
        }
    }

    /**
     * Checks all the children of the element and throws away connections that are connected to
     * a module which is not in the selected module list. It looks also inside the connecionGroup elements
     * @return true if there is at least on element in the list that must be kept, false if
     * all connections were filtered out.
     */
    private boolean filterConnections(List<INedElement> elementList) {
        int noOfConnections = 0;
        for(INedElement child : elementList) {
            if (child instanceof ConnectionGroupElement) {
                // look into the connection groups and filter the connections there, too
                List<INedElement> connGroupList = new ArrayList<INedElement>();
                for(int i=0; i<child.getNumChildren(); ++i)
                    connGroupList.add(child.getChild(i));
                // throw away the empty groups
                if(!filterConnections(connGroupList))
                    child.removeFromParent();
            }

            if (child instanceof ConnectionElementEx) {
                noOfConnections++;
                ConnectionElementEx conn = (ConnectionElementEx)child;
                String oldSrcModule = conn.getSrcModule();
                String oldDestModule = conn.getDestModule();
                if (old2newMapping.containsKey(oldSrcModule) && old2newMapping.containsKey(oldDestModule)) {
                    // remap the connections to point to attach them to the newly created submodules
                    conn.setSrcModule(old2newMapping.get(oldSrcModule).getName());
                    conn.setDestModule(old2newMapping.get(oldDestModule).getName());
                }
                else { // otherwise we do not need this connections so we should throw out.
                    child.removeFromParent();
                    noOfConnections--;
                }
            }
            // all other elements are copied unchanged (loops, conditions)
        }
        return noOfConnections > 0;
    }

    @Override
    public void undo() {
        for (SubmoduleElementEx mod : newModules)
            mod.removeFromParent();

        for(INedElement elem : newConnectionsChildren)
            elem.removeFromParent();

        // remove the auto-created submodules or connections elements
        if (newSubmodulesElement != null) {
            newSubmodulesElement.removeFromParent();
            newSubmodulesElement = null;
        }
        if (newConnectionsElement != null) {
            newConnectionsElement.removeFromParent();
            newConnectionsElement = null;
        }
    }

}