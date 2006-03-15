package org.omnetpp.ned.editor.graph;

import java.util.ArrayList;
import java.util.List;

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
import org.eclipse.gef.palette.PaletteStack;
import org.eclipse.gef.palette.PanningSelectionToolEntry;
import org.eclipse.gef.palette.ToolEntry;
import org.eclipse.gef.requests.SimpleFactory;
import org.eclipse.gef.tools.MarqueeSelectionTool;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.ned.editor.graph.misc.ImageFactory;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.CompoundModuleNodeEx;
import org.omnetpp.ned.editor.graph.model.NEDElementFactoryEx;
import org.omnetpp.ned.editor.graph.model.SubmoduleNodeEx;
import org.omnetpp.ned.editor.graph.properties.NedPropertySourceAdapterFactory;
import org.omnetpp.ned2.model.NEDElement;

public class GraphicalNedEditorPlugin extends AbstractUIPlugin {

    public final static String PLUGIN_ID = "org.omnetpp.ned.editor.graph";

    private static GraphicalNedEditorPlugin singleton;
    
    public GraphicalNedEditorPlugin() {
        super();
        if (singleton == null) {
            singleton = this;
        }
        
        // initialize the model factory
   		NEDElementFactoryEx.setInstance(new NEDElementFactoryEx());
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
        PaletteRoot logicPalette = new PaletteRoot();
        logicPalette.addAll(createCategories(logicPalette));
        return logicPalette;
    }

    static private List<PaletteContainer> createCategories(PaletteRoot root) {
        List<PaletteContainer> categories = new ArrayList<PaletteContainer>();

        categories.add(createControlGroup(root));
        categories.add(createComponentsDrawer());
        categories.add(createTemplatesDrawer());
        categories.add(createImportsDrawer());

        return categories;
    }

    static private PaletteContainer createControlGroup(PaletteRoot root) {
        PaletteGroup controlGroup = new PaletteGroup(MessageFactory.LogicPlugin_Category_ControlGroup_Label);

        List<PaletteEntry> entries = new ArrayList<PaletteEntry>();

        ToolEntry tool = new PanningSelectionToolEntry();
        entries.add(tool);
        root.setDefaultEntry(tool);

    	PaletteStack marqueeStack = new PaletteStack(MessageFactory.Marquee_Stack, "", null); //$NON-NLS-1$
    	marqueeStack.add(new MarqueeToolEntry());
    	MarqueeToolEntry marquee = new MarqueeToolEntry();
    	marquee.setToolProperty(MarqueeSelectionTool.PROPERTY_MARQUEE_BEHAVIOR, 
    			new Integer(MarqueeSelectionTool.BEHAVIOR_CONNECTIONS_TOUCHED));
    	marqueeStack.add(marquee);
    	marquee = new MarqueeToolEntry();
    	marquee.setToolProperty(MarqueeSelectionTool.PROPERTY_MARQUEE_BEHAVIOR, 
    			new Integer(MarqueeSelectionTool.BEHAVIOR_CONNECTIONS_TOUCHED 
    			| MarqueeSelectionTool.BEHAVIOR_NODES_CONTAINED));
    	marqueeStack.add(marquee);
    	marqueeStack.setUserModificationPermission(PaletteEntry.PERMISSION_NO_MODIFICATION);
    	entries.add(marqueeStack);

        tool = new ConnectionCreationToolEntry(
                "Connection",
                "The connection tool can be used to connect various modules and submodules",
                null, 
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_CONNECTION),//$NON-NLS-1$
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_CONNECTION,"l", null, 24)//$NON-NLS-1$
        );
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
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_TEMPLATE)); //$NON-NLS-1$

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

    static private PaletteContainer createImportsDrawer() {
        PaletteDrawer drawer = new PaletteDrawer(
                "Imports",
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_TEMPLATE)); //$NON-NLS-1$

        List entries = new ArrayList();

//        CombinedTemplateCreationEntry combined = new CombinedTemplateCreationEntry(
//                "EthMAC",
//                "Ethernet MAC protocol module",
//                NedModelFactory.getHalfAdderFactory(),
//                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE),
//                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE,"l",null,24)
//        );
//        entries.add(combined);
//
//        combined = new CombinedTemplateCreationEntry(
//                "IPv4",
//                "Internet protocol v4 module",
//                NedModelFactory.getFullAdderFactory(),
//                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE),
//                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE,"l",null,24)
//        );
//        entries.add(combined);

        drawer.addAll(entries);
        return drawer;
    }
}