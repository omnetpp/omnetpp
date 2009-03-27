package org.omnetpp.runtimeenv.views;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.experimental.simkernel.swig.cClassDescriptor;
import org.omnetpp.experimental.simkernel.swig.cCollectChildrenVisitor;
import org.omnetpp.experimental.simkernel.swig.cObject;
import org.omnetpp.experimental.simkernel.swig.cSimulation;

public class ObjectPropertiesView extends ViewPart {
	public static final String ID = "org.omnetpp.runtimeenv.ObjectPropertiesView";

	private TreeViewer viewer;

	class StructKey {
        long ptr;
	    cClassDescriptor desc;

	    public StructKey(long ptr, cClassDescriptor desc) {
	        this.desc = desc; this.ptr = ptr;
	    }
	}
	class GroupKey {
	    long ptr;
	    cClassDescriptor desc;
        String groupName;

        public GroupKey(long ptr, cClassDescriptor desc, String groupName) {
            this.desc = desc; this.groupName = groupName; this.ptr = ptr;
        }
	}
	class FieldKey {
	    long ptr;
	    cClassDescriptor desc;
	    int fieldID;

	    public FieldKey(long ptr, cClassDescriptor desc, int fieldID) {
            this.desc = desc; this.fieldID = fieldID; this.ptr = ptr;
        }
	}
	class ArrayElementKey {
	    long ptr;
	    cClassDescriptor desc;
	    int fieldID;
	    int index;

	    public ArrayElementKey(long ptr, cClassDescriptor desc, int fieldID, int index) {
            this.desc = desc; this.fieldID = fieldID; this.index = index; this.ptr = ptr;
        }
	}
	
	class ViewContentProvider implements ITreeContentProvider {
	    public Object[] getChildren(Object element) {
	        if (element instanceof cObject) {
	            cObject object = (cObject)element;
	            long ptr = cClassDescriptor.getCPtr(object);
                cClassDescriptor desc = cClassDescriptor.getDescriptorFor(object);
                if (desc == null) {
                    return getChildObjects(object);
                } else {
                    Object[] ungroupedFields = getFieldsInGroup(ptr, desc, "");
                    Object[] groups = getGroupKeys(ptr, desc);
                    Object[] childObjects = getChildObjects(object); //FIXME needed?
                    return ArrayUtils.addAll(ArrayUtils.addAll(ungroupedFields, groups), childObjects);
                }
	        }
            else if (element instanceof StructKey) {
                StructKey key = (StructKey)element;
                Object[] ungroupedFields = getFieldsInGroup(key.ptr, key.desc, "");
                Object[] groups = getGroupKeys(key.ptr, key.desc);
                return ArrayUtils.addAll(ungroupedFields, groups);
            }
            else if (element instanceof GroupKey) {
                GroupKey key = (GroupKey)element;
                return getFieldsInGroup(key.ptr, key.desc, key.groupName);
            }
            else if (element instanceof FieldKey) {
                FieldKey key = (FieldKey)element;
                boolean isArray = key.desc.getFieldIsArray(key.ptr, key.fieldID);
                if (isArray)
                    return getElementsInArray(key.ptr, key.desc, key.fieldID);
                boolean isCompound = key.desc.getFieldIsCompound(key.ptr, key.fieldID);
                if (isCompound)
                    return getFieldsOfCompoundField(key.ptr, key.desc, key.fieldID, 0);
            }
            else if (element instanceof ArrayElementKey) {
                ArrayElementKey key = (ArrayElementKey)element;
                return getFieldsOfCompoundField(key.ptr, key.desc, key.fieldID, key.index);
            }
	        return new Object[0];
	    }

        private Object[] getFieldsOfCompoundField(long ptr, cClassDescriptor desc, int fieldID, int index) {
            // return children of this class/struct
            long fieldPtr = desc.getFieldStructPointer(ptr, fieldID, index);
            if (fieldPtr == 0)
                return new Object[0];
            boolean isCObject = desc.getFieldIsCPolymorphic(ptr, fieldID);
            if (isCObject) {
                return getChildren(null); //FIXME cast fieldPtr to cObject
            } else {
                String fieldStructName = desc.getFieldStructName(ptr, fieldID);
                cClassDescriptor fieldDesc = cClassDescriptor.getDescriptorFor(fieldStructName);
                if (fieldDesc == null)
                    return new Object[0]; // nothing known about it
                return getChildren(new StructKey(fieldPtr, fieldDesc));
            }
        }

        private Object[] getChildObjects(cObject o) {
            cCollectChildrenVisitor visitor = new cCollectChildrenVisitor(o);
            visitor.process(o);
            int m = visitor.getArraySize();
            Object[] result2 = new Object[m];
            for (int i=0; i<m; i++)
                result2[i] = visitor.get(i);
            return result2;
        }

	    protected Object[] getElementsInArray(long ptr, cClassDescriptor desc, int fieldID) {
	        int n = desc.getArraySize(ptr, fieldID);
	        Object[] result = new Object[n];
	        for (int i=0; i<n; i++)
	            result[i] = new ArrayElementKey(ptr, desc, fieldID, i);
            return result;
        }

        public Object[] getElements(Object inputElement) {
	        return getChildren(inputElement);
	    }

	    public Object getParent(Object element) {
	        if (element instanceof cObject)
	            return ((cObject)element).getOwner();
	        return null;
	    }

	    public boolean hasChildren(Object element) {
	        if (element instanceof GroupKey)
	            return true;   
	        else if (element instanceof cObject)
	            return getChildren(element).length!=0;  //FIXME make it more efficient (this counts all children!)
	        else
	            return getChildren(element).length!=0; //FIXME make it more efficient (this counts all children!)
	    }

	    protected Object[] getGroupKeys(long ptr, cClassDescriptor desc) {
	        // collect unique group names
	        int numFields = desc.getFieldCount(ptr);
	        Object[] groupNames = new Object[0];
	        for (int i=0; i<numFields; i++) {
	            String groupName = desc.getFieldProperty(ptr, i, "group");
	            if (groupName != null && !ArrayUtils.contains(groupNames, groupName))
	                groupNames = ArrayUtils.add(groupNames, groupName);
	        }

	        // convert to GroupKey[] in-place
	        Object[] result = groupNames;
	        for (int i=0; i<result.length; i++)
	            result[i] = new GroupKey(ptr, desc, (String)result[i]);
	        return result;
	        
	    }

	    protected Object[] getFieldsInGroup(long ptr, cClassDescriptor desc, String groupName) {
            int numFields = desc.getFieldCount(ptr);
            Object[] fieldKeys = new Object[numFields]; // upper bound
            int numFieldKeys = 0;
            for (int i=0; i<numFields; i++) {
                String fieldGroupName = desc.getFieldProperty(ptr, i, "group");
                if (groupName.equals(fieldGroupName))
                    fieldKeys[numFieldKeys++] = new FieldKey(ptr, desc, i);
            }
            return ArrayUtils.subarray(fieldKeys, 0, numFieldKeys);
	    }
	    
	    public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
	        // Do nothing
	    }

	    public void dispose() {
            // Do nothing
        }
	}

	class ViewLabelProvider extends LabelProvider {
	    @Override
	    public String getText(Object element) {
	        if (element instanceof cObject) {
	            cObject obj = (cObject) element;
	            String typeName = obj.getClassName();  //XXX use opp_getobjectshorttypename
	            return obj.getFullName() + " (" + typeName + ")";
	        }
	        else if (element instanceof StructKey) {
	            StructKey key = (StructKey)element;
                return key.desc.getName();
	        }
            else if (element instanceof GroupKey) {
                GroupKey key = (GroupKey)element;
                return "<" + key.groupName + ">";
            }
            else if (element instanceof FieldKey) {
                FieldKey key = (FieldKey)element;
                return getFieldText(key.ptr, key.desc, key.fieldID, -1);
            }
            else if (element instanceof ArrayElementKey) {
                ArrayElementKey key = (ArrayElementKey)element;
                return getFieldText(key.ptr, key.desc, key.fieldID, key.index);
            }
	        return super.getText(element);
	    }
	    
        @Override
		public Image getImage(Object obj) {
			return PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJ_ELEMENT);
		}
        
        public String getFieldText(long ptr, cClassDescriptor desc, int fieldID, int index) {
            return desc.getFieldName(ptr, fieldID) + "=" + ""; //XXX copy Tcl function to do this!!! key.desc.getFieldAsString(ptr, fieldID);
/*XXX TODO:
proc getFieldNodeInfo_getFieldText {obj sd fieldid index} {

    set typename [opp_classdescriptor $obj $sd fieldtypename $fieldid]
    set isarray [opp_classdescriptor $obj $sd fieldisarray $fieldid]
    set iscompound [opp_classdescriptor $obj $sd fieldiscompound $fieldid]
    set ispoly [opp_classdescriptor $obj $sd fieldiscpolymorphic $fieldid]
    set isobject [opp_classdescriptor $obj $sd fieldiscobject $fieldid]
    set iseditable [opp_classdescriptor $obj $sd fieldiseditable $fieldid]

    # field name can be overridden with @label property
    set name [opp_classdescriptor $obj $sd fieldproperty $fieldid "label"]
    if {$name==""} {set name [opp_classdescriptor $obj $sd fieldname $fieldid]}

    # if it's an unexpanded array, return "name[size]" immediately
    if {$isarray && $index==""} {
        set size [opp_classdescriptor $obj $sd fieldarraysize $fieldid]
        return "$name\[$size\] ($typename)"
    }

    # when showing array elements, omit name and just show "[index]" instead
    if {$index!=""} {
        set name "\[$index\]"
    }

    # we'll want to print the field type, except for expanded array elements
    # (no need to repeat it, as it's printed in the "name[size]" node already)
    if {$index==""} {
        set typenametext " ($typename)"
    } else {
        set typenametext ""
    }

    # "editable" flag
    if {$iseditable} {
        set typenametext " \[...\] $typenametext"
    }

    if {$iscompound} {
        # if it's an object, try to say something about it...
        if {$ispoly} {
            set fieldobj [opp_classdescriptor $obj $sd fieldstructpointer $fieldid $index]
            if [opp_isnull $fieldobj] {return "$name = \bNULL\b$typenametext"}
            if {!$isobject || [opp_getobjectowner $fieldobj]==$obj} {
                set fieldobjname [opp_getobjectfullname $fieldobj]
            } else {
                set fieldobjname [opp_getobjectfullpath $fieldobj]
            }
            set fieldobjname [opp_getobjectfullname $fieldobj]
            set fieldobjclassname [opp_getobjectshorttypename $fieldobj]
            set fieldobjinfo [opp_getobjectinfostring $fieldobj]
            if {$fieldobjinfo!=""} {
                set fieldobjinfotext ": $fieldobjinfo"
            } else {
                set fieldobjinfotext ""
            }
            return "$name = \b($fieldobjclassname) $fieldobjname$fieldobjinfotext\b$typenametext"
        } else {
            # a value can be generated via operator<<
            if [catch {set value [opp_classdescriptor $obj $sd fieldvalue $fieldid $index]} err] {set value "<!> Error: $err"}
            if {$value==""} {
                return "$name$typenametext"
            } else {
                return "$name = \b$value\b$typenametext"
            }
        }
    } else {
        # plain field, return "name = value" text
        if [catch {set value [opp_classdescriptor $obj $sd fieldvalue $fieldid $index]} err] {set value "<!> Error: $err"}
        set enumname [opp_classdescriptor $obj $sd fieldproperty $fieldid "enum"]
        if {$enumname!=""} {
            append typename " - enum $enumname"
            set symbolicname [opp_getnameforenum $enumname $value]
            set value "$symbolicname ($value)"
        }
        #if {$typename=="string"} {set value "\"$value\""}
        if {$typename=="string"} {set value "'$value'"}
        if {$value==""} {
            return "$name$typenametext"
        } else {
            return "$name = \b$value\b$typenametext"
        }
    }
}
            
 */
            
        }
	}

	/**
	 * This is a callback that will allow us to create the viewer and initialize
	 * it.
	 */
	public void createPartControl(Composite parent) {
		viewer = new TreeViewer(parent, SWT.MULTI | SWT.H_SCROLL | SWT.V_SCROLL);
		viewer.setContentProvider(new ViewContentProvider());
		viewer.setLabelProvider(new ViewLabelProvider());
		viewer.setInput(cSimulation.getActiveSimulation());
	}

	/**
	 * Passing the focus request to the viewer's control.
	 */
	public void setFocus() {
		viewer.getControl().setFocus();
	}
}