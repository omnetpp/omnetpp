package org.omnetpp.ned.editor.graph;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.TreeMap;

import org.eclipse.core.runtime.Platform;
import org.eclipse.gef.palette.CombinedTemplateCreationEntry;
import org.eclipse.gef.palette.ConnectionCreationToolEntry;
import org.eclipse.gef.palette.MarqueeToolEntry;
import org.eclipse.gef.palette.PaletteContainer;
import org.eclipse.gef.palette.PaletteDrawer;
import org.eclipse.gef.palette.PaletteGroup;
import org.eclipse.gef.palette.PaletteRoot;
import org.eclipse.gef.palette.PanningSelectionToolEntry;
import org.eclipse.gef.palette.ToolEntry;
import org.eclipse.gef.tools.MarqueeSelectionTool;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.displaymodel.IDisplayStringProvider;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.ned.editor.graph.misc.NedConnectionCreationTool;
import org.omnetpp.ned.editor.graph.misc.NedSelectionTool;
import org.omnetpp.ned.editor.graph.misc.ModelFactory;
import org.omnetpp.ned.editor.graph.properties.NedPropertySourceAdapterFactory;
import org.omnetpp.ned2.model.ChannelInterfaceNodeEx;
import org.omnetpp.ned2.model.ChannelNodeEx;
import org.omnetpp.ned2.model.CompoundModuleNodeEx;
import org.omnetpp.ned2.model.ConnectionNodeEx;
import org.omnetpp.ned2.model.INEDTypeInfo;
import org.omnetpp.ned2.model.INamed;
import org.omnetpp.ned2.model.ModuleInterfaceNodeEx;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.NEDElementUtilEx;
import org.omnetpp.ned2.model.SimpleModuleNodeEx;
import org.omnetpp.ned2.model.SubmoduleNodeEx;
import org.omnetpp.ned2.model.pojo.PropertyNode;
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

    // TODO move the palette creation out from this file
    static PaletteRoot createPalette() {
        PaletteRoot nedPalette = new PaletteRoot();
        nedPalette.add(createToolGroup(nedPalette));
        nedPalette.add(createComponentsDrawer());
        nedPalette.addAll(createSubmodulesDrawer());
        return nedPalette;
    }

    static private PaletteContainer createToolGroup(PaletteRoot root) {
        PaletteGroup controlGroup = new PaletteGroup("Tools");

        ToolEntry tool = new PanningSelectionToolEntry("Selector","Select module(s) and connections");
        tool.setToolClass(NedSelectionTool.class);
        controlGroup.add(tool);
        root.setDefaultEntry(tool);

    	MarqueeToolEntry marquee = new MarqueeToolEntry("Connection selector","Select all connections touching the marked area");
    	marquee.setToolProperty(MarqueeSelectionTool.PROPERTY_MARQUEE_BEHAVIOR, 
    			new Integer(MarqueeSelectionTool.BEHAVIOR_CONNECTIONS_TOUCHED));
    	controlGroup.add(marquee);
    	
        tool = new ConnectionCreationToolEntry(
                "Connection",
                "The connection tool can be used to connect various modules and submodules",
                new ModelFactory(ConnectionNodeEx.getStaticTagName()), 
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_CONNECTION),
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_CONNECTION,"l", null, 24)//$NON-NLS-1$
        );
        // sets the required connection tool
        tool.setToolClass(NedConnectionCreationTool.class);
        controlGroup.add(tool);
        
        return controlGroup;
    }

    static private PaletteContainer createComponentsDrawer() {

        PaletteGroup drawer = new PaletteGroup("Components");//$NON-NLS-1$

        CombinedTemplateCreationEntry combined;

        combined = new CombinedTemplateCreationEntry(
                "Simple module",
                "A simple module that can be used as a basic building block",
                new ModelFactory(SimpleModuleNodeEx.getStaticTagName(), INamed.INITIAL_NAME), 
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_SIMPLE),
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_SIMPLE,"l",null,24)
        );
        drawer.add(combined);

        combined = new CombinedTemplateCreationEntry(
                "Submodule",
                "A submodule that can be placed in any compound module",
                new ModelFactory(SubmoduleNodeEx.getStaticTagName(), INamed.INITIAL_NAME, INamed.INITIAL_NAME.toLowerCase()), 
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_SIMPLE),
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_SIMPLE,"l",null,24)
        );
        drawer.add(combined);

        combined = new CombinedTemplateCreationEntry(
                "Module",
                "A compound module that is built up from several other modules",
                new ModelFactory(CompoundModuleNodeEx.getStaticTagName(), INamed.INITIAL_NAME), 
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE),
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE,"l",null,24)
        );
        drawer.add(combined);

        combined = new CombinedTemplateCreationEntry(
                "Interface",
                "A compound module interface ",
                new ModelFactory(ModuleInterfaceNodeEx.getStaticTagName(), INamed.INITIAL_NAME), 
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE),
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE,"l",null,24)
        );
        drawer.add(combined);
        
        combined = new CombinedTemplateCreationEntry(
                "Channel",
                "A channel is a connection type",
                new ModelFactory(ChannelNodeEx.getStaticTagName(), INamed.INITIAL_NAME, INamed.INITIAL_NAME.toLowerCase()), 
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_CONNECTION),
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_CONNECTION,"l",null,24)
        );
        drawer.add(combined);

        combined = new CombinedTemplateCreationEntry(
                "Channel interface",
                "A channel interface",
                new ModelFactory(ChannelInterfaceNodeEx.getStaticTagName(), INamed.INITIAL_NAME), 
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_CONNECTION),
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_CONNECTION,"l",null,24)
        );
        drawer.add(combined);

        // TODO add other top level components (channel etc.)
        return drawer;
    }


    /**
     * creates submodule drawers using currently parsed types
     * @return
     */
    static private List<PaletteContainer> createSubmodulesDrawer() {
    	String defaultGroupName = "Submodule";
    	TreeMap<String,PaletteDrawer> containerMap = new TreeMap<String, PaletteDrawer>(); 
    	
    	// create the default 
        PaletteDrawer defDrawer 
        	= new PaletteDrawer(defaultGroupName, ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_TEMPLATE));
        containerMap.put(defaultGroupName, defDrawer);

        // get all the possible type names in alphabetical order
        List<String> typeNames 
        		= new ArrayList<String>(NEDResourcesPlugin.getNEDResources().getModuleNames());
        Collections.sort(typeNames);
        
        for(String name : typeNames) {
        	INEDTypeInfo comp = NEDResourcesPlugin.getNEDResources().getComponent(name);
        	NEDElement nedElement = comp.getNEDElement();
        	// set the default images for the palette entry
        	ImageDescriptor imageDescNorm = ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_SIMPLE,null,null,0);
        	ImageDescriptor imageDescLarge = ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_SIMPLE,"l",null,0);
        	if (nedElement instanceof IDisplayStringProvider) {
                IDisplayString dps = ((IDisplayStringProvider)nedElement).getEffectiveDisplayString();
        		String iid = dps.getAsStringDef(IDisplayString.Prop.IMAGE);
        		if (iid != null && !"".equals(iid)) {
                	imageDescNorm = ImageFactory.getDescriptor(iid,"vs",null,0);
                	imageDescLarge = ImageFactory.getDescriptor(iid,"s",null,0);
        		}
        	}
        	
        	// determine which palette group it belongs to or put it to the default
        	PropertyNode property = (PropertyNode)comp.getProperty("group"); 
        	String group = (property == null) ? defaultGroupName
        			        : NEDElementUtilEx.getPropertyValue(property);
        	
        	if (group == null || "".equals(group))
        		group = defaultGroupName;
        	
        	// get the correct drawer and create a new one if does not exists
        	PaletteDrawer currentDrawer = containerMap.get(group);
        	if (currentDrawer == null) {
        		currentDrawer = new PaletteDrawer(group, ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_TEMPLATE));
        		containerMap.put(group, currentDrawer);
        	}
        	
        	// create the tool entry
        	CombinedTemplateCreationEntry combined = new CombinedTemplateCreationEntry(
                    name, "A submodule with type "+name,
                    new ModelFactory(SubmoduleNodeEx.getStaticTagName(),name, name), 
                    imageDescNorm, imageDescLarge );
        	// add to the selected drawer
            currentDrawer.add(combined);
        }
        
        return new ArrayList<PaletteContainer>(containerMap.values());
    }
}