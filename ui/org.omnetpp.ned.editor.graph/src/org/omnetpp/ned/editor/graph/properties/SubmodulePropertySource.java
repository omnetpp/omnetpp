package org.omnetpp.ned.editor.graph.properties;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.omnetpp.ned.editor.graph.model.SubmoduleNodeEx;
import org.omnetpp.ned2.model.NEDElement;

public class SubmodulePropertySource extends AbstractNedPropertySource {

    public static String PROP_NAME = "name"; //$NON-NLS-1$
    public static String PROP_TYPE = "type"; //$NON-NLS-1$
    public static String PROP_DISPLAY = "display"; //$NON-NLS-1$
    protected static IPropertyDescriptor[] descriptors;
    
    static {
        PropertyDescriptor nameProp = new TextPropertyDescriptor(PROP_NAME, "Name");
        PropertyDescriptor typeProp = new TextPropertyDescriptor(PROP_TYPE, "Type");
        PropertyDescriptor displayProp = new TextPropertyDescriptor(PROP_DISPLAY, "Display");
        descriptors = new IPropertyDescriptor[] { nameProp, typeProp, displayProp };
    }

    protected SubmoduleDisplayPropertySource submoduleDisplayPropertySource;
    
    public SubmodulePropertySource(NEDElement model) {
        super(model);
        // create a nested displayPropertySource source for the model
        submoduleDisplayPropertySource = 
            new SubmoduleDisplayPropertySource((SubmoduleNodeEx)model);
    }

    public Object getEditableValue() {
        // maybe we should create a copy from the model object here??? 
        return nedModel;
    }

    public IPropertyDescriptor[] getPropertyDescriptors() {
        return descriptors;
    }

    public Object getPropertyValue(Object propName) {
        if (PROP_NAME.equals(propName)) { 
            return ((SubmoduleNodeEx)nedModel).getName(); 
        }
        if (PROP_TYPE.equals(propName)) { 
            return ((SubmoduleNodeEx)nedModel).getType(); 
        }
        if (PROP_DISPLAY.equals(propName)) { 
            return submoduleDisplayPropertySource; 
        }
        return null;
    }

    public boolean isPropertySet(Object propName) {
        return PROP_NAME.equals(propName) || PROP_TYPE.equals(propName);
    }

    public void resetPropertyValue(Object propName) {
    }

    public void setPropertyValue(Object propName, Object value) {
        SubmoduleNodeEx subModule = ((SubmoduleNodeEx)nedModel); 
        if (PROP_NAME.equals(propName)) {
            subModule.setName(value.toString());
        }
        if (PROP_TYPE.equals(propName)) {
            subModule.setType(value.toString());
        }
        if (PROP_DISPLAY.equals(propName)) {
            subModule.setDisplayString(value.toString());
        }
    }

    @Override
    public boolean isPropertyResettable(Object id) {
        return true;
    }

    @Override
    public void modelChanged() {
        // NO need to implement a listener because we do not cache any data 
        // from the model
        // the displayStringPropertySource listens for modelchanges on its own
    }

}
