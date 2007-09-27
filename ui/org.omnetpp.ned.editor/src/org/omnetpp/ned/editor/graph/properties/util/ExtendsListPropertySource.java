package org.omnetpp.ned.editor.graph.properties.util;

import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.pojo.ExtendsElement;

/**
 * Property source to display all submodules for a given compound module
 *
 * @author rhornig
 */
public class ExtendsListPropertySource extends NedBasePropertySource {
    public final static String CATEGORY = "extends";
    public final static String DESCRIPTION = "List of components this component extends - (read only)";
    protected PropertyDescriptor[] pdesc;

    public ExtendsListPropertySource(INedTypeElement model) {
        super(model);
    }

    @Override
    public IPropertyDescriptor[] getPropertyDescriptors() {
        List<ExtendsElement> extendsList = getNedTypeModel().getAllExtends();

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

        for (ExtendsElement extendsElement : ((INedTypeElement)getModel()).getAllExtends())
            summary.append(extendsElement.getName()+",");

        // strip the trailing ',' char
        summary.setLength(Math.max(summary.length()-1, 0));

        return summary;
    }

    public INedTypeElement getNedTypeModel() {
        return (INedTypeElement)getModel();
    }
}
