package org.omnetpp.ned.editor.graph.properties.util;

import org.eclipse.jface.viewers.ICellEditorValidator;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.omnetpp.ned.model.interfaces.IHasName;

/**
 * A property source for displaying names
 *
 * @author rhornig
 */
public class NamePropertySource implements IPropertySource2 {

    protected IPropertyDescriptor[] descriptors;
    protected IHasName model;

    public enum Prop { Name }

    public NamePropertySource(IHasName namedNodeModel, ICellEditorValidator validator) {
        model = namedNodeModel;

        // set up property descriptors
        PropertyDescriptor nameProp = new TextPropertyDescriptor(Prop.Name, "name");
        nameProp.setValidator(validator);
        nameProp.setAlwaysIncompatible(true);
        nameProp.setCategory(MergedPropertySource.BASE_CATEGORY);
        nameProp.setDescription("The name of the object");
        descriptors = new IPropertyDescriptor[] { nameProp };
    }

    public Object getEditableValue() {
        // we don't need this if we don't want to embed this property source into an other propertysource
        return model.getName();
    }

    public IPropertyDescriptor[] getPropertyDescriptors() {
        return descriptors;
    }

    public Object getPropertyValue(Object propName) {
        if (Prop.Name.equals(propName))
            return model.getName();
        return null;
    }

    public void setPropertyValue(Object propName, Object value) {
        if (Prop.Name.equals(propName)) {
            model.setName(value.toString());
        }
    }

    public boolean isPropertySet(Object propName) {
        return Prop.Name.equals(propName) && !"".equals(model.getName()) && (model.getName() != null);
    }

    public void resetPropertyValue(Object propName) {
    	// TODO reset it to a default name
    }

    public boolean isPropertyResettable(Object propName) {
        return false;
    }

}
