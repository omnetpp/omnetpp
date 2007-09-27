package org.omnetpp.ned.editor.graph.properties;

import java.util.EnumSet;

import org.omnetpp.ned.editor.graph.properties.util.DelegatingPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.DisplayPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.ExtendsListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.GateListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.MergedPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.NamePropertySource;
import org.omnetpp.ned.editor.graph.properties.util.ParameterListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.TypeNameValidator;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.ex.ModuleInterfaceElementEx;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class ModuleInterfacePropertySource extends MergedPropertySource {

    protected static class ModuleInterfaceDisplayPropertySource extends DisplayPropertySource {
        protected ModuleInterfaceElementEx model;

        public ModuleInterfaceDisplayPropertySource(ModuleInterfaceElementEx model) {
            super(model);
            this.model = model;
            supportedProperties.addAll( EnumSet.range(DisplayString.Prop.WIDTH,
                                                      DisplayString.Prop.IMAGESIZE));
        }

    }

    public ModuleInterfacePropertySource(ModuleInterfaceElementEx nodeModel) {
    	super(nodeModel);
        mergePropertySource(new NamePropertySource(nodeModel, new TypeNameValidator(nodeModel)));
        mergePropertySource(new DelegatingPropertySource(
                new ExtendsListPropertySource(nodeModel),
                ExtendsListPropertySource.CATEGORY,
                ExtendsListPropertySource.DESCRIPTION));
        mergePropertySource(new DelegatingPropertySource(
                new ParameterListPropertySource(nodeModel),
                ParameterListPropertySource.CATEGORY,
                ParameterListPropertySource.DESCRIPTION));
        mergePropertySource(new DelegatingPropertySource(
                new GateListPropertySource(nodeModel),
                GateListPropertySource.CATEGORY,
                GateListPropertySource.DESCRIPTION));
        // create a nested displayPropertySource
        mergePropertySource(new ModuleInterfaceDisplayPropertySource(nodeModel));
    }

}
