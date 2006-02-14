/*******************************************************************************
 * Copyright (c) 2000, 2004 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Common Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/cpl-v10.html
 * 
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.omnetpp.ned.editor.graph;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Platform;
import org.eclipse.gef.palette.*;
import org.eclipse.gef.requests.SimpleFactory;
import org.eclipse.gef.tools.MarqueeSelectionTool;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.ned.editor.graph.misc.ImageFactory;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.*;
//import org.omnetpp.ned.model.ModelUtil;
//import org.omnetpp.ned.model.swig.*;

public class ModuleEditorPlugin extends AbstractUIPlugin {

    public final static String PLUGIN_ID = "org.omnetpp.ned.editor.graph";

    private static ModuleEditorPlugin singleton;

    public ModuleEditorPlugin() {
        super();
        if (singleton == null) {
            singleton = this;
        }
        // XXX remove below. only for testing
//        NEDParser np = new NEDParser();
//        np.setParseExpressions(false);
//        np.parseFile("c:/temp/tictoc.ned");
//        NEDElement treeRoot = np.getTree();
//        System.out.println(treeRoot.getTagName());
//        NEDGenerator ng = new NEDGenerator();
//        ng.setNewSyntax(false);
//        System.out.println(ng.generate(treeRoot, ""));
//        System.out.println(ModelUtil.printSwigElement(treeRoot));
//        
//        // generate EMF tree
//        System.out.println(ModelUtil.swig2emf(treeRoot));
    }

    public static ModuleEditorPlugin getDefault() {
        return singleton;
    }

    public static String getString(String key) {
        return Platform.getResourceString(ModuleEditorPlugin.getDefault().getBundle(), key);
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

        ToolEntry tool = new PanningSelectionToolEntry("Arrow","Select and manipulate");
        entries.add(tool);
        root.setDefaultEntry(tool);

        PaletteStack marqueeStack = new PaletteStack("Marquee Tools", "", null);
        MarqueeToolEntry mte;
        marqueeStack.add(mte = new MarqueeToolEntry("Modules","Marquee tool to select only modules and submodules"));
        mte.setToolProperty(MarqueeSelectionTool.PROPERTY_MARQUEE_BEHAVIOR, MarqueeSelectionTool.BEHAVIOR_NODES_CONTAINED);
        marqueeStack.add(mte = new MarqueeToolEntry("Connections", "Marquee tool to select only connections"));
        mte.setToolProperty(MarqueeSelectionTool.PROPERTY_MARQUEE_BEHAVIOR, MarqueeSelectionTool.BEHAVIOR_CONNECTIONS_TOUCHED);
        marqueeStack.add(new MarqueeToolEntry("All", "Marquee tool to select anything"));
        mte.setToolProperty(MarqueeSelectionTool.PROPERTY_MARQUEE_BEHAVIOR, MarqueeSelectionTool.BEHAVIOR_NODES_AND_CONNECTIONS);
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

        combined = new CombinedTemplateCreationEntry(
                "Submodule",
                "A submodule that can be placed in any compound module",
                TemplateConstants.SIMPLE_MODULE, new SimpleFactory(SimpleModule.class), 
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_SIMPLE),
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_SIMPLE,"l",null,24)
        );
        entries.add(combined);

        combined = new CombinedTemplateCreationEntry(
                "Module",
                "A compound module that is built up from several other modules",
                TemplateConstants.MODULE, new SimpleFactory(Module.class), 
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE),
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE,"l",null,24)
        );
        entries.add(combined);

        entries.add(new PaletteSeparator());

        combined = new CombinedTemplateCreationEntry(
                "Comment",
                "A comment box to add remark to the diagram",
                TemplateConstants.COMMENT, new SimpleFactory(Comment.class), 
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_LABEL),
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_LABEL,"l",null,24)
        );
        entries.add(combined);

        drawer.addAll(entries);
        return drawer;
    }

    static private PaletteContainer createTemplatesDrawer() {
        PaletteDrawer drawer = new PaletteDrawer(
                "Templates",
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_TEMPLATE)); //$NON-NLS-1$

        List entries = new ArrayList();

        CombinedTemplateCreationEntry combined = new CombinedTemplateCreationEntry(
                "Ring",
                "Arrange nodes in a ring topology",
                TemplateConstants.TEMPLATE_HALF_ADDER, ModelFactory.getHalfAdderFactory(),
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE),
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE,"l",null,24) //$NON-NLS-1$
        );
        entries.add(combined);

        combined = new CombinedTemplateCreationEntry(
                "Mesh",
                "Arrange nodes in a mesh topology",
                TemplateConstants.TEMPLATE_FULL_ADDER, ModelFactory.getFullAdderFactory(),
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE),
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE,"l",null,24)
        );
        entries.add(combined);

        drawer.addAll(entries);
        return drawer;
    }

    static private PaletteContainer createImportsDrawer() {
        PaletteDrawer drawer = new PaletteDrawer(
                "Imports",
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_TEMPLATE)); //$NON-NLS-1$

        List entries = new ArrayList();

        CombinedTemplateCreationEntry combined = new CombinedTemplateCreationEntry(
                "EthMAC",
                "Ethernet MAC protocol module",
                TemplateConstants.TEMPLATE_HALF_ADDER, ModelFactory.getHalfAdderFactory(),
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE),
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE,"l",null,24)
        );
        entries.add(combined);

        combined = new CombinedTemplateCreationEntry(
                "IPv4",
                "Internet protocol v4 module",
                TemplateConstants.TEMPLATE_FULL_ADDER, ModelFactory.getFullAdderFactory(),
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE),
                ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_MODULE,"l",null,24)
        );
        entries.add(combined);

        drawer.addAll(entries);
        return drawer;
    }
}