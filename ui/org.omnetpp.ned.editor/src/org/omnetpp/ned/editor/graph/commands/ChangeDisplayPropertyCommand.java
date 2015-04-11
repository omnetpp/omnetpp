/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.core.runtime.Assert;
import org.eclipse.gef.commands.Command;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.ned.model.interfaces.IHasDisplayString;

/**
 * This command changes a property in a display string
 *
 * @author rhornig
 */
public class ChangeDisplayPropertyCommand extends Command {
    private IHasDisplayString element;
    private IDisplayString.Prop property;
    private String value;
    private String oldValue;

    public ChangeDisplayPropertyCommand(IHasDisplayString element, IDisplayString.Prop prop) {
        Assert.isNotNull(element);
        Assert.isNotNull(prop);
        this.element = element;
        this.property = prop;

        // store the old value for undo support
        oldValue = element.getDisplayString().getAsStringLocal(property);
    }

    public ChangeDisplayPropertyCommand(IHasDisplayString element, IDisplayString.Prop prop, String value) {
        this(element, prop);
        setValue(value);
    }

    /**
     * Sets the new value to be set. Must be called before execute().
     */
    public void setValue(String value) {
        this.value = value;
    }

    @Override
    public void execute() {
        redo();
    }

    @Override
    public void redo() {
        element.getDisplayString().set(property, value);
    }

    @Override
    public void undo() {
        element.getDisplayString().set(property, oldValue);
    }

    /**
     * Returns the new value
     */
    public String getValue() {
        return value;
    }

    /**
     * Returns the original value of the element's property
     */
    public String getOldValue() {
        return oldValue;
    }

    /**
     * Returns the element to be changed
     */
    public IHasDisplayString getElement() {
        return element;
    }

}
