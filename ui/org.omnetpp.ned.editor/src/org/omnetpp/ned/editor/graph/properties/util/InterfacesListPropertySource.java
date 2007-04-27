package org.omnetpp.ned.editor.graph.properties.util;

import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IHasInterfaces;
import org.omnetpp.ned.model.pojo.InterfaceNameNode;

/**
 * Property source to display whis components are implemented
 * @author rhornig
 */
public class InterfacesListPropertySource extends NotifiedPropertySource {
    public final static String CATEGORY = "like";
    public final static String DESCRIPTION = "List of componets this component implements - (read only)";
    protected IHasInterfaces model;
    protected PropertyDescriptor[] pdesc;

    public InterfacesListPropertySource(IHasInterfaces model) {
        super(model);
        this.model = model;
    }

    @Override
    public IPropertyDescriptor[] getPropertyDescriptors() {
        List<InterfaceNameNode> interfacesList = model.getAllInterfaces();

        pdesc = new PropertyDescriptor[interfacesList.size()];
        int totalCount = 0;
        for(InterfaceNameNode interfaceElement : interfacesList) {
            pdesc[totalCount] = new PropertyDescriptor(interfaceElement.getName(), interfaceElement.getName());
            pdesc[totalCount].setCategory(CATEGORY);
            pdesc[totalCount].setDescription("Component "+interfaceElement.getName()+" - (read only)");
            totalCount++;
        }

        return pdesc;
    }

    @Override
    public Object getEditableValue() {
        StringBuilder summary = new StringBuilder("");

        for(InterfaceNameNode interfaceElement : model.getAllInterfaces())
            summary.append(interfaceElement.getName()+",");

        // strip the trailing ',' char
        summary.setLength(Math.max(summary.length()-1, 0));

        return summary;
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
