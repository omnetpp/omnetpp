package org.omnetpp.ned.editor.graph.misc;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.TreeMap;

import org.eclipse.core.resources.IFile;
import org.eclipse.gef.palette.*;
import org.eclipse.gef.tools.MarqueeSelectionTool;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IFileEditorInput;

import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.util.DelayedJob;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.graph.GraphicalNedEditor;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.ChannelElementEx;
import org.omnetpp.ned.model.ex.ChannelInterfaceElementEx;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.ModuleInterfaceElementEx;
import org.omnetpp.ned.model.ex.NEDElementUtilEx;
import org.omnetpp.ned.model.ex.SimpleModuleElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IHasDisplayString;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.IModuleTypeElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
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
    private static final String LOCAL_TYPE_GROUP_NAME = "Local types";

    protected GraphicalNedEditor hostingEditor;
    protected PaletteRoot nedPalette = new PaletteRoot();
    protected PaletteContainer typesDrawer;
    protected DelayedJob paletteUpdaterJob = new DelayedJob(200) {
        public void run() {
            buildPalette();
        }
    };

    public PaletteManager(GraphicalNedEditor hostingEditor) {
        super();
        this.hostingEditor = hostingEditor;
        typesDrawer = createTypesDrawer();
        buildPalette();
    }

    /* (non-Javadoc)
     * @see org.omnetpp.ned.model.notification.INEDChangeListener#modelChanged(org.omnetpp.ned.model.notification.NEDModelEvent)
     * Called when a change occurred in the module which forces palette redraw
     * NOTE: this notification can arrive in any thread (even in a background thread)
     */
    public void modelChanged(NEDModelEvent event) {
        refresh();
    }

    public void refresh() {
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
        nedPalette.add(typesDrawer);

        IFileEditorInput feinput = ((IFileEditorInput)hostingEditor.getEditorInput());
        if (feinput != null) {
            PaletteContainer localTypeDrawer = createInnerTypesDrawer(feinput.getFile());
            if (localTypeDrawer != null && localTypeDrawer.getChildren().size()>0)
                nedPalette.add(localTypeDrawer);
        }
        List<PaletteContainer> submodulesDrawers = createSubmodulesDrawers();
        nedPalette.addAll(submodulesDrawers);
    }

    /**
     * Iterates over all top level types in a NED file and gathers all NEDTypes from all components.
     * Returns a Container containing all types in this file or NULL if there are no inner types
     * defined in this file's top level modules.
     */
    private static PaletteContainer createInnerTypesDrawer(IFile file) {
        PaletteContainer drawer = new PaletteDrawer(LOCAL_TYPE_GROUP_NAME, ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_FOLDER));
        for (INEDElement topLevelElement : NEDResourcesPlugin.getNEDResources().getNEDFileModel(file)) {
            // skip non NedType elements
            if (!(topLevelElement instanceof INedTypeElement))
                continue;

            // check all inner types for this element
            for (INedTypeElement typeElement : ((INedTypeElement)topLevelElement).getNEDTypeInfo().getInnerTypes().values()) {
                // skip if it cannot be used to create a submodule (ie. should be Module a module type)
                if (!(typeElement instanceof IModuleTypeElement) && !(typeElement instanceof ModuleInterfaceElement))
                    continue;

                boolean isInterface = typeElement instanceof ModuleInterfaceElement;

                // set the default images for the palette entry
                ImageDescriptor imageDescNorm = ImageFactory.getDescriptor(ImageFactory.DEFAULT,"vs",null,0);
                ImageDescriptor imageDescLarge = ImageFactory.getDescriptor(ImageFactory.DEFAULT,"s",null,0);
                if (typeElement instanceof IHasDisplayString) {
                    IDisplayString dps = ((IHasDisplayString)typeElement).getDisplayString();
                    String iid = dps.getAsString(IDisplayString.Prop.IMAGE);
                    if (StringUtils.isNotEmpty(iid)) {
                        imageDescNorm = ImageFactory.getDescriptor(iid,"vs",null,0);
                        imageDescLarge = ImageFactory.getDescriptor(iid,"s",null,0);
                    }
                }

                // create the tool entry (if we are currently dropping an interface, we should use the IF type for the like parameter
                CombinedTemplateCreationEntry toolEntry = new CombinedTemplateCreationEntry(
                        typeElement.getName() + (isInterface ? NBSP+"(interface)" : ""), StringUtils.makeBriefDocu(typeElement.getComment(), 300),
                        new ModelFactory(SubmoduleElementEx.getStaticTagName(),StringUtils.toInstanceName(typeElement.getName()), typeElement.getName(), isInterface),
                        imageDescNorm, imageDescLarge );

                drawer.add(toolEntry);
            }
        }

        return drawer;
    }

    /**
     * Creates several submodule drawers using currently parsed types,
     * and using the GROUP property as the drawer name.
     */
    private static List<PaletteContainer> createSubmodulesDrawers() {
        TreeMap<String,PaletteDrawer> containerMap = new TreeMap<String, PaletteDrawer>();

        // get all the possible type names in alphabetical order
        List<String> typeNames
                = new ArrayList<String>(NEDResourcesPlugin.getNEDResources().getModuleNames());
        typeNames.addAll(NEDResourcesPlugin.getNEDResources().getModuleInterfaceNames());
        Collections.sort(typeNames, StringUtils.dictionaryComparator);

        for (String name : typeNames) {
            INEDTypeInfo comp = NEDResourcesPlugin.getNEDResources().getComponent(name);
            INEDElement typeElement = comp.getNEDElement();
            boolean isInterface = typeElement instanceof ModuleInterfaceElement;

            // skip this type if it is a top level network
            if (typeElement instanceof CompoundModuleElementEx &&
                    ((CompoundModuleElementEx)typeElement).getIsNetwork()) {
                continue;
            }

            // set the default images for the palette entry
            ImageDescriptor imageDescNorm = ImageFactory.getDescriptor(ImageFactory.DEFAULT,"vs",null,0);
            ImageDescriptor imageDescLarge = ImageFactory.getDescriptor(ImageFactory.DEFAULT,"s",null,0);
            if (typeElement instanceof IHasDisplayString) {
                IDisplayString dps = ((IHasDisplayString)typeElement).getDisplayString();
                String iid = dps.getAsString(IDisplayString.Prop.IMAGE);
                if (StringUtils.isNotEmpty(iid)) {
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
                    name + (isInterface ? NBSP+"(interface)" : ""), StringUtils.makeBriefDocu(typeElement.getComment(), 300),
                    new ModelFactory(SubmoduleElementEx.getStaticTagName(),StringUtils.toInstanceName(name), name, isInterface),
                    imageDescNorm, imageDescLarge );
            // add to the selected drawer
            currentDrawer.add(combined);
        }

        // remove the default drawer (and put it at the beginning)
        PaletteContainer def = containerMap.remove(DEFAULT_SUBMODULE_GROUP_NAME);
        ArrayList<PaletteContainer> result = new ArrayList<PaletteContainer>(containerMap.values());
        if (def != null && def.getChildren().size() > 0)
            result.add(0, def);
        return result;
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

        MarqueeToolEntry marquee = new MarqueeToolEntry("Connection"+NBSP+"selector","Drag out an area to select connections in it");
        marquee.setToolProperty(MarqueeSelectionTool.PROPERTY_MARQUEE_BEHAVIOR,
                new Integer(MarqueeSelectionTool.BEHAVIOR_CONNECTIONS_TOUCHED));
        connectionStack.add(marquee);


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

        controlGroup.add(createChannelsStack());

        return controlGroup;
    }

    /**
     * Builds a drawer containing base NED components without specifying types like
     * simple and compound modules
     */
    private static PaletteContainer createTypesDrawer() {

        PaletteDrawer drawer = new PaletteDrawer("Types", ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_FOLDER));;
        drawer.setInitialState(PaletteDrawer.INITIAL_STATE_PINNED_OPEN);

         CombinedTemplateCreationEntry entry = new CombinedTemplateCreationEntry(
                "Simple"+NBSP+"module",
                "Create a simple module type",
                new ModelFactory(SimpleModuleElementEx.getStaticTagName(), IHasName.DEFAULT_TYPE_NAME),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_SIMPLEMODULE),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_SIMPLEMODULE)
        );
        drawer.add(entry);

        entry = new CombinedTemplateCreationEntry(
                "Compound"+NBSP+"Module",
                "Create a compound module type that may contain submodules",
                new ModelFactory(CompoundModuleElementEx.getStaticTagName(), IHasName.DEFAULT_TYPE_NAME),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_COMPOUNDMODULE),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_COMPOUNDMODULE)
        );
        drawer.add(entry);

        entry = new CombinedTemplateCreationEntry(
                "Channel",
                "Create a channel type",
                new ModelFactory(ChannelElementEx.getStaticTagName(), IHasName.DEFAULT_TYPE_NAME),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_CHANNEL),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_CHANNEL)
        );
        drawer.add(entry);

        entry = new CombinedTemplateCreationEntry(
        		"Module"+NBSP+"Interface",
        		"Create a module interface type",
        		new ModelFactory(ModuleInterfaceElementEx.getStaticTagName(), IHasName.DEFAULT_TYPE_NAME),
        		ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_INTERFACE),
        		ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_INTERFACE)
        );
        drawer.add(entry);

        entry = new CombinedTemplateCreationEntry(
                "Channel"+NBSP+"Interface",
                "Create a channel interface type",
                new ModelFactory(ChannelInterfaceElementEx.getStaticTagName(), IHasName.DEFAULT_TYPE_NAME),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_CHANNELINTERFACE),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_CHANNELINTERFACE)
        );
        drawer.add(entry);

        return drawer;
    }
}
