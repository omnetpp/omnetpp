package org.omnetpp.simulation.model;

import java.io.IOException;
import java.util.List;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.simulation.controller.Simulation;


/**
 * Represents a cObject in the simulation.
 * @author Andras
 */
//TODO implement frozen objects!!!! freeze(), isFrozen();  references inside frozen objects: freeze them too, even if they are unfilled?
public class cObject {
    private Simulation simulation;
    private long objectId;
    private boolean isFilledIn = false;
    private boolean isFieldsFilledIn = false;
    private boolean isDisposed = false; //TODO deleted (~C++)

    private cClassDescriptor descriptor;
    private String className;
    private String name;
    private String fullName;
    private String fullPath;
    private String info;
    private cObject owner;
    private String icon; // the @icon property - cached here for efficiency
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
        info = (String) jsonObject.get("info");
        owner = simulation.getObjectByJSONRef((String) jsonObject.get("owner"));
        icon = (String) jsonObject.get("icon");
        descriptor = (cClassDescriptor) simulation.getObjectByJSONRef((String) jsonObject.get("descriptor"));

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
        if (!descriptor.isFilledIn())
            simulation.loadObject(descriptor); // for convenience -- will be needed for accessing the fields anyway
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
        // IMPORTANT: we MUST NOT access cClassDescriptor here, because it might not be loaded at this point!
        // We must be able to load fields without cClassDescriptor present, and have it loaded only later.

        List jsonFields = (List) jsonObject.get("fields");
        int numFields = jsonFields.size();
        boolean haveDesc = isFilledIn() && getDescriptor().isFilledIn();
        if (haveDesc)
            Assert.isTrue(numFields == getDescriptor().getFieldDescriptors().length);  // basic sanity check
        fields = new Field[numFields];

        for (int fieldId = 0; fieldId < numFields; fieldId++) {
            Map jsonField = (Map) jsonFields.get(fieldId);
            if (haveDesc)
                Assert.isTrue(getDescriptor().getFieldDescriptor(fieldId).getName().equals(jsonField.get("name")));  // basic sanity check
            boolean isObjRef = Boolean.TRUE.equals((Boolean)jsonField.get("isObjRef"));
            Field field;
            if (jsonField.containsKey("value")) {
                Object value = jsonField.get("value");
                if (isObjRef)
                    value = simulation.getObjectByJSONRef((String)value);
                field = new Field(this, fieldId, value);
            }
            else {
                Object[] values = ((List)jsonField.get("values")).toArray();
                if (isObjRef)
                    for (int i = 0; i < values.length; i++)
                        values[i] = simulation.getObjectByJSONRef((String)values[i]);
                field = new Field(this, fieldId, values);
            }
            fields[fieldId] = field;
        }

        // success
        isFieldsFilledIn = true;
    }

    public boolean isFieldsFilledIn() {
        return isFieldsFilledIn;
    }

    public Field[] getFields() {
        checkState();
        if (!isFieldsFilledIn)
            throw new InvalidSimulationObjectException("fields of object " + getObjectId() + "-" + getClass().getSimpleName() + " not yet filled in");
        if (!descriptor.isFilledIn())
            throw new RuntimeException("must load the class descriptor (cClassDescriptor) of the object to access its fields");
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
        return icon; // in theory the same as getDescriptor().getProperty("icon"), but we access it frequently, and even before class descriptor is loaded
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

    public cClassDescriptor getDescriptor() {
        checkState();
        return descriptor;
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
