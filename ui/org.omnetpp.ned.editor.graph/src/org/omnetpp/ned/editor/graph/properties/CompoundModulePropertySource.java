package org.omnetpp.ned.editor.graph.properties;

import java.util.EnumSet;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.ned2.model.CompoundModuleNodeEx;

public class CompoundModulePropertySource extends DelegatingPropertySource {
    // compound module display properties
    private static class CompoundModuleDisplayPropertySource extends DisplayPropertySource {
        protected CompoundModuleNodeEx model;

        public CompoundModuleDisplayPropertySource(CompoundModuleNodeEx model) {
            super(model);
            this.model = model;
            setDisplayString(model.getDisplayString());
            supportedProperties.addAll(EnumSet.range(DisplayString.Prop.X, 
                                                	 DisplayString.Prop.MODULE_UNIT));
        }

        @Override
        public void modelChanged() {
            if(model != null)
                setDisplayString(model.getDisplayString());
        }
    }
    
    // constructor
    public CompoundModulePropertySource(CompoundModuleNodeEx compModuleNodeModel) {
        super(compModuleNodeModel);
        // create a nested displayPropertySource
        addPropertySource(new NamePropertySource(compModuleNodeModel));
        addPropertySource(new CompoundModuleDisplayPropertySource(compModuleNodeModel));
    }

}
