package org.omnetpp.ned.editor.graph.properties.util;

import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.ned.model.interfaces.INedTypeNode;
import org.omnetpp.ned.model.pojo.ExtendsElement;

/**
 * Property source to display all submodules for a given compound module
 *
 * @author rhornig
 */
public class ExtendsListPropertySource extends NotifiedPropertySource {
    public final static String CATEGORY = "extends";
    public final static String DESCRIPTION = "List of components this component extends - (read only)";
    protected INedTypeNode model;
    protected PropertyDescriptor[] pdesc;

    public ExtendsListPropertySource(INedTypeNode model) {
        super(model);
        this.model = model;
    }

    @Override
    public IPropertyDescriptor[] getPropertyDescriptors() {
        List<ExtendsElement> extendsList = model.getAllExtends();

        pdesc = new PropertyDescriptor[extendsList.size()];
        int totalCount = 0;
        for (ExtendsElement extendsElement : extendsList) {
            pdesc[totalCount] = new PropertyDescriptor(extendsElement.getName(), extendsElement.getName());
            pdesc[totalCount].setCategory(CATEGORY);
            pdesc[totalCount].setDescription("Component "+extendsElement.getName()+" - (read only)");
            totalCount++;
        }

        return pdesc;
    }

    @Override
    public Object getEditableValue() {
        StringBuilder summary = new StringBuilder("");

        for (ExtendsElement extendsElement : model.getAllExtends())
            summary.append(extendsElement.getName()+",");

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
