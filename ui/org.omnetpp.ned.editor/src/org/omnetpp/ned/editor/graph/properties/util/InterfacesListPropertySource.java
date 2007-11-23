package org.omnetpp.ned.editor.graph.properties.util;

import java.util.Set;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

/**
 * Property source to display the interface list of a component
 *
 * @author rhornig
 */
public class InterfacesListPropertySource extends NedBasePropertySource {
    public final static String CATEGORY = "like";
    public final static String DESCRIPTION = "List of interfaces this component implements - (read only)";
    protected PropertyDescriptor[] pdesc;

    public InterfacesListPropertySource(INedTypeElement model) {
        super(model);
    }

    @Override
    public IPropertyDescriptor[] getPropertyDescriptors() {
        Set<INedTypeElement> interfacesList = getNedTypeModel().getNEDTypeInfo().getInterfaces();

        pdesc = new PropertyDescriptor[interfacesList.size()];
        int totalCount = 0;
        for (INedTypeElement interfaceType : interfacesList) {
            pdesc[totalCount] = new PropertyDescriptor(interfaceType, interfaceType.getName());
            pdesc[totalCount].setCategory(CATEGORY);
            pdesc[totalCount].setDescription("Interface '" + interfaceType.getName() + "' - (read only)");
            totalCount++;
        }

        return pdesc;
    }

    @Override
    public Object getEditableValue() {
    	return StringUtils.join(getNedTypeModel().getNEDTypeInfo().getInterfaces(), ",");
    }

    public INedTypeElement getNedTypeModel() {
        return (INedTypeElement)getModel();
    }
}
