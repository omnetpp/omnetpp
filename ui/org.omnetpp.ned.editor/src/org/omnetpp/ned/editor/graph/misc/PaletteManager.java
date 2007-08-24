package org.omnetpp.ned.editor.graph.misc;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.TreeMap;

import org.eclipse.gef.palette.*;
import org.eclipse.gef.tools.MarqueeSelectionTool;
import org.eclipse.jface.resource.ImageDescriptor;

import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.util.DelayedJob;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.graph.GraphicalNedEditor;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.ChannelInterfaceElementEx;
import org.omnetpp.ned.model.ex.ChannelElementEx;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.ModuleInterfaceElementEx;
import org.omnetpp.ned.model.ex.NEDElementUtilEx;
import org.omnetpp.ned.model.ex.SimpleModuleElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IHasDisplayString;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.notification.INEDChangeListener;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.pojo.ChannelInterfaceElement;
import org.omnetpp.ned.model.pojo.ModuleInterfaceElement;
import org.omnetpp.ned.model.pojo.PropertyElement;

/**
 * Responsible for managing palette entries and keeping them in sync with
 * the components in NEDResources plugin
 *
 * @author rhornig
 */
// TODO currently the whole palette is rebuilt on each model change
// this is not necessary. Detect the changes in the palette and add/remove/modify only
// entries which really need refresh
public class PaletteManager implements INEDChangeListener {
    private static final String NBSP = "\u00A0";
    private static final String GROUP_PROPERTY = "group";
    private static final String DEFAULT_SUBMODULE_GROUP_NAME = "Submodules";

    protected GraphicalNedEditor hostingEditor;
    protected PaletteRoot nedPalette = new PaletteRoot();
    protected PaletteContainer componentsDrawer;
    protected DelayedJob paletteUpdaterJob = new DelayedJob(200) {
        public void run() {
            buildPalette();
        }
    };

    public PaletteManager(GraphicalNedEditor hostingEditor) {
        super();
        this.hostingEditor = hostingEditor;
        componentsDrawer = createComponentsDrawer();
        buildPalette();
    }

    /* (non-Javadoc)
     * @see org.omnetpp.ned.model.notification.INEDChangeListener#modelChanged(org.omnetpp.ned.model.notification.NEDModelEvent)
     * Called when a change occurred in the module which forces palette redraw
     * NOTE: this notification can arrive in any thread (even in a background thread)
     */
    public void modelChanged(NEDModelEvent event) {
        paletteUpdaterJob.restartTimer();
    }

    public PaletteRoot getRootPalette() {
        return nedPalette;
    }

    /**
     * Builds the palette (all drawers)
     */
    public void buildPalette() {
        nedPalette.getChildren().clear();
        nedPalette.add(createToolsDrawer(nedPalette));
        nedPalette.add(componentsDrawer);
        nedPalette.addAll(createSubmodulesDrawer());
    }

    /**
     * Creates several submodule drawers using currently parsed types,
     * and using the GROUP property as the drawer name.
     */
    private static List<PaletteContainer> createSubmodulesDrawer() {
        TreeMap<String,PaletteDrawer> containerMap = new TreeMap<String, PaletteDrawer>();

//        // create the default
//        PaletteDrawer defDrawer
//            = new PaletteDrawer(DEFAULT_SUBMODULE_GROUP_NAME, ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_FOLDER));
//        containerMap.put(DEFAULT_SUBMODULE_GROUP_NAME, defDrawer);

        // get all the possible type names in alphabetical order
        List<String> typeNames
                = new ArrayList<String>(NEDResourcesPlugin.getNEDResources().getModuleNames());
        typeNames.addAll(NEDResourcesPlugin.getNEDResources().getModuleInterfaceNames());
        Collections.sort(typeNames, StringUtils.dictionaryComparator);

        for (String name : typeNames) {
            INEDTypeInfo comp = NEDResourcesPlugin.getNEDResources().getComponent(name);
            INEDElement modelElement = comp.getNEDElement();
            boolean isInterface = modelElement instanceof ModuleInterfaceElement;

            // skip this type if it is a top level network
            if (modelElement instanceof CompoundModuleElementEx &&
                    ((CompoundModuleElementEx)modelElement).getIsNetwork()) {
                continue;
            }

            // set the default images for the palette entry
            ImageDescriptor imageDescNorm = ImageFactory.getDescriptor(ImageFactory.DEFAULT,"vs",null,0);
            ImageDescriptor imageDescLarge = ImageFactory.getDescriptor(ImageFactory.DEFAULT,"s",null,0);
            if (modelElement instanceof IHasDisplayString) {
                IDisplayString dps = ((IHasDisplayString)modelElement).getDisplayString();
                String iid = dps.getAsString(IDisplayString.Prop.IMAGE);
                if (iid != null && !"".equals(iid)) {
                    imageDescNorm = ImageFactory.getDescriptor(iid,"vs",null,0);
                    imageDescLarge = ImageFactory.getDescriptor(iid,"s",null,0);
                }
            }

            // determine which palette group it belongs to or put it to the default
            PropertyElement property = comp.getProperties().get(GROUP_PROPERTY);
            String group = property == null ? DEFAULT_SUBMODULE_GROUP_NAME
                            : NEDElementUtilEx.getPropertyValue(property);

            group = StringUtils.defaultIfEmpty(group, DEFAULT_SUBMODULE_GROUP_NAME);

            // get the correct drawer and create a new one if does not exists
            PaletteDrawer currentDrawer = containerMap.get(group);
            if (currentDrawer == null) {
                currentDrawer = new PaletteDrawer(group, ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_FOLDER));
                containerMap.put(group, currentDrawer);
            }

            // create the tool entry (if we are currently dropping an interface, we should use the IF type for the like parameter
            CombinedTemplateCreationEntry combined = new CombinedTemplateCreationEntry(
                    name + (isInterface ? NBSP+"(interface)" : ""), StringUtils.makeBriefDocu(comp.getNEDElement().getComment(), 300),
                    new ModelFactory(SubmoduleElementEx.getStaticTagName(),StringUtils.toInstanceName(name), name, isInterface),
                    imageDescNorm, imageDescLarge );
            // add to the selected drawer
            currentDrawer.add(combined);
        }

        return new ArrayList<PaletteContainer>(containerMap.values());
    }

    private static PaletteStack createChannelsStack() {
        PaletteStack connectionStack = new PaletteStack("Connections","Connect modules using this tool",ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_CONNECTION));

        ConnectionCreationToolEntry defConnTool = new ConnectionCreationToolEntry(
                "Connection",
                "Create connections between submodules, or submodule and parent module",
                new ModelFactory(ConnectionElementEx.getStaticTagName()),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_CONNECTION),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_CONNECTION)
        );
        // sets the required connection tool
        defConnTool.setToolClass(NedConnectionCreationTool.class);
        connectionStack.add(defConnTool);

        // get all the possible type names in alphabetical order
        List<String> channelNames
                = new ArrayList<String>(NEDResourcesPlugin.getNEDResources().getChannelNames());
        channelNames.addAll(NEDResourcesPlugin.getNEDResources().getChannelInterfaceNames());
        Collections.sort(channelNames, StringUtils.dictionaryComparator);

        for (String name : channelNames) {
            INEDTypeInfo comp = NEDResourcesPlugin.getNEDResources().getComponent(name);
            INEDElement modelElement = comp.getNEDElement();
            boolean isInterface = modelElement instanceof ChannelInterfaceElement;

            ConnectionCreationToolEntry tool
               = new ConnectionCreationToolEntry(
                    name + (isInterface ? NBSP+"(interface)" : ""),
                    StringUtils.makeBriefDocu(comp.getNEDElement().getComment(), 300),
                    new ModelFactory(ConnectionElementEx.getStaticTagName(),name.toLowerCase(), name, isInterface),
                    ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_CONNECTION),
                    ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_CONNECTION)
            );
            // sets the required connection tool
            tool.setToolClass(NedConnectionCreationTool.class);
            connectionStack.add(tool);
        }
        return connectionStack;
    }

    /**
     * Builds a drawer containing basic tools like selection connection etc.
     */
    private PaletteContainer createToolsDrawer(PaletteRoot root) {
        PaletteGroup controlGroup = new PaletteGroup("Tools");

        ToolEntry tool = new PanningSelectionToolEntry("Selector","Select module(s) and connections");
        tool.setToolClass(NedSelectionTool.class);
        controlGroup.add(tool);
        root.setDefaultEntry(tool);

        MarqueeToolEntry marquee = new MarqueeToolEntry("Connection"+NBSP+"selector","Drag out an area to select connections in it");
        marquee.setToolProperty(MarqueeSelectionTool.PROPERTY_MARQUEE_BEHAVIOR,
                new Integer(MarqueeSelectionTool.BEHAVIOR_CONNECTIONS_TOUCHED));
        controlGroup.add(marquee);

        controlGroup.add(createChannelsStack());

        return controlGroup;
    }

    /**
     * Builds a drawer containing base NED components without specifying types like
     * simple and compound modules
     */
    private static PaletteContainer createComponentsDrawer() {

        PaletteGroup drawer = new PaletteGroup("Components");

        CombinedTemplateCreationEntry combined;

        combined = new CombinedTemplateCreationEntry(
        		"Submodule",
        		"Create a submodule in a compound module",
        		new ModelFactory(SubmoduleElementEx.getStaticTagName(), SubmoduleElementEx.DEFAULT_NAME, SubmoduleElementEx.DEFAULT_TYPE),
        		ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_SUBMODULE),
        		ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_SUBMODULE)
        );
        drawer.add(combined);

        combined = new CombinedTemplateCreationEntry(
                "Simple"+NBSP+"module",
                "Create a simple module type",
                new ModelFactory(SimpleModuleElementEx.getStaticTagName(), IHasName.DEFAULT_TYPE_NAME),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_SIMPLEMODULE),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_SIMPLEMODULE)
        );
        drawer.add(combined);

        combined = new CombinedTemplateCreationEntry(
                "Compound"+NBSP+"Module",
                "Create a compound module type that may contain submodules",
                new ModelFactory(CompoundModuleElementEx.getStaticTagName(), IHasName.DEFAULT_TYPE_NAME),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_COMPOUNDMODULE),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_COMPOUNDMODULE)
        );
        drawer.add(combined);

        combined = new CombinedTemplateCreationEntry(
                "Channel",
                "Create a channel type",
                new ModelFactory(ChannelElementEx.getStaticTagName(), IHasName.DEFAULT_TYPE_NAME),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_CHANNEL),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_CHANNEL)
        );
        drawer.add(combined);

        combined = new CombinedTemplateCreationEntry(
        		"Module"+NBSP+"Interface",
        		"Create a module interface type",
        		new ModelFactory(ModuleInterfaceElementEx.getStaticTagName(), IHasName.DEFAULT_TYPE_NAME),
        		ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_INTERFACE),
        		ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_INTERFACE)
        );
        drawer.add(combined);

        combined = new CombinedTemplateCreationEntry(
                "Channel"+NBSP+"Interface",
                "Create a channel interface type",
                new ModelFactory(ChannelInterfaceElementEx.getStaticTagName(), IHasName.DEFAULT_TYPE_NAME),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_CHANNELINTERFACE),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_CHANNELINTERFACE)
        );
        drawer.add(combined);

        return drawer;
    }
}
