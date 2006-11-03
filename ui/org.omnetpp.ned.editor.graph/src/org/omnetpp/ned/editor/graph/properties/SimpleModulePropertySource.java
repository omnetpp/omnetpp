package org.omnetpp.ned.editor.graph.properties;

import java.util.EnumSet;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.ned2.model.SimpleModuleNodeEx;

public class SimpleModulePropertySource extends MergedPropertySource {

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

    public SimpleModulePropertySource(SimpleModuleNodeEx nodeModel) {
        super(nodeModel);
        //create name
        mergePropertySource(new NamePropertySource(nodeModel));
        // parameter list property
        mergePropertySource(new DelegatingPropertySource(
                new ParameterListPropertySource(nodeModel),
                ParameterListPropertySource.CATEGORY,
                ParameterListPropertySource.DESCRIPTION));
        mergePropertySource(new DelegatingPropertySource(
                new GateListPropertySource(nodeModel),
                GateListPropertySource.CATEGORY,
                GateListPropertySource.DESCRIPTION));
        // create a nested displayPropertySource
        mergePropertySource(new SimpleModuleDisplayPropertySource(nodeModel));
        
    }

}
