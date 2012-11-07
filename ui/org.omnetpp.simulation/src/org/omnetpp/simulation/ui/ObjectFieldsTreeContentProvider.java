package org.omnetpp.simulation.ui;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang.ObjectUtils;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.simulation.EssentialsRegistry;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.controller.Simulation;
import org.omnetpp.simulation.model.Field;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.model.cPacket;
import org.omnetpp.simulation.ui.ObjectFieldsViewer.Mode;
import org.omnetpp.simulation.ui.ObjectFieldsViewer.Ordering;

/**
 *
 * @author Andras
 */
public class ObjectFieldsTreeContentProvider implements ITreeContentProvider {
    static final List<String> CPACKET_BASE_CLASSES = Arrays.asList(new String[]{ "cObject", "cNamedObject", "cOwnedObject", "cMessage", "cPacket" });

    private Mode mode = Mode.GROUPED;
    private Ordering ordering = Ordering.NATURAL;


    /**
     * Represents an intermediate node in the tree
     */
    public static class FieldArrayElement implements IAdaptable {
        Field field;
        int index;

        public FieldArrayElement(Field field, int index) {
            this.field = field;
            this.index = index;
        }

        @Override
        @SuppressWarnings("rawtypes")
        public Object getAdapter(Class adapter) {
            // being able to adapt to cObject helps working with the selection
            Object value = field.getValues()[index];
            if (adapter.isInstance(value))
                return value;
            if (adapter.isInstance(this))
                return this;
            return null;
        }

        @Override
        public String toString() {
            return field.toString() + ":[" + index + "]";
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            return prime * field.hashCode() + index;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            FieldArrayElement other = (FieldArrayElement) obj;
            return (field == other.field || field.equals(other.field)) && index == other.index;
        }

    }

    /**
     * Represents an intermediate node in the tree
     */
    public static class FieldGroup {
        Object parent;
        String groupName;
        Field[] fields;

        public FieldGroup(Object parent, String groupName, Field[] fields) {
            this.parent = parent;
            this.groupName = groupName;
            this.fields = fields;
        }

        @Override
        public String toString() {
            return parent.toString() + "/" + groupName;
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1; // note: leave out fields[] as in equals()
            result = prime * result + ((groupName == null) ? 0 : groupName.hashCode());
            result = prime * result + ((parent == null) ? 0 : parent.hashCode());
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null || getClass() != obj.getClass())
                return false;
            // note: leave out fields[], otherwise turning sorting on/off closes the field group node
            // in the tree! (TreeViewer uses equals() to identify tree nodes)
            FieldGroup other = (FieldGroup) obj;
            if (groupName == null && other.groupName != null)
                return false;
            else if (!groupName.equals(other.groupName))
                return false;
            if (parent == null && other.parent != null)
                return false;
            else if (!parent.equals(other.parent))
                return false;
            return true;
        }
    }

    public static class Notice {
        Object parent;
        Image image;
        String message;

        public Notice(Object parent, Image image, String message) {
            this.parent = parent;
            this.image = image;
            this.message = message;
        }

        @Override
        public String toString() {
            return parent.toString() + "/" + message;
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + ((image == null) ? 0 : image.hashCode());
            result = prime * result + ((message == null) ? 0 : message.hashCode());
            result = prime * result + ((parent == null) ? 0 : parent.hashCode());
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null || getClass() != obj.getClass())
                return false;
            Notice other = (Notice) obj;
            if (!ObjectUtils.equals(image, other.image))
                return false;
            if (!ObjectUtils.equals(message, other.message))
                return false;
            if (!ObjectUtils.equals(parent, other.parent))
                return false;
            return true;
        }
    }

    public static class ErrorNotice extends Notice {
        public ErrorNotice(Object parent, String message) {
            super(parent, PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJS_ERROR_TSK), message);
        }
    }

    public static class WarningNotice extends Notice {
        public WarningNotice(Object parent, String message) {
            super(parent, PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJS_WARN_TSK), message);
        }
    }

    public static class InfoNotice extends Notice {
        public InfoNotice(Object parent, String message) {
            super(parent, PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJS_INFO_TSK), message);
        }
    }

    protected static class FieldNameComparator implements Comparator<Field> {
        @Override
        public int compare(Field o1, Field o2) {
            return o1.getName().compareTo(o2.getName());
        }
    }

    protected static class ObjectNameComparator implements Comparator<cObject> {
        @Override
        public int compare(cObject o1, cObject o2) {
            return o1.getFullName().compareTo(o2.getFullName());
        }
    }


    public ObjectFieldsTreeContentProvider() {
    }

    public Mode getMode() {
        return mode;
    }

    public void setMode(Mode mode) {
        this.mode = mode;
    }

    public Ordering getOrdering() {
        return ordering;
    }

    public void setOrdering(Ordering ordering) {
        this.ordering = ordering;
    }

    public Object[] getChildren(Object element) {
        // Note: cObject only occurs here as root, at other places it's
        // always a field value somewhere, and as such, is wrapped into
        // Field or FieldArrayElement.
        try {
            if (element instanceof cObject) {
                // resolve object to its fields
                return getObjectChildren((cObject)element);
            }
            else if (element instanceof FieldGroup) {
                // resolve field group to the fields it contains
                return ((FieldGroup) element).fields;
            }
            else if (element instanceof Field && ((Field)element).isArray()) {
                // resolve array field to individual array elements
                Field field = (Field)element;
                FieldArrayElement[] result = new FieldArrayElement[field.getValues().length];
                for (int i = 0; i < result.length; i++)
                    result[i] = new FieldArrayElement(field, i);
                if (field.isCObject()) {
                    List<cObject> objsToLoad = new ArrayList<cObject>();
                    for (Object value : field.getValues()) {
                        if (value != null)
                            objsToLoad.add((cObject)value);
                    }
                    Simulation simulation = field.getOwner().getSimulation();
                    simulation.loadUnfilledObjects(objsToLoad);
                    simulation.loadFieldsOfUnfilledObjects(objsToLoad);
                }
                return result;
            }
            else if (element instanceof Field || element instanceof FieldArrayElement) {
                // resolve children of a field value
                boolean isArrayElement = element instanceof FieldArrayElement;
                Field field = !isArrayElement ? (Field)element : ((FieldArrayElement)element).field;
                Object value = !isArrayElement ? field.getValue() : field.getValues()[((FieldArrayElement)element).index];

                if (value instanceof cObject) {
                    // treat like cObject: resolve object to its fields
                    return getObjectChildren((cObject)value);
                }

                //TODO if struct, return its fields, etc
            }
            return new Object[0];
        }
        catch (CommunicationException e) {
            return new Object[] { new ErrorNotice(element, "Error loading content, try Refresh") };
        }
    }

    protected Object[] getObjectChildren(cObject object) throws CommunicationException {
        //TODO observe ordering, possibly filtering!
        if (mode == Mode.CHILDREN) {
            object.loadIfUnfilled();
            cObject[] children = object.getChildObjects();
            object.getSimulation().loadUnfilledObjects(children);
            if (ordering == Ordering.ALPHABETICAL)
                Arrays.sort(children = children.clone(), new ObjectNameComparator());
            return children;
        }
        else if (mode == Mode.FLAT) {
            object.loadFieldsIfUnfilled();
            return sortIfNeeded(Arrays.asList(object.getFields().clone())); // clone needed because sort() is destructive
        }
        else if (mode == Mode.GROUPED) {
            object.loadFieldsIfUnfilled();
            Field[] fields = object.getFields();
            Map<String,List<Field>> groups = new LinkedHashMap<String, List<Field>>();
            for (Field f : fields) {
                if (!groups.containsKey(f.getGroupProperty()))
                    groups.put(f.getGroupProperty(), new ArrayList<Field>());
                groups.get(f.getGroupProperty()).add(f);
            }
            List<Object> result = new ArrayList<Object>(groups.size());
            if (groups.containsKey(null))
                result.addAll(groups.get(null));
            for (String groupName : groups.keySet()) {
                if (groupName != null)
                    result.add(new FieldGroup(object, groupName, sortIfNeeded(groups.get(groupName))));
            }
            return result.toArray();
        }
        else if (mode == Mode.INHERITANCE) {
            //XXX this is a copypasta of the above, only groupProperty is replaced by declaredOn
            object.loadFieldsIfUnfilled();
            Field[] fields = object.getFields();
            Map<String,List<Field>> groups = new LinkedHashMap<String, List<Field>>();
            for (Field f : fields) {
                if (!groups.containsKey(f.getDeclaredOn()))
                    groups.put(f.getDeclaredOn(), new ArrayList<Field>());
                groups.get(f.getDeclaredOn()).add(f);
            }
            List<Object> result = new ArrayList<Object>(groups.size());
            if (groups.containsKey(""))
                result.addAll(groups.get(""));
            for (String groupName : groups.keySet()) {
                if (!groupName.equals(""))
                    result.add(new FieldGroup(object, groupName, sortIfNeeded(groups.get(groupName))));
            }
            return result.toArray();
        }
        else if (mode == Mode.PACKET) {
            List<Object> result = new ArrayList<Object>();
            cObject pk = object;
            while (pk instanceof cPacket) {
                pk.loadIfUnfilled();
                pk.loadFieldsIfUnfilled();

                // add its non-builtin fields
                List<Field> fields = new ArrayList<Field>();
                for (Field f : pk.getFields())
                    if (!CPACKET_BASE_CLASSES.contains(f.getDeclaredOn()))
                        fields.add(f);

                // and form a new field group from it
                result.add(new FieldGroup(object, pk.getClassName(), sortIfNeeded(fields)));

                // go down to the encapsulated packet
                pk = ((cPacket)pk).getEncapsulatedPacket();  //TODO also follow segments[] and suchlike!!!
            }
            return result.toArray();
        }
        else if (mode == Mode.ESSENTIALS) {
            EssentialsRegistry essentialsRegistry = SimulationPlugin.getDefault().getEssentialsRegistry();
            IEssentialsProvider bestProvider = essentialsRegistry.getBestEssentialsProvider(object);
            if (bestProvider != null)
                return bestProvider.getChildren(object);
            else {
                object.loadFieldsIfUnfilled();
                return sortIfNeeded(Arrays.asList(object.getFields().clone())); // fall back to FLAT mode
            }
        }
        else {
            throw new RuntimeException("unknow mode " + mode);
        }
    }

    // utility function for groupFieldsOf()
    private Field[] sortIfNeeded(List<Field> fields) {
        if (ordering == Ordering.ALPHABETICAL)
            Collections.sort(fields, new FieldNameComparator());
        return fields.toArray(new Field[]{});
    }

    @SuppressWarnings("rawtypes")
    public Object[] getElements(Object inputElement) {
        if (inputElement.getClass().isArray())
            return (Object[])inputElement;
        else if (inputElement instanceof Collection)
            return ((Collection)inputElement).toArray();
        else
            return getChildren(inputElement);  // leave out toplevel element itself (the inspected object) from the tree
    }

    public Object getParent(Object element) {
        if (element instanceof Field)
            return ((Field) element).getOwner();
        else if (element instanceof FieldArrayElement)
            return ((FieldArrayElement) element).field;
        else if (element instanceof FieldGroup)
            return ((FieldGroup) element).parent;
        else if (element instanceof Notice)
            return ((Notice) element).parent;
        else // note: cObject can only occur in this tree as root, so we must ignore cObject.getOwner() here!
            return null;
    }

    public boolean hasChildren(Object element) {
        // For collapsed tree nodes TreeViewer only calls hasChildren() but not getChildren(),
        // so it makes sense to have a proper, streamlined implementation for hasChildren().
        // Note that getChildren() preloads the content of the child objects, so a simple
        // 'return getChildren().length > 0' implementation here would do a lot of completely
        // superfluous HTTP GETs for collapsed nodes.

        // Note: to be kept consistent with getChildren()!
        try {
            if (element instanceof cObject) {
                // resolve object to its fields
                return objectHasChildren((cObject)element);
            }
            else if (element instanceof FieldGroup) {
                // resolve field group to the fields it contains
                return ((FieldGroup) element).fields.length > 0;
            }
            else if (element instanceof Field && ((Field)element).isArray()) {
                // resolve array field to individual array elements
                Field field = (Field)element;
                return field.getValues().length > 0;
            }
            else if (element instanceof Field || element instanceof FieldArrayElement) {
                // resolve children of a field value
                boolean isArrayElement = element instanceof FieldArrayElement;
                Field field = !isArrayElement ? (Field)element : ((FieldArrayElement)element).field;
                Object value = !isArrayElement ? field.getValue() : field.getValues()[((FieldArrayElement)element).index];

                if (value instanceof cObject) {
                    // treat like cObject: resolve object to its fields
                    return objectHasChildren((cObject)value);
                }

                //TODO if struct, return its fields, etc
            }
            return false;
        }
        catch (CommunicationException e) {
            return true;  // child is the error element
        }
    }

    protected boolean objectHasChildren(cObject object) throws CommunicationException {
        if (mode == Mode.CHILDREN) {
            object.loadIfUnfilled();
            return object.getChildObjects().length > 0;
        }
        else if (mode == Mode.FLAT) {
            return true; // all objects have fields, and we don't want to call loadFieldsIfUnfilled() just yet
        }
        else if (mode == Mode.GROUPED) {
            return true; // all objects have fields, and we don't want to call loadFieldsIfUnfilled() just yet
        }
        else if (mode == Mode.INHERITANCE) {
            return true; // unless it's a cObject which cannot happen...
        }
        else if (mode == Mode.PACKET) {
            cObject pk = object;
            while (pk instanceof cPacket) {
                pk.loadIfUnfilled();
                pk.loadFieldsIfUnfilled();

                // find out if it has non-builtin fields
                for (Field f : pk.getFields())
                    if (!CPACKET_BASE_CLASSES.contains(f.getDeclaredOn()))
                        return true;

                // go down to the encapsulated packet
                pk = ((cPacket)pk).getEncapsulatedPacket();  //TODO also follow segments[] and suchlike!!!
            }
            return false;
        }
        else if (mode == Mode.ESSENTIALS) {
            EssentialsRegistry essentialsRegistry = SimulationPlugin.getDefault().getEssentialsRegistry();
            IEssentialsProvider bestProvider = essentialsRegistry.getBestEssentialsProvider(object);
            if (bestProvider != null)
                return bestProvider.hasChildren(object);
            else
                return true; // as we fall back to FLAT mode
        }
        else {
            throw new RuntimeException("unknow mode " + mode);
        }
    }

    public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
        // Do nothing
    }

    public void dispose() {
        // Do nothing
    }
}
