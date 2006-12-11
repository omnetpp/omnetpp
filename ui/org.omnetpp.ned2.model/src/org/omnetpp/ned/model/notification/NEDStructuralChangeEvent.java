package org.omnetpp.ned.model.notification;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.ned.model.NEDElement;

/**
 * @author rhornig
 * Notifies about a structural change in the model. A model element was added or removed
 * to/from the model.
 */
public class NEDStructuralChangeEvent extends NEDModelEvent {
    public enum Type {INSERTION, REMOVAL} ;
    protected Type type;
    protected NEDElement child;
    protected NEDElement oldLocation;
    protected NEDElement newLocation;

    /**
     * @param source The model element causing the event
     * @param child The new child node currently inserted/removed
     * @param type The type of the event
     * @param newLocation The (child) node BEFORE the newly inseted/moved node 
     *        (NULL means appended at the end)
     * @param oldLocation The (child) node BEFORE the old removed/moved node 
     *        (NULL means removed at the end)
     */
    public NEDStructuralChangeEvent(NEDElement source, NEDElement child, Type type, 
                            NEDElement newLocation,NEDElement oldLocation) {
        super(source);
        Assert.isNotNull(child);
        Assert.isNotNull(type);
        this.child = child;
        this.type = type;
        this.newLocation = newLocation;
        this.oldLocation = oldLocation;
    }

    /**
     * @return The new child node that was inserted
     */
    public NEDElement getChild() {
        return child;
    }

    /**
     * @return The location of the change (The new child was removed before this node)
     *         (or was removed at the end if it's NULL)
     */
    public NEDElement getOldLocation() {
        return oldLocation;
    }

    /**
     * @return The location of the change (The new child was added before this node)
     *         (or was appended at the end if it's NULL)
     */
    public NEDElement getNewLocation() {
        return newLocation;
    }

    /**
     * @return The type of this change. 
     */
    public Type getType() {
        return type;
    }
    
    @Override
    public String toString() {
        String nameString = child.getAttribute("name");
        return super.toString()+" "+type.name()+" of child: "+child.getTagName() +
                    ((nameString != null) ? " "+nameString : "");
    }
}
