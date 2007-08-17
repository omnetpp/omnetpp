package org.omnetpp.ned.model.notification;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.ned.model.INEDElement;

/**
 * Notifies about a structural change in the model. A model element was added or removed
 * to/from the model.
 *
 * @author rhornig
 */
public class NEDStructuralChangeEvent extends NEDModelEvent {
    public enum Type {INSERTION, REMOVAL} ;
    protected Type type;
    protected INEDElement child;
    protected INEDElement oldLocation;
    protected INEDElement newLocation;

    /**
     * Constructor.
     * 
     * @param source It is the parent of the new location or the old location for insert and delete respectively.
     * @param child The new child node currently inserted/removed
     * @param type The type of the event
     * @param newLocation The (child) node BEFORE the newly inserted/moved node
     *        (NULL means appended at the end)
     * @param oldLocation The (child) node BEFORE the old removed/moved node
     *        (NULL means removed at the end)
     */
    public NEDStructuralChangeEvent(INEDElement source, INEDElement child, Type type,
                            INEDElement newLocation,INEDElement oldLocation) {
        super(source);
        Assert.isNotNull(child);
        Assert.isNotNull(type);
        this.child = child;
        this.type = type;
        this.newLocation = newLocation;
        this.oldLocation = oldLocation;
    }

    /**
     * Returns the new child node that was inserted or removed
     */
    public INEDElement getChild() {
        return child;
    }

    /**
     * Returns the location of the change. (The new child was removed before this node,
     * or was removed at the end if it's NULL.
     */
    public INEDElement getOldLocation() {
        return oldLocation;
    }

    /**
     * Returns the location of the change. (The new child was added before this node,
     * or was appended at the end if it's NULL)
     */
    public INEDElement getNewLocation() {
        return newLocation;
    }

    /**
     * Returns the type of this change.
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
