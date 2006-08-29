package org.omnetpp.ned.editor.graph;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Set;
import java.util.SortedSet;

import org.eclipse.core.runtime.Platform;
import org.eclipse.gef.palette.CombinedTemplateCreationEntry;
import org.eclipse.gef.palette.ConnectionCreationToolEntry;
import org.eclipse.gef.palette.MarqueeToolEntry;
import org.eclipse.gef.palette.PaletteContainer;
import org.eclipse.gef.palette.PaletteDrawer;
import org.eclipse.gef.palette.PaletteEntry;
import org.eclipse.gef.palette.PaletteGroup;
import org.eclipse.gef.palette.PaletteRoot;
import org.eclipse.gef.palette.PaletteSeparator;
import org.eclipse.gef.palette.PanningSelectionToolEntry;
import org.eclipse.gef.palette.ToolEntry;
import org.eclipse.gef.requests.SimpleFactory;
import org.eclipse.gef.tools.MarqueeSelectionTool;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.actions.QuickStartAction;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.ned.editor.graph.misc.NedConnectionCreationTool;
import org.omnetpp.ned.editor.graph.misc.NedSelectionTool;
import org.omnetpp.ned.editor.graph.misc.SubmoduleFactory;
import org.omnetpp.ned.editor.graph.properties.NedPropertySourceAdapterFactory;
import org.omnetpp.ned2.model.CompoundModuleNodeEx;
import org.omnetpp.ned2.model.ConnectionNodeEx;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.SubmoduleNodeEx;
import org.omnetpp.resources.NEDResources;
import org.omnetpp.resources.NEDResourcesPlugin;

public class GraphicalNedEditorPlugin extends AbstractUIPlugin {

    public final static String PLUGIN_ID = "org.omnetpp.ned.editor.graph";

    private static GraphicalNedEditorPlugin singleton;
    
    public GraphicalNedEditorPlugin() {
        super();
        if (singleton == null) {
            singleton = this;
        }
        
        // initialize the model's property srcModule factory
        Platform.getAdapterManager().registerAdapters(new NedPropertySourceAdapterFactory(), 
                                                      NEDElement.class);

    }

    public static GraphicalNedEditorPlugin getDefault() {
        return singleton;
    }

    public static ImageDescriptor imageDescriptorFromPlugin(String image_path) {
        return AbstractUIPlugin.imageDescriptorFromPlugin(PLUGIN_ID, image_path);
    }

    static PaletteRoot createPalette() {
        PaletteRoot nedPalette = new PaletteRoot();
        nedPalette.addAll(createCategories(nedPalette));
        return nedPalette;
    }

    static private List<PaletteContainer> createCategories(PaletteRoot root) {
        List<PaletteContainer> categories = new ArrayList<PaletteContainer>();

        categories.add(createControlGroup(root));
        categories.add(createComponentsDrawer());
        categories.add(createImportsDrawer());
        categories.add(createTemplatesDrawer());

        return categories;
    }

    static private PaletteContainer createControlGroup(PaletteRoot root) {
        PaletteGroup controlGroup = new PaletteGroup("Control group");

        List<PaletteEntry> entries = new ArrayList<PaletteEntry>();

        ToolEntry tool = new PanningSelectionToolEntry("Selector","Select module(s) and connections");
        tool.setToolClass(NedSelectionTool.class);
        entries.add(tool);
        root.setDefaultEntry(tool);

    	MarqueeToolEntry marquee = new MarqueeToolEntry("Connection selector","Select all connections touching the marked area");
    	marquee.setToolProperty(MarqueeSelectionTool.PROPERTY_MARQUEE_BEHAVIOR, 
    			new Integer(MarqueeSelectionTool.BEHAVIOR_CONNECTIONS_TOUCHED));
    	entries.add(marquee);
    	
    	
        tool = new ConnectionCreationToolEntry(
                "Connection",
                "The connection tool can be used to connect various modules and submodules",
                new SimpleFactory(ConnectionNodeEx.class), 
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_CONNECTION),
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_CONNECTION,"l", null, 24)//$NON-NLS-1$
        );
        // sets the required connection tool
        tool.setToolClass(NedConnectionCreationTool.class);
        entries.add(tool);
        
        controlGroup.addAll(entries);
        return controlGroup;
    }

    static private PaletteContainer createComponentsDrawer() {

        PaletteDrawer drawer = new PaletteDrawer("Components");//$NON-NLS-1$

        List entries = new ArrayList();

        CombinedTemplateCreationEntry combined;

        // FIXME component must be created using the correct factory
        combined = new CombinedTemplateCreationEntry(
                "Submodule",
                "A submodule that can be placed in any compound module",
                new SimpleFactory(SubmoduleNodeEx.class), 
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_SIMPLE),
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_SIMPLE,"l",null,24)
        );
        entries.add(combined);

        // FIXME component must be created using the correct factory
        combined = new CombinedTemplateCreationEntry(
                "Module",
                "A compound module that is built up from several other modules",
                new SimpleFactory(CompoundModuleNodeEx.class), 
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE),
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE,"l",null,24)
        );
        entries.add(combined);

        entries.add(new PaletteSeparator());

        drawer.addAll(entries);
        return drawer;
    }

    static private PaletteContainer createTemplatesDrawer() {
        PaletteDrawer drawer = new PaletteDrawer(
                "Templates",
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_TEMPLATE)); 

        List entries = new ArrayList();

//        CombinedTemplateCreationEntry combined = new CombinedTemplateCreationEntry(
//                "Ring",
//                "Arrange nodes in a ring topology",
//                NedModelFactory.getHalfAdderFactory(),
//                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE),
//                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE,"l",null,24) //$NON-NLS-1$
//        );
//        entries.add(combined);
//
//        combined = new CombinedTemplateCreationEntry(
//                "Mesh",
//                "Arrange nodes in a mesh topology",
//                NedModelFactory.getFullAdderFactory(),
//                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE),
//                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE,"l",null,24)
//        );
//        entries.add(combined);

        drawer.addAll(entries);
        return drawer;
    }

    /**
     * fills the import draver with currently parsed types
     * @return
     */
    static private PaletteContainer createImportsDrawer() {
        PaletteDrawer drawer = new PaletteDrawer(
                "Imports",
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_TEMPLATE)); 

        List entries = new ArrayList();
        
        List<String> typeNames 
        		= new ArrayList<String>(NEDResourcesPlugin.getNEDResources().getModuleNames());
        Collections.sort(typeNames);
        
        for(String name : typeNames) {
        	// TODO display correct icons, based on the type's default display string
        	CombinedTemplateCreationEntry combined = new CombinedTemplateCreationEntry(
                    name,
                    "A submodule with type "+name,
                    new SubmoduleFactory(name), 
                    ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_SIMPLE),
                    ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_SIMPLE,"l",null,24)
            );
            entries.add(combined);
        }
        
        drawer.addAll(entries);
        return drawer;
    }
}