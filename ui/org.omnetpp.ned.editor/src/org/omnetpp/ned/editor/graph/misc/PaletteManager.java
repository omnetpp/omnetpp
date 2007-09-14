package org.omnetpp.ned.editor.graph.misc;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.gef.palette.*;
import org.eclipse.gef.tools.MarqueeSelectionTool;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IFileEditorInput;

import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.graph.GraphicalNedEditor;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.NEDElementUtilEx;
import org.omnetpp.ned.model.interfaces.IHasDisplayString;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.IModuleKindTypeElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.pojo.ChannelInterfaceElement;
import org.omnetpp.ned.model.pojo.ModuleInterfaceElement;
import org.omnetpp.ned.model.pojo.NEDElementTags;
import org.omnetpp.ned.model.pojo.PropertyElement;

/**
 * Responsible for managing palette entries and keeping them in sync with
 * the components in NEDResources plugin
 *
 * @author rhornig
 */
public class PaletteManager {
	private static final String NBSP = "\u00A0";
    private static final String GROUP_PROPERTY = "group";

    // encoding of internal IDs
    private static final String MRU_GROUP = "!mru";
    private static final String CONNECTIONS_GROUP = "!connections";
    private static final String TYPES_GROUP = "!types";
    private static final String GROUP_DELIMITER = "~";

    // state
    protected GraphicalNedEditor hostingEditor;
    protected PaletteRoot nedPalette;
    protected PaletteContainer toolsContainer;
    protected PaletteContainer channelsStack;
    protected PaletteDrawer typesContainer;
    protected PaletteDrawer defaultContainer;

    protected Map<String, PaletteEntry> currentEntries = new HashMap<String, PaletteEntry>();
    protected Map<String, PaletteDrawer> currentContainers = new HashMap<String, PaletteDrawer>();

    public PaletteManager(GraphicalNedEditor hostingEditor) {
        super();
        this.hostingEditor = hostingEditor;
        nedPalette = new PaletteRoot();
        channelsStack = new PaletteStack("Connections", "Connect modules using this tool",ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_CONNECTION));
        toolsContainer = createTools();
        typesContainer = new PaletteDrawer("Types", ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_FOLDER));
        typesContainer.setInitialState(PaletteDrawer.INITIAL_STATE_PINNED_OPEN);
        defaultContainer = new PaletteDrawer("Submodules", ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_FOLDER));

        refresh();
    }

    public PaletteRoot getRootPalette() {
        return nedPalette;
    }


    /**
     * Builds the palette (all drawers)
     */
    public void refresh() {
        long startMillis = System.currentTimeMillis();

        nedPalette.getChildren().clear();
        nedPalette.add(toolsContainer);
        nedPalette.add(typesContainer);
        nedPalette.add(defaultContainer);
        channelsStack.getChildren().clear();
        typesContainer.getChildren().clear();
        defaultContainer.getChildren().clear();
        for (PaletteContainer container : currentContainers.values())
            container.getChildren().clear();

        Map<String, PaletteEntry> newEntries = createPaletteModel();
        for (String id : newEntries.keySet()) {
            // if the same tool already exist, use that object so the object identity
        	// will not change unnecessarily
            if (currentEntries.containsKey(id))
                newEntries.put(id, currentEntries.get(id));

            getContainerFor(id).add(newEntries.get(id));
        }
        currentEntries = newEntries;

        // TODO sort the containers by name
        for (PaletteContainer container : currentContainers.values())
            nedPalette.add(container);

        long dt = System.currentTimeMillis() - startMillis;
        System.out.println("paletteManager refresh(): " + dt + "ms");
    }

    /**
     * The container belonging to this ID
     */
    public PaletteContainer getContainerFor(String id) {
        if (!id.contains(GROUP_DELIMITER))
            return defaultContainer;
        String group = StringUtils.substringBefore(id, GROUP_DELIMITER);
        if (MRU_GROUP.equals(group))
            return defaultContainer;
        if (CONNECTIONS_GROUP.equals(group))
            return channelsStack;
        if (TYPES_GROUP.equals(group))
            return typesContainer;

        PaletteDrawer drawer = currentContainers.get(group);
        if (drawer == null) {
            drawer = new PaletteDrawer(group, ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_FOLDER));
            drawer.setInitialState(PaletteDrawer.INITIAL_STATE_CLOSED);
            currentContainers.put(group, drawer);
        }

        return drawer;
    }

    /**
     * Builds a drawer containing basic tools like selection connection etc.
     */
    private PaletteContainer createTools() {
        PaletteGroup controlGroup = new PaletteGroup("Tools");

        ToolEntry tool = new PanningSelectionToolEntry("Selector","Select module(s)");
        tool.setToolClass(NedSelectionTool.class);
        controlGroup.add(tool);
        getRootPalette().setDefaultEntry(tool);

        controlGroup.add(channelsStack);
        return controlGroup;
    }

    private Map<String, PaletteEntry> createPaletteModel() {
        Map<String, PaletteEntry> result = new LinkedHashMap<String, PaletteEntry>();

        IEditorInput input = hostingEditor.getEditorInput();
        if (!(input instanceof IFileEditorInput))
        	return result; // sorry
        IFile file = ((IFileEditorInput)input).getFile();
        IProject contextProject = file.getProject();

    	// connection and type creation tools
        result.putAll(createChannelsStackEntries(contextProject));
        result.putAll(createTypesEntries());

        // submodule creation tools
        result.putAll(createInnerTypes(file));
        result.put("separator", new PaletteSeparator());
        result.putAll(createSubmodules(contextProject));

        return result;
    }

    /**
     * Iterates over all top level types in a NED file and gathers all NEDTypes from all components.
     * Returns a Container containing all types in this file or NULL if there are no inner types
     * defined in this file's top level modules.
     */
    private static Map<String, ToolEntry> createInnerTypes(IFile file) {
        Map<String, ToolEntry> entries = new LinkedHashMap<String, ToolEntry>();

        // add module and module interface *inner* types of NED types in this file
        for (INEDElement element : NEDResourcesPlugin.getNEDResources().getNedFileElement(file))
            if (element instanceof INedTypeElement)
            	for (INedTypeElement typeElement : ((INedTypeElement)element).getNEDTypeInfo().getInnerTypes().values())
            		if (typeElement instanceof IModuleKindTypeElement)
            			addToolEntry(typeElement, MRU_GROUP, entries);
        return entries;
    }

    /**
     * Creates several submodule drawers using currently parsed types,
     * and using the GROUP property as the drawer name.
     * @param contextProject TODO
     */
    private static Map<String, ToolEntry> createSubmodules(IProject contextProject) {
        Map<String, ToolEntry> entries = new LinkedHashMap<String, ToolEntry>();

        // get all the possible type names in alphabetical order
        List<String> typeNames = new ArrayList<String>();
        typeNames.addAll(NEDResourcesPlugin.getNEDResources().getModuleQNames(contextProject));
        typeNames.addAll(NEDResourcesPlugin.getNEDResources().getModuleInterfaceQNames(contextProject));
        Collections.sort(typeNames, StringUtils.dictionaryComparator);

        for (String name : typeNames) {
            INedTypeElement typeElement = NEDResourcesPlugin.getNEDResources().getToplevelNedType(name, contextProject).getNEDElement();

            // skip this type if it is a top level network
            if (typeElement instanceof CompoundModuleElementEx &&
                    ((CompoundModuleElementEx)typeElement).getIsNetwork()) {
                continue;
            }

            // determine which palette group it belongs to or put it into the default
            PropertyElement property = typeElement.getNEDTypeInfo().getProperties().get(GROUP_PROPERTY);
            String group = (property == null) ? "" : NEDElementUtilEx.getPropertyValue(property);

            addToolEntry(typeElement, group, entries);
        }

        return entries;
    }

    private static void addToolEntry(INedTypeElement typeElement, String group, Map<String, ToolEntry> entries) {
        String fullyQualifiedTypeName = typeElement.getNEDTypeInfo().getFullyQualifiedName();

        String key = fullyQualifiedTypeName;
        if (StringUtils.isNotEmpty(group))
            key = group+GROUP_DELIMITER+key;

        // set the default images for the palette entry
        ImageDescriptor imageDescNorm = ImageFactory.getDescriptor(ImageFactory.DEFAULT,"vs",null,0);
        ImageDescriptor imageDescLarge = ImageFactory.getDescriptor(ImageFactory.DEFAULT,"s",null,0);
        if (typeElement instanceof IHasDisplayString) {
            IDisplayString dps = ((IHasDisplayString)typeElement).getDisplayString();
            String imageId = dps.getAsString(IDisplayString.Prop.IMAGE);
            if (StringUtils.isNotEmpty(imageId)) {
                imageDescNorm = ImageFactory.getDescriptor(imageId,"vs",null,0);
                imageDescLarge = ImageFactory.getDescriptor(imageId,"s",null,0);
                key += ":"+imageId;
            }
        }

        // create the tool entry (if we are currently dropping an interface, we should use the IF type for the like parameter

        boolean isInterface = typeElement instanceof ModuleInterfaceElement;
        String label = fullyQualifiedTypeName; // or: getName();
        if (typeElement.getEnclosingTypeElement() != null)
        	label += NBSP+"(in"+NBSP+typeElement.getEnclosingTypeElement().getName()+")";
        if (isInterface)
        	label += NBSP+"(interface)";

        String instanceName = StringUtils.toInstanceName(typeElement.getName());
		CombinedTemplateCreationEntry toolEntry = new CombinedTemplateCreationEntry(
                label, StringUtils.makeBriefDocu(typeElement.getComment(), 300),
                new ModelFactory(NEDElementTags.NED_SUBMODULE, instanceName, fullyQualifiedTypeName, isInterface),
                imageDescNorm, imageDescLarge );

        entries.put(key, toolEntry);
    }

    private static Map<String, ToolEntry> createChannelsStackEntries(IProject contextProject) {
        Map<String, ToolEntry> entries = new LinkedHashMap<String, ToolEntry>();

        ConnectionCreationToolEntry defaultConnectionTool = new ConnectionCreationToolEntry(
                "Connection",
                "Create connections between submodules, or submodule and parent module",
                new ModelFactory(NEDElementTags.NED_CONNECTION),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_CONNECTION),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_CONNECTION)
        );

        // sets the required connection tool
        defaultConnectionTool.setToolClass(NedConnectionCreationTool.class);
        entries.put(CONNECTIONS_GROUP+GROUP_DELIMITER+"connection", defaultConnectionTool);

        // connection selection
        MarqueeToolEntry marquee = new MarqueeToolEntry("Connection"+NBSP+"selector", "Drag out an area to select connections in it");
        marquee.setToolProperty(MarqueeSelectionTool.PROPERTY_MARQUEE_BEHAVIOR,
                MarqueeSelectionTool.BEHAVIOR_CONNECTIONS_TOUCHED);
        entries.put(CONNECTIONS_GROUP+GROUP_DELIMITER+"marquee", marquee);

        // get all the possible type names in alphabetical order
        List<String> channelNames = new ArrayList<String>();
        channelNames.addAll(NEDResourcesPlugin.getNEDResources().getChannelQNames(contextProject));
        channelNames.addAll(NEDResourcesPlugin.getNEDResources().getChannelInterfaceQNames(contextProject));
        Collections.sort(channelNames, StringUtils.dictionaryComparator);

        for (String fullyQualifiedName : channelNames) {
            INEDTypeInfo typeInfo = NEDResourcesPlugin.getNEDResources().getToplevelNedType(fullyQualifiedName, contextProject);
            INEDElement modelElement = typeInfo.getNEDElement();
            boolean isInterface = modelElement instanceof ChannelInterfaceElement;

            ConnectionCreationToolEntry tool = new ConnectionCreationToolEntry(
                    fullyQualifiedName + (isInterface ? NBSP+"(interface)" : ""),
                    StringUtils.makeBriefDocu(typeInfo.getNEDElement().getComment(), 300),
                    new ModelFactory(NEDElementTags.NED_CONNECTION, "n/a", fullyQualifiedName, isInterface),
                    ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_CONNECTION),
                    ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_CONNECTION)
            );
            // sets the required connection tool
            tool.setToolClass(NedConnectionCreationTool.class);
            entries.put(CONNECTIONS_GROUP+GROUP_DELIMITER+fullyQualifiedName, tool);
        }
        return entries;
    }

    /**
     * Builds a tool entry list containing base top level NED components like simple, module, channel etc.
     */
    private static Map<String, ToolEntry> createTypesEntries() {
        Map<String, ToolEntry> entries = new LinkedHashMap<String, ToolEntry>();

        CombinedTemplateCreationEntry entry = new CombinedTemplateCreationEntry(
                "Simple"+NBSP+"Module",
                "Create a simple module type",
                new ModelFactory(NEDElementTags.NED_SIMPLE_MODULE, IHasName.DEFAULT_TYPE_NAME),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_SIMPLEMODULE),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_SIMPLEMODULE)
        );
        entries.put(TYPES_GROUP+GROUP_DELIMITER+"simple", entry);

        entry = new CombinedTemplateCreationEntry(
                "Compound"+NBSP+"Module",
                "Create a compound module type that may contain submodules",
                new ModelFactory(NEDElementTags.NED_COMPOUND_MODULE, IHasName.DEFAULT_TYPE_NAME),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_COMPOUNDMODULE),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_COMPOUNDMODULE)
        );
        entries.put(TYPES_GROUP+GROUP_DELIMITER+"compound", entry);

        entry = new CombinedTemplateCreationEntry(
                "Channel",
                "Create a channel type",
                new ModelFactory(NEDElementTags.NED_CHANNEL, IHasName.DEFAULT_TYPE_NAME),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_CHANNEL),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_CHANNEL)
        );
        entries.put(TYPES_GROUP+GROUP_DELIMITER+"channel", entry);

        entry = new CombinedTemplateCreationEntry(
        		"Module"+NBSP+"Interface",
        		"Create a module interface type",
        		new ModelFactory(NEDElementTags.NED_MODULE_INTERFACE, IHasName.DEFAULT_TYPE_NAME),
        		ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_INTERFACE),
        		ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_INTERFACE)
        );
        entries.put(TYPES_GROUP+GROUP_DELIMITER+"moduleinterface", entry);

        entry = new CombinedTemplateCreationEntry(
                "Channel"+NBSP+"Interface",
                "Create a channel interface type",
                new ModelFactory(NEDElementTags.NED_CHANNEL_INTERFACE, IHasName.DEFAULT_TYPE_NAME),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_CHANNELINTERFACE),
                ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_CHANNELINTERFACE)
        );
        entries.put(TYPES_GROUP+GROUP_DELIMITER+"channelinterface", entry);

        return entries;
    }

}
