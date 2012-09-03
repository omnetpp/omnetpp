package org.omnetpp.simulation.model;

import org.eclipse.core.runtime.IAdaptable;

/**
 *
 * @author Andras
 */
//TODO: strip metadata into a cClassDescriptor class
public class Field implements IAdaptable {
    public cObject owner;
    public String name;
    public String type;
    public String declaredOn;
    public String groupProperty;  // content of @group(), or null
    public String labelProperty;  // content of @label(), or null
    public String hintProperty;  // content of @hint(), or null
    public String enumProperty;  // content of @enum(), or null
    public boolean isArray;  //XXX actually redundant (can check which one of value/values is non-null)
    public boolean isCompound;
    public boolean isPointer;  //XXX likely redundant (what do we use it for?)
    public boolean isCObject;
    public boolean isCOwnedObject; //XXX actually redundant (can do instanceof on the object)
    public boolean isEditable;  // unused (we don't support editing, do we)
    public String structName;
    public Object value;
    public Object[] values;
    public String valueSymbolicName;  //TODO
    public String[] valueSymbolicNames; //TODO

    @Override
    @SuppressWarnings("rawtypes")
    public Object getAdapter(Class adapter) {
        // being able to adapt to cObject helps working with the selection
        if (adapter.isInstance(value))
            return value;
        if (adapter.isInstance(this))
            return this;
        return null;
    }

    @Override
    public String toString() {
        return declaredOn + "::" + name;
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((name == null) ? 0 : name.hashCode());
        result = prime * result + ((owner == null) ? 0 : owner.hashCode());
        result = prime * result + ((declaredOn == null) ? 0 : declaredOn.hashCode());
//        result = prime * result + ((enumProperty == null) ? 0 : enumProperty.hashCode());
//        result = prime * result + ((groupProperty == null) ? 0 : groupProperty.hashCode());
//        result = prime * result + ((hintProperty == null) ? 0 : hintProperty.hashCode());
//        result = prime * result + (isArray ? 1231 : 1237);
//        result = prime * result + (isCObject ? 1231 : 1237);
//        result = prime * result + (isCOwnedObject ? 1231 : 1237);
//        result = prime * result + (isCompound ? 1231 : 1237);
//        result = prime * result + (isEditable ? 1231 : 1237);
//        result = prime * result + (isPointer ? 1231 : 1237);
//        result = prime * result + ((labelProperty == null) ? 0 : labelProperty.hashCode());
//        result = prime * result + ((structName == null) ? 0 : structName.hashCode());
//        result = prime * result + ((type == null) ? 0 : type.hashCode());
//        result = prime * result + ((value == null) ? 0 : value.hashCode());
//        result = prime * result + ((valueSymbolicName == null) ? 0 : valueSymbolicName.hashCode());
//        result = prime * result + Arrays.hashCode(valueSymbolicNames);
//        result = prime * result + Arrays.hashCode(values);
        return result;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        Field other = (Field) obj;

        if (name == null ? other.name != null : !name.equals(other.name))
            return false;

        if (owner == null ? other.owner != null : !owner.equals(other.owner))
            return false;

        if (declaredOn == null ? other.declaredOn != null : !declaredOn.equals(other.declaredOn))
            return false;

//        if (enumProperty == null) {
//            if (other.enumProperty != null)
//                return false;
//        }
//        else if (!enumProperty.equals(other.enumProperty))
//            return false;
//
//        if (groupProperty == null) {
//            if (other.groupProperty != null)
//                return false;
//        }
//        else if (!groupProperty.equals(other.groupProperty))
//            return false;
//
//        if (hintProperty == null) {
//            if (other.hintProperty != null)
//                return false;
//        }
//        else if (!hintProperty.equals(other.hintProperty))
//            return false;
//
//        if (isArray != other.isArray)
//            return false;
//        if (isCObject != other.isCObject)
//            return false;
//        if (isCOwnedObject != other.isCOwnedObject)
//            return false;
//        if (isCompound != other.isCompound)
//            return false;
//        if (isEditable != other.isEditable)
//            return false;
//        if (isPointer != other.isPointer)
//            return false;
//
//        if (labelProperty == null) {
//            if (other.labelProperty != null)
//                return false;
//        }
//        else if (!labelProperty.equals(other.labelProperty))
//            return false;
//
//        if (structName == null) {
//            if (other.structName != null)
//                return false;
//        }
//        else if (!structName.equals(other.structName))
//            return false;
//
//        if (type == null) {
//            if (other.type != null)
//                return false;
//        }
//        else if (!type.equals(other.type))
//            return false;
//
//        if (valueSymbolicName == null) {
//            if (other.valueSymbolicName != null)
//                return false;
//        }
//        else if (!valueSymbolicName.equals(other.valueSymbolicName))
//            return false;
//
//        if (!Arrays.equals(valueSymbolicNames, other.valueSymbolicNames))
//            return false;
//
//        if (value == null) {
//            if (other.value != null)
//                return false;
//        }
//        else if (!value.equals(other.value))
//            return false;
//
//        if (!Arrays.equals(values, other.values))
//            return false;

        return true;
    }
}