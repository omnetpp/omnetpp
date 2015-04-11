/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.notification;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.ned.model.INedElement;

/**
 * Notifies about a structural change in the model. A model element was added or removed
 * to/from the model.
 *
 * @author rhornig
 */
public class NedStructuralChangeEvent extends NedModelChangeEvent {
    public enum Type {INSERTION, REMOVAL} ;
    protected Type type;
    protected INedElement child;
    protected INedElement oldLocation;
    protected INedElement newLocation;

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
    public NedStructuralChangeEvent(INedElement source, INedElement child, Type type,
                            INedElement newLocation,INedElement oldLocation) {
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
    public INedElement getChild() {
        return child;
    }

    /**
     * Returns the location of the change. (The new child was removed before this node,
     * or was removed at the end if it's null.)
     */
    public INedElement getOldLocation() {
        return oldLocation;
    }

    /**
     * Returns the location of the change. (The new child was added before this node,
     * or was appended at the end if it's null.)
     */
    public INedElement getNewLocation() {
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
        return super.toString() + " " + type.name() + " of child: " + child.toString();
    }
}
