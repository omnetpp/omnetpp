package org.omnetpp.ned.editor.graph.properties;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.omnetpp.ned.editor.graph.model.SubmoduleNodeEx;

public class SubmoduleDisplayPropertySource extends DisplayPropertySource {
    protected static IPropertyDescriptor[] propertyDescArray;
    protected SubmoduleNodeEx model;


    public SubmoduleDisplayPropertySource(SubmoduleNodeEx model) {
        super(model);
        this.model = model;
        setDisplayString(model.getDisplayString());
    }

    @Override
    protected void fireDisplayStringChanged() {
        if(model != null)
            model.setDisplayString(getDisplayString());
    }

    @Override
    public void modelChanged() {
        if(model != null)
            setDisplayString(model.getDisplayString());
    }

}
