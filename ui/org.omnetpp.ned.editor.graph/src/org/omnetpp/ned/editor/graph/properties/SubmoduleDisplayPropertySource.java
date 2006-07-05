package org.omnetpp.ned.editor.graph.properties;

import java.util.EnumSet;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.ned2.model.SubmoduleNodeEx;

public class SubmoduleDisplayPropertySource extends DisplayPropertySource {
    protected static IPropertyDescriptor[] propertyDescArray;
    protected SubmoduleNodeEx model;


    public SubmoduleDisplayPropertySource(SubmoduleNodeEx model) {
        super(model);
        this.model = model;
        setDisplayString(model.getDisplayString());
        supportedProperties = EnumSet.range(DisplayString.Prop.X, 
                                            DisplayString.Prop.TOOLTIP);
    }

    @Override
    public void modelChanged() {
        if(model != null)
            setDisplayString(model.getDisplayString());
    }

}
