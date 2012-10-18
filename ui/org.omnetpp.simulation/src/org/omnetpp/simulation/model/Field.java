package org.omnetpp.simulation.model;

import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IAdaptable;

/**
 *
 * @author Andras
 */
public class Field implements IAdaptable {
    private cObject owner;
    private int fieldId;  // index in the Field[] array returned by owner.getFields()
    private Object value;
    private Object[] values;

    public Field(cObject owner, int fieldId, Object value) {
        this.owner = owner;
        this.fieldId = fieldId;
        this.value = value;
    }

    public Field(cObject owner, int fieldId, Object[] values) {
        this.owner = owner;
        this.fieldId = fieldId;
        this.values = values;
    }

    @Override
    @SuppressWarnings("rawtypes")
    public Object getAdapter(Class adapter) {
        // being able to adapt to cObject helps working with the selection
        if (adapter.isInstance(getValue()))
            return getValue();
        if (adapter.isInstance(this))
            return this;
        return null;
    }

    public cObject getOwner() {
        return owner;
    }

    public int getFieldId() {
        return fieldId;
    }

    public FieldDescriptor getFieldDescriptor() {
        return owner.getDescriptor().getFieldDescriptor(fieldId);
    }

    public String getName() {
        return getFieldDescriptor().getName();
    }

    public String getType() {
        return getFieldDescriptor().getType();
    }

    public String getDeclaredOn() {
        return getFieldDescriptor().getDeclaredOn();
    }

    public Map<String, String> getProperties() {
        return getFieldDescriptor().getProperties();
    }

    public String getProperty(String name) {
        return getFieldDescriptor().getProperty(name);
    }

    public String getGroupProperty() {
        return getProperty("group");
    }

    public String getLabelProperty() {
        return getProperty("label");
    }

    public String getHintProperty() {
        return getProperty("hint");
    }

    public String getEnumProperty() {
        return getProperty("enum");
    }

    public boolean isArray() {
        return getFieldDescriptor().isArray();
    }

    public boolean isCompound() {
        return getFieldDescriptor().isCompound();
    }

    public boolean isPointer() {
        return getFieldDescriptor().isPointer();
    }

    public boolean isCObject() {
        return getFieldDescriptor().isCObject();
    }

    public boolean isCOwnedObject() {
        return getFieldDescriptor().isCOwnedObject();
    }

    public boolean isEditable() {
        return getFieldDescriptor().isEditable();
    }

    public String getStructName() {
        return getFieldDescriptor().getStructName();
    }

    public Object getValue() {
        return value;
    }

    public Object[] getValues() {
        return values;
    }

    public String getValueSymbolicName() {
        return "TODO"; //TODO
    }

    public String[] getValueSymbolicNames() {
        return new String[] {"TODO"}; //TODO
    }

    @Override
    public String toString() {
        return getDeclaredOn() + "::" + getName();
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        return owner.hashCode() + prime * fieldId;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null || getClass() != obj.getClass())
            return false;
        Field other = (Field) obj;
        Assert.isTrue(owner != null);
        if (owner != other.owner && !owner.equals(other.owner))
            return false;
        if (fieldId != other.fieldId)
            return false;
        return true;
    }
}