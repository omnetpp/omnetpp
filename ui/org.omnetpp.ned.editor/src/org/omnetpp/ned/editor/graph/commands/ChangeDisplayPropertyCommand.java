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
    IHasDisplayString element;
    IDisplayString.Prop property;
    String value;
    String oldValue;


    /**
     * Constructor.
     * @param element The element where display property must be set
     * @param prop
     */
    public ChangeDisplayPropertyCommand(IHasDisplayString element, IDisplayString.Prop prop) {
        Assert.isNotNull(element);
        Assert.isNotNull(prop);
        this.element = element;
        this.property = prop;
        // store the old value for undo support
        oldValue = element.getDisplayString().getAsStringLocal(property);
    }

    @Override
    public void execute() {
        redo();
    }

    @Override
    public void redo() {
        // set the new value
        element.getDisplayString().set(property, value);
    }

    @Override
    public void undo() {
        // set the old value back for undo
        element.getDisplayString().set(property, oldValue);
    }

    public String getValue() {
        return value;
    }

    /**
     * @param value The new value to be used
     */
    public void setValue(String value) {
        this.value = value;
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
