package org.omnetpp.ned.editor.graph.properties;

import org.omnetpp.ned.editor.graph.properties.util.DelegatingPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.ExtendsListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.GateListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.MergedPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.NamePropertySource;
import org.omnetpp.ned.editor.graph.properties.util.ParameterListPropertySource;
import org.omnetpp.ned.model.ex.ModuleInterfaceElementEx;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class ModuleInterfacePropertySource extends MergedPropertySource {

    public ModuleInterfacePropertySource(ModuleInterfaceElementEx nodeModel) {
    	super(nodeModel);
        mergePropertySource(new NamePropertySource(nodeModel));
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
    }

}
