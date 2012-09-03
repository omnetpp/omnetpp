package org.omnetpp.simulation.model;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.simulation.controller.Simulation;


/**
 * Represents a cObject in the simulation.
 * @author Andras
 */
//TODO implement frozen objects!!!! freeze(), isFrozen();  references inside frozen objects: freeze them too, even if they are unfilled?
//TODO StringPool the strings, esp. Field members!
//TODO field metadata could be stored separately, in a cClassDescriptor
public class cObject {
    private Simulation simulation;
    private long objectId;
    private boolean isFilledIn = false;
    private boolean isFieldsFilledIn = false;
    private boolean isDisposed = false; //TODO deleted (~C++)

    private String className;
    private String name;
    private String fullName;
    private String fullPath;
    private String icon;   //TODO could come from the class descriptor
    private String info;
    private cObject owner;
    private cObject[] childObjects;

    private Field[] fields;
    private long lastAccessEventNumber;

    public cObject(Simulation simulation, long id) {
        this.simulation = simulation;
        this.objectId = id;
    }

    public Simulation getSimulation() {
        return simulation;
    }

    public long getObjectId() {
        return objectId;
    }

    public boolean isFilledIn() {
        return isFilledIn;
    }

    public boolean isDisposed() {
        return isDisposed;
    }

    public long getLastAccessEventNumber() {
        return lastAccessEventNumber;
    }

    // technically public, but should only be called from the Simulation class!
    @SuppressWarnings("rawtypes")
    public void fillFromJSON(Map jsonObject) {
        doFillFromJSON(jsonObject);
        isFilledIn = true;
    }

    @SuppressWarnings("rawtypes")
    protected void doFillFromJSON(Map jsonObject) {
        className = (String) jsonObject.get("className");
        name = (String) jsonObject.get("name");
        fullName = (String) jsonObject.get("fullName");
        fullPath = (String) jsonObject.get("fullPath");
        icon = (String) jsonObject.get("icon");
        info = (String) jsonObject.get("info");
        owner = simulation.getObjectByJSONRef((String) jsonObject.get("owner"));

        List jsonChildren = (List) jsonObject.get("children");
        childObjects = new cObject[jsonChildren.size()];
        for (int i = 0; i < childObjects.length; i++)
            childObjects[i] = simulation.getObjectByJSONRef((String) jsonChildren.get(i));
    }

    public void load() throws IOException {
        if (isDisposed)
            throw new InvalidSimulationObjectException("object " + getObjectId() + "-" + getClass().getSimpleName() + " is already deleted");
        simulation.loadObject(this);
    }

    public void unload() {
        System.out.println("@@@ unloading: " + toString());
        Assert.isTrue(isFilledIn && !isDisposed);
        isFilledIn = false;
        isFieldsFilledIn = false;
        clearReferences(); // clear references to other objects, to allow them to be garbage collected
    }

    protected void clearReferences() {
        owner = null;
        childObjects = null;
        fields = null;
    }

    //TODO why not introduce a loadIfNeeded() method, to spare the "if"'s???
    public void safeLoad() {
        // convenience method
        try {
            load();
        }
        catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    //TODO should load base AND fields together! (unless base is already loaded)
    public void loadFields() throws IOException {
        if (isDisposed)
            throw new InvalidSimulationObjectException("object " + getObjectId() + "-" + getClass().getSimpleName() + " is already deleted");
        simulation.loadObjectFields(this);
    }

    //TODO should load base AND fields together! (unless base is already loaded)
    public void safeLoadFields() {
        // convenience method
        try {
            loadFields();
        }
        catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    @SuppressWarnings("rawtypes")
    public void fillFieldsFromJSON(Map jsonObject) {
        List<Field> fields = new ArrayList<Field>();
        for (Object o: (List)jsonObject.get("fields")) {
            Map jfield = (Map)o;
            Field f = new Field();
            f.owner = this;
            f.name = (String) jfield.get("name");
            f.type = (String) jfield.get("type");
            f.declaredOn = (String) jfield.get("declaredOn");
            f.groupProperty = (String) jfield.get("@group"); // null for ungrouped
            f.labelProperty = (String) jfield.get("@label");
            f.hintProperty = (String) jfield.get("@hint");
            f.enumProperty = (String) jfield.get("@enum");
            f.isArray = Boolean.TRUE.equals((Boolean)jfield.get("isArray"));
            f.isCompound = Boolean.TRUE.equals((Boolean)jfield.get("isCompound"));
            f.isPointer = Boolean.TRUE.equals((Boolean)jfield.get("isPointer"));
            f.isCObject = Boolean.TRUE.equals((Boolean)jfield.get("isCObject"));
            f.isCOwnedObject = Boolean.TRUE.equals((Boolean)jfield.get("isCOwnedObject"));
            f.isEditable = Boolean.TRUE.equals((Boolean)jfield.get("isEditable"));
            f.structName = (String) jfield.get("declaredOn");
            f.value = f.isArray ? null : jfield.get("value");
            f.values = f.isArray ? ((List) jfield.get("value")).toArray() : null;

            if (f.isCObject) {
                // values are object references; resolve them
                if (!f.isArray)
                    f.value = simulation.getObjectByJSONRef((String)f.value);
                else
                    for (int i = 0; i < f.values.length; i++)
                        f.values[i] = simulation.getObjectByJSONRef((String)f.values[i]);
            }

            fields.add(f);
        }
        this.fields = fields.toArray(new Field[]{});
        isFieldsFilledIn = true;
    }

    public boolean isFieldsFilledIn() {
        return isFieldsFilledIn;
    }

    public Field[] getFields() {
        checkState();
        if (!isFieldsFilledIn)
            throw new InvalidSimulationObjectException("fields of object " + getObjectId() + "-" + getClass().getSimpleName() + " not yet filled in");
        return fields;
    }

    public void markAsDisposed() {
        isDisposed = true;
        clearReferences(); // allow other objects to be garbage collected
    }

    protected void checkState() {
        if (!isFilledIn)
            throw new InvalidSimulationObjectException("object " + getObjectId() + "-" + getClass().getSimpleName() + " is not yet filled in");
        if (isDisposed)
            throw new InvalidSimulationObjectException("object " + getObjectId() + "-" + getClass().getSimpleName() + " is already deleted");
        lastAccessEventNumber = simulation.getLastEventNumber();
    }

    public String getClassName() {
        checkState();
        return className;
    }

    public String getShortTypeName() {
        return getClassName();  // and for modules and channels, it will return the NED type name without package
    }

    public String getIcon() {
        checkState();
        return icon;
    }

    public String getName() {
        checkState();
        return name;
    }

    public String getFullName() {
        checkState();
        return fullName;
    }

    public String getFullPath() {
        checkState();
        return fullPath;
    }

    public String getInfo() {
        checkState();
        return info;
    }

    public cObject getOwner() {
        checkState();
        return owner;
    }

    public cObject[] getChildObjects() {
        checkState();
        return childObjects;
    }

    @Override
    public int hashCode() {
        return (int) (objectId ^ (objectId >>> 32)) + 31*simulation.hashCode();
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null || getClass() != obj.getClass())
            return false;
        cObject other = (cObject) obj;
        return simulation == other.simulation && objectId == other.objectId;
    }

    @Override
    public String toString() {
        if (!isFilledIn)
            return "[oid=" + objectId + ",unfilled]";
        else if (isDisposed)
            return "[oid=" + objectId + ",disposed]";
        else
            return "(" + className + ") " + fullPath + "[oid=" + objectId + "]";
    }
}
