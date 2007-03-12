package org.omnetpp.ned.editor.graph;

import org.eclipse.core.runtime.Platform;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.ned.editor.graph.properties.IPropertySourceSupport;
import org.omnetpp.ned.editor.graph.properties.NedPropertySourceAdapterFactory;
import org.osgi.framework.BundleContext;

public class GraphicalNedEditorPlugin extends AbstractUIPlugin {

    public static String PLUGIN_ID;

    private static GraphicalNedEditorPlugin singleton;
    
    public GraphicalNedEditorPlugin() {
        super();
        if (singleton == null) {
            singleton = this;
        }
        // initialize the property source factory
        Platform.getAdapterManager().registerAdapters(new NedPropertySourceAdapterFactory(), 
                IPropertySourceSupport.class);
    }
    
    @Override
    public void start(BundleContext context) throws Exception {
        super.start(context);
        PLUGIN_ID = getBundle().getSymbolicName();
    }

    public static GraphicalNedEditorPlugin getDefault() {
        return singleton;
    }

}