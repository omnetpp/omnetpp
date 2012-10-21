package org.omnetpp.simulation.ui;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.Viewer;
import org.omnetpp.simulation.model.Field;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.model.cPacket;
import org.omnetpp.simulation.model.essentials.cMessageEssentialsProvider;
import org.omnetpp.simulation.model.essentials.cModuleEssentialsProvider;
import org.omnetpp.simulation.model.essentials.cParEssentialsProvider;
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

    private static List<IEssentialsProvider> providers = new ArrayList<IEssentialsProvider>();  //FIXME take it from some global state
    static {
        providers.add(new cModuleEssentialsProvider());
        providers.add(new cMessageEssentialsProvider());
        providers.add(new cParEssentialsProvider());
    }


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
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
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
        if (element instanceof cObject) {
            // resolve object to its fields
            cObject object = (cObject)element;
            if (!object.isFilledIn())
                object.safeLoad(); //FIXME return if failed!
            if (!object.isFieldsFilledIn())
                object.safeLoadFields(); //FIXME return if failed!
            return groupFieldsOf(object);
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
                for (Object value : field.getValues()) {  //TODO replace loop with controller bulk load operation!!!
                    if (value != null) {
                        ((cObject)value).safeLoad();
                        ((cObject)value).safeLoadFields();
                    }
                }
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
                cObject object = (cObject)value;
                if (!object.isFilledIn())
                    object.safeLoad(); //FIXME return if failed!
                if (!object.isFieldsFilledIn())
                    object.safeLoadFields(); //FIXME return if failed!
                return groupFieldsOf(object);
            }

            //TODO if struct, return its fields, etc
        }
        return new Object[0];
    }

    protected Object[] groupFieldsOf(cObject object) {
        //TODO observe ordering, possibly filtering!
        if (mode == Mode.CHILDREN) {
            cObject[] children = object.getChildObjects();
            try {
                object.getSimulation().loadUnfilledObjects(children);
            }
            catch (IOException e) {
                e.printStackTrace();  //TODO how to handle exceptions properly...
            }
            if (ordering == Ordering.ALPHABETICAL)
                Arrays.sort(children = children.clone(), new ObjectNameComparator());
            return children;
        }
        else if (mode == Mode.FLAT) {
            return sortIfNeeded(Arrays.asList(object.getFields().clone())); // clone needed because sort() is destructive
        }
        else if (mode == Mode.GROUPED) {
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
            for (String groupName : groups.keySet())
                if (groupName != null)
                    result.add(new FieldGroup(object, groupName, sortIfNeeded(groups.get(groupName))));
                    return result.toArray();
        }
        else if (mode == Mode.INHERITANCE) {
            //XXX this is a copypasta of the above, only groupProperty is replaced by declaredOn
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
            for (String groupName : groups.keySet())
                if (!groupName.equals(""))
                    result.add(new FieldGroup(object, groupName, sortIfNeeded(groups.get(groupName))));
                    return result.toArray();
        }
        else if (mode == Mode.PACKET) {
            List<Object> result = new ArrayList<Object>();
            cObject pk = object;
            while (pk instanceof cPacket) {
                if (!pk.isFilledIn())
                    pk.safeLoad();
                if (!pk.isFieldsFilledIn())
                    pk.safeLoadFields();

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
            IEssentialsProvider bestProvider = getBestProvider(object);
            if (bestProvider != null)
                return bestProvider.getChildren(object);
            else
                return sortIfNeeded(Arrays.asList(object.getFields().clone())); // copypasta of FLAT mode
        }
        else {
            throw new RuntimeException("unknow mode " + mode);
        }
    }

    protected IEssentialsProvider getBestProvider(cObject object) {
        // note: we only try to call getScore() if there are at least two candidates; maybe this is not necessary, and then supports() and getScore() can be merged
        IEssentialsProvider bestProvider = null;
        int bestProviderScore = Integer.MIN_VALUE;
        for (IEssentialsProvider provider : providers) {
            if (provider.supports(object)) {
                if (bestProvider == null)
                    bestProvider = provider;
                else {
                    if (bestProviderScore == Integer.MIN_VALUE)
                        bestProviderScore = bestProvider.getScore(object);
                    int score = provider.getScore(object);
                    if (score > bestProviderScore) {
                        bestProvider = provider;
                        bestProviderScore = score;
                    }
                }
            }
        }
        return bestProvider;
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
        else // note: cObject can only occur in this tree as root, so we must ignore cObject.getOwner() here!
            return null;
    }

    public boolean hasChildren(Object element) {
        // complicated, so reduce it to getChildren()
        return getChildren(element).length > 0;
    }

    public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
        // Do nothing
    }

    public void dispose() {
        // Do nothing
    }
}
