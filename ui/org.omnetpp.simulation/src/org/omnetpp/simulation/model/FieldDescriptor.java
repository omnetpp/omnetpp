package org.omnetpp.simulation.model;

import java.util.Map;

import org.eclipse.core.runtime.Assert;

/**
 *
 * @author Andras
 */
public class FieldDescriptor {
    private String name;
    private String type;
    private String declaredOn;
    private Map<String, String> properties;
    private boolean isArray;
    private boolean isCompound;
    private boolean isPointer;  //XXX likely redundant (what do we use it for?)
    private boolean isCObject;
    private boolean isCOwnedObject;
    private boolean isEditable;  // unused (we don't support editing, do we)
    private String structName;

    public FieldDescriptor() {
    }

    @SuppressWarnings("rawtypes")
    public FieldDescriptor(Map jsonField) {
        fillFromJSON(jsonField);
    }

    @SuppressWarnings({ "rawtypes", "unchecked" })
    public void fillFromJSON(Map jsonField) {
        name = (String) jsonField.get("name");
        type = (String) jsonField.get("type");
        declaredOn = (String) jsonField.get("declaredOn");
        properties = (Map<String, String>) jsonField.get("properties");  //TODO check it's really String->String
        isArray = Boolean.TRUE.equals((Boolean)jsonField.get("isArray"));
        isCompound = Boolean.TRUE.equals((Boolean)jsonField.get("isCompound"));
        isPointer = Boolean.TRUE.equals((Boolean)jsonField.get("isPointer"));
        isCObject = Boolean.TRUE.equals((Boolean)jsonField.get("isCObject"));
        isCOwnedObject = Boolean.TRUE.equals((Boolean)jsonField.get("isCOwnedObject"));
        isEditable = Boolean.TRUE.equals((Boolean)jsonField.get("isEditable"));
        structName = (String) jsonField.get("structName");
    }

    public String getName() {
        return name;
    }

    public String getType() {
        return type;
    }

    public String getDeclaredOn() {
        return declaredOn;
    }

    public Map<String, String> getProperties() {
        return properties;
    }

    public String getProperty(String name) {
        return properties.get(name);
    }

    public boolean isArray() {
        return isArray;
    }

    public boolean isCompound() {
        return isCompound;
    }

    public boolean isPointer() {
        return isPointer;
    }

    public boolean isCObject() {
        return isCObject;
    }

    public boolean isCOwnedObject() {
        return isCOwnedObject;
    }

    public boolean isEditable() {
        return isEditable;
    }

    public String getStructName() {
        return structName;
    }

    @Override
    public String toString() {
        return getDeclaredOn() + "::" + getName();
    }

    @Override
    public int hashCode() {
        Assert.isTrue(name != null && declaredOn != null);
        final int prime = 31;
        return name.hashCode() + prime * declaredOn.hashCode();
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null || getClass() != obj.getClass())
            return false;
        FieldDescriptor other = (FieldDescriptor) obj;
        Assert.isTrue(name != null && declaredOn != null);
        return name.equals(other.name) && declaredOn.equals(other.declaredOn);
    }

}