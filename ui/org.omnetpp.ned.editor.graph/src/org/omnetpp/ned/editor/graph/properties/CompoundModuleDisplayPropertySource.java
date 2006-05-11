package org.omnetpp.ned.editor.graph.properties;

import java.util.EnumSet;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.omnetpp.ned2.model.CompoundModuleNodeEx;
import org.omnetpp.ned2.model.DisplayString;

public class CompoundModuleDisplayPropertySource extends DisplayPropertySource {
    protected static IPropertyDescriptor[] propertyDescArray;
    protected CompoundModuleNodeEx model;


    public CompoundModuleDisplayPropertySource(CompoundModuleNodeEx model) {
        super(model);
        this.model = model;
        setDisplayString(model.getDisplayString());
        supportedProperties = EnumSet.range(DisplayString.Prop.X, 
                                            DisplayString.Prop.MODULE_UNIT);
    }

    @Override
    public void modelChanged() {
        if(model != null)
            setDisplayString(model.getDisplayString());
    }

}
