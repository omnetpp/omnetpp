package org.omnetpp.ned.editor.graph.properties;

import java.util.ArrayList;
import java.util.Collections;
import java.util.EnumSet;
import java.util.List;

import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.graph.properties.util.DelegatingPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.DisplayPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.GateListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.MergedPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.NamePropertySource;
import org.omnetpp.ned.editor.graph.properties.util.ParameterListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.TypePropertySource;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.ex.SubmoduleNodeEx;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class SubmodulePropertySource extends MergedPropertySource {

	// submodule specific display property desc
    protected static class SubmoduleDisplayPropertySource extends DisplayPropertySource {
        protected SubmoduleNodeEx model;

        public SubmoduleDisplayPropertySource(SubmoduleNodeEx model) {
            super(model);
            this.model = model;
            setDisplayString(model.getDisplayString());
            supportedProperties.addAll( EnumSet.range(DisplayString.Prop.X,
            										  DisplayString.Prop.TOOLTIP));
        }

    }

    public SubmodulePropertySource(SubmoduleNodeEx submoduleNodeModel) {
        super(submoduleNodeModel);
        // create a nested displayPropertySource
        // name
        mergePropertySource(new NamePropertySource(submoduleNodeModel));
        // type
        mergePropertySource(new TypePropertySource(submoduleNodeModel) {
            @Override
            protected List<String> getPossibleValues() {
              List<String> moduleNames = new ArrayList<String>(NEDResourcesPlugin.getNEDResources().getModuleNames());
              Collections.sort(moduleNames);
              return moduleNames;
            }
        });
        // parameters
        mergePropertySource(new DelegatingPropertySource(
                new ParameterListPropertySource(submoduleNodeModel),
                ParameterListPropertySource.CATEGORY,
                ParameterListPropertySource.DESCRIPTION));
        // gates
        mergePropertySource(new DelegatingPropertySource(
                new GateListPropertySource(submoduleNodeModel),
                GateListPropertySource.CATEGORY,
                GateListPropertySource.DESCRIPTION));
        // display
        mergePropertySource(new SubmoduleDisplayPropertySource(submoduleNodeModel));
    }

}
