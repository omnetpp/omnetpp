package org.omnetpp.ned.editor.graph.properties;

import java.util.EnumSet;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.ned2.model.SimpleModuleNodeEx;

public class SimpleModulePropertySource extends DelegatingPropertySource {

    protected static class SimpleModuleDisplayPropertySource extends DisplayPropertySource {
        protected SimpleModuleNodeEx model;

        public SimpleModuleDisplayPropertySource(SimpleModuleNodeEx model) {
            super(model);
            this.model = model;
            setDisplayString(model.getDisplayString());
            supportedProperties.addAll( EnumSet.range(DisplayString.Prop.WIDTH, 
            										  DisplayString.Prop.IMAGESIZE));
        }

        @Override
        public void modelChanged() {
            if(model != null)
                setDisplayString(model.getDisplayString());
        }

    }

    public SimpleModulePropertySource(SimpleModuleNodeEx simpleModuleNodeModel) {
        super(simpleModuleNodeModel);
        //create name
        addPropertySource(new NamePropertySource(simpleModuleNodeModel));
        // create a nested displayPropertySource
        addPropertySource(new SimpleModuleDisplayPropertySource(simpleModuleNodeModel));
        
    }

}
