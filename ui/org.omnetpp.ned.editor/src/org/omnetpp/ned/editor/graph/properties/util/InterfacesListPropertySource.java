package org.omnetpp.ned.editor.graph.properties.util;

import java.util.Set;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.interfaces.INedTypeNode;

/**
 * Property source to display the interface list of a component
 *
 * @author rhornig
 */
public class InterfacesListPropertySource extends NotifiedPropertySource {
    public final static String CATEGORY = "like";
    public final static String DESCRIPTION = "List of interfaces this component implements - (read only)";
    protected INedTypeNode model;
    protected PropertyDescriptor[] pdesc;

    public InterfacesListPropertySource(INedTypeNode model) {
        super(model);
        this.model = model;
    }

    @Override
    public IPropertyDescriptor[] getPropertyDescriptors() {
        Set<String> interfacesList = model.getNEDTypeInfo().getInterfaces();

        pdesc = new PropertyDescriptor[interfacesList.size()];
        int totalCount = 0;
        for (String interfaceName : interfacesList) {
            pdesc[totalCount] = new PropertyDescriptor(interfaceName, interfaceName);
            pdesc[totalCount].setCategory(CATEGORY);
            pdesc[totalCount].setDescription("Interface '" + interfaceName + "' - (read only)");
            totalCount++;
        }

        return pdesc;
    }

    @Override
    public Object getEditableValue() {
    	return StringUtils.join(model.getNEDTypeInfo().getInterfaces(), ",");
    }

    @Override
    public Object getPropertyValue(Object id) {
        return "";
    }

    @Override
    public boolean isPropertyResettable(Object id) {
        return false;
    }

    @Override
    public boolean isPropertySet(Object id) {
        return false;
    }

    @Override
    public void resetPropertyValue(Object id) {
    }

    @Override
    public void setPropertyValue(Object id, Object value) {
    }

}
