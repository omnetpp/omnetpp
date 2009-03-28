package org.omnetpp.runtimeenv.views;

import org.apache.commons.lang.ArrayUtils;
import org.apache.commons.lang.StringUtils;
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
import org.omnetpp.experimental.simkernel.swig.cEnum;
import org.omnetpp.experimental.simkernel.swig.cObject;
import org.omnetpp.experimental.simkernel.swig.cSimulation;
import org.omnetpp.runtimeenv.Activator;
import org.omnetpp.runtimeenv.ISimulationListener;

//XXX what's lost, compared to Tcl:
// - bold (\b)
// - editable
// - multi-line text
//TODO should follow canvas selection
//TODO should enable pinning, and creation of new View instances
public class ObjectPropertiesView extends ViewPart implements ISimulationListener {
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
                    return getFieldsOfCompoundField(key.ptr, key.desc, key.fieldID, -1);
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
                return getChildren(desc.getFieldAsCObject(ptr, fieldID, index));
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
                return "[" + key.groupName + "]";
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
		public Image getImage(Object element) {
            if (element instanceof cObject) {
                //FIXME cache image by object's classname!
                cObject object = (cObject)element;
                long ptr = cClassDescriptor.getCPtr(object);
                cClassDescriptor desc = cClassDescriptor.getDescriptorFor(object);
                String icon = desc.getProperty("icon");
                if (!StringUtils.isEmpty(icon)) {
                    return Activator.getImageDescriptor("icons/obj16/"+icon+".png").createImage(); //FIXME TODO error check, caching, look into image path, etc
                }
            }
            else if (element instanceof StructKey) {
                return Activator.getImageDescriptor("icons/obj16/field.png").createImage(); //FIXME TODO error check, caching, look into image path, etc
            }
            else if (element instanceof GroupKey) {
                return Activator.getImageDescriptor("icons/obj16/fieldgroup.png").createImage(); //FIXME TODO error check, caching, look into image path, etc
                //return PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJ_FOLDER);
            }
            else if (element instanceof FieldKey) {
                FieldKey key = (FieldKey)element;
                boolean isObject = key.desc.getFieldIsCObject(key.ptr, key.fieldID);
                if (isObject)
                    return getImage(key.desc.getFieldAsCObject(key.ptr, key.fieldID, -1));
                else
                    return Activator.getImageDescriptor("icons/obj16/field.png").createImage(); //FIXME TODO error check, caching, look into image path, etc
            }
            else if (element instanceof ArrayElementKey) {
                ArrayElementKey key = (ArrayElementKey)element;
                boolean isObject = key.desc.getFieldIsCObject(key.ptr, key.fieldID);
                if (isObject)
                    return getImage(key.desc.getFieldAsCObject(key.ptr, key.fieldID, key.index));
                else
                    return Activator.getImageDescriptor("icons/obj16/field.png").createImage(); //FIXME TODO error check, caching, look into image path, etc
            }
            
			return PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJS_ERROR_TSK);
		}
        
        public String getFieldText(long object, cClassDescriptor desc, int field, int index) {
            String typeName = desc.getFieldTypeString(object, field);
            boolean isArray = desc.getFieldIsArray(object, field);
            boolean isCompound = desc.getFieldIsCompound(object, field);
            boolean isPoly = desc.getFieldIsCPolymorphic(object, field);
            boolean isObject = desc.getFieldIsCObject(object, field);
            boolean isEditable = desc.getFieldIsEditable(object, field);
        
            // field name can be overridden with @label property
            String name = desc.getFieldProperty(object, field, "label");
            if (StringUtils.isEmpty(name))
                name = desc.getFieldName(object, field);
        
            // if it's an unexpanded array, return "name[size]" immediately
            if (isArray && index == -1) {
                int size = desc.getArraySize(object, field);
                return name + "[" + size + "] (" + typeName + ")";
            }
        
            // when showing array elements, omit name and just show "[index]" instead
            if (index != -1)
                name = "[" + index + "]";
        
            // we'll want to print the field type, except for expanded array elements
            // (no need to repeat it, as it's printed in the "name[size]" node already)
            String typeNameText = (index == -1) ? " (" + typeName + ")" : "";
        
            // "editable" flag
            if (isEditable)
                typeNameText = " [...] " + typeNameText;
        
            if (isCompound) {
                // if it's an object, try to say something about it...
                if (isPoly) {
                    cObject fieldObj = desc.getFieldAsCObject(object, field, index);
                    if (fieldObj == null)
                        return name + " = NULL" + typeNameText;
                    String fieldObjName;
                    if (!isObject || cClassDescriptor.getCPtr(fieldObj.getOwner()) == object)
                        fieldObjName = fieldObj.getFullName();
                    else
                        fieldObjName = fieldObj.getFullPath();
                    String className = fieldObj.getClassName(); //FIXME use shorttypename!!!
                    String info = fieldObj.info();
                    String infoText = info.equals("") ? "" : ": " + info;
                    return name + " = " + "(" + className + ") " + fieldObjName + infoText + typeNameText;
                } 
                else {
                    // a value was generated via operator<<
                    String value = desc.getFieldAsString(object, field, index);
                    if (value.equals(""))
                        return name + typeNameText;
                    else
                        return name + " = " + value + typeNameText;
                }
            } else {
                // plain field, return "name = value" text
                String value = desc.getFieldAsString(object, field, index);
                if (typeName.equals("string")) 
                    value = "'" + value + "'";

                String enumName = desc.getFieldProperty(object, field, "enum");
                if (!StringUtils.isEmpty(enumName)) {
                    typeNameText = typeNameText + " - enum " + enumName;
                    cEnum enumDef = cEnum.find(enumName);
                    if (enumDef != null) {
                        try {
                            String symbolicName = enumDef.getStringFor(Integer.parseInt(value));
                            value = StringUtils.defaultIfEmpty(symbolicName, "???") + " (" + value + ")";
                        } 
                        catch (NumberFormatException e) { }
                    }
                }

                if (value.equals(""))
                    return name + typeNameText;
                else
                    return name + " = " + value + typeNameText;
            }
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
        Activator.getSimulationManager().addChangeListener(this);
	}

	/**
	 * Passing the focus request to the viewer's control.
	 */
	public void setFocus() {
		viewer.getControl().setFocus();
	}

	@Override
    public void changed() {
        viewer.refresh();
    }

    @Override
    public void dispose() {
        Activator.getSimulationManager().removeChangeListener(this);
        super.dispose();
    }
}
