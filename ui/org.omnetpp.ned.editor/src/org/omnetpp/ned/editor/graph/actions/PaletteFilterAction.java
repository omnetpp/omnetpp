package org.omnetpp.ned.editor.graph.actions;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.resources.IProject;
import org.eclipse.gef.ui.actions.WorkbenchPartAction;
import org.eclipse.jface.viewers.CheckStateChangedEvent;
import org.eclipse.jface.viewers.ICheckStateListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.dialogs.CheckedTreeSelectionDialog;
import org.eclipse.ui.part.FileEditorInput;
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeLabelProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.graph.GraphicalNedEditor;
import org.omnetpp.ned.editor.graph.misc.PaletteManager;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;


/**
 * Dialog for filtering the palette contents by packages
 * 
 * @author Andras
 */
public class PaletteFilterAction extends WorkbenchPartAction {

    public PaletteFilterAction(GraphicalNedEditor part) {
        super(part);
        setText("Select &packages...");
        setId("PaletteFilter");
        setToolTipText("Select NED packages for the palette");
    }

    @Override
    protected boolean calculateEnabled() {
        return true;
    }
    
    @Override
    public void run() {
        GraphicalNedEditor editor = (GraphicalNedEditor)getWorkbenchPart();

        // collect the list of (non-empty) packages, and how many NED types they contain 
        IProject project = ((FileEditorInput)editor.getEditorInput()).getFile().getProject();
        NEDResources res = NEDResourcesPlugin.getNEDResources();
        final Map<String,Integer> packages = new LinkedHashMap<String,Integer>();  // (package,count)
        List<String> orderedQNames = new ArrayList<String>(res.getNedTypeQNames(project));
        Collections.sort(orderedQNames);
        for (String qname : orderedQNames) {
            INEDTypeInfo nedType = res.getToplevelNedType(qname, project);
            String packageName = StringUtils.fallback(nedType.getPackageName(), "(default)");
            if (!packages.containsKey(packageName))
                packages.put(packageName, 1);
            else 
                packages.put(packageName, packages.get(packageName) + 1);
        }
        
        // build a tree (note: tree nodes will be package name prefixes like
        // "inet", "inet.transport", "inet.transport.tcp" -- although we'll
        // only display their last segment in the tree). 
        PaletteManager paletteManager = editor.getPaletteManager();
        Set<String> excludedPackages = paletteManager.getExcludedPackages();

        Set<GenericTreeNode> allNodes = new HashSet<GenericTreeNode>();
        Set<GenericTreeNode> initialSelectedNodes = new HashSet<GenericTreeNode>();
        GenericTreeNode root = new GenericTreeNode("root");
        for (String packageName : packages.keySet()) {
            GenericTreeNode node = root;
            String packageNamePrefix = null;
            for (String segment : packageName.split("\\.")) {
                if (packageNamePrefix == null)
                    packageNamePrefix = segment;
                else
                    packageNamePrefix += "." + segment;
                node = getOrCreateChild(node, packageNamePrefix);
                allNodes.add(node);
                if (!excludedPackages.contains(packageName))
                    initialSelectedNodes.add(node);
            }
        }
        
        // display tree dialog
        CheckedTreeSelectionDialog dlg = new CheckedTreeSelectionDialog(
                null,
                new GenericTreeLabelProvider(new LabelProvider() {
                    @Override
                    public String getText(Object element) {
                        String packageName = (String)element;
                        int count = packages.containsKey(packageName) ? packages.get(packageName) : 0;
                        String displayName = StringUtils.isEmpty(packageName) ? "(default)" : packageName.replaceFirst("^.*\\.", "");
                        return displayName + " (" + count + ")";
                    }
                }),
                new GenericTreeContentProvider()) 
        {
            @Override
            protected Control createDialogArea(Composite parent) {
                Control composite = super.createDialogArea(parent);
                getTreeViewer().expandToLevel(2);
                getTreeViewer().addCheckStateListener(new ICheckStateListener() {
                    public void checkStateChanged(CheckStateChangedEvent event) {
                        getTreeViewer().setSelection(new StructuredSelection(event.getElement()));
                        getTreeViewer().setExpandedState(event.getElement(), true);
                    }
                });
                getTreeViewer().getTree().addSelectionListener(new SelectionAdapter() {
                    @Override
                    public void widgetDefaultSelected(SelectionEvent e) {
                        Object element = e.item.getData();
                        //getTreeViewer().expandToLevel(element, 10);
                        getTreeViewer().setSubtreeChecked(element, !getTreeViewer().getChecked(element));
                    }
                });
                return composite;
            }  
            @Override
            protected Composite createSelectionButtons(Composite composite) {
                Composite buttonComposite = super.createSelectionButtons(composite);
                final Button selectButton = createButton(buttonComposite, -1, "Select Subtree", false);
                final Button deselectButton = createButton(buttonComposite, -1, "Deselect Subtree", false);
                SelectionListener listener = new SelectionAdapter() {
                    @Override
                    public void widgetSelected(SelectionEvent e) {
                        Object element = ((IStructuredSelection)getTreeViewer().getSelection()).getFirstElement();
                        if (element != null)
                            getTreeViewer().setSubtreeChecked(element, e.widget == selectButton);
                    }
                };
                selectButton.addSelectionListener(listener);
                deselectButton.addSelectionListener(listener);
                return buttonComposite;
            }
        };
        dlg.setTitle("Select Packages");
        dlg.setMessage("Select packages for the palette:");
        dlg.setHelpAvailable(false);
        dlg.setInput(root);
        dlg.setStatusLineAboveButtons(false);
        dlg.setInitialSelections(initialSelectedNodes.toArray(new Object[]{}));

        if (dlg.open() == Window.OK) {
            // extract the result, and set it back on the paletteManager
            Object[] selectedNodes = dlg.getResult();
            Set<String> result = new HashSet<String>();
            allNodes.removeAll(Arrays.asList(selectedNodes)); // abuse allNodes as unselectedNodes
            for (GenericTreeNode node : allNodes)
                result.add((String)node.getPayload());
            paletteManager.setExcludedPackages(result);
        }
    }

    private GenericTreeNode getOrCreateChild(GenericTreeNode node, String segment) {
        for (GenericTreeNode child : node.getChildren())
            if (child.getPayload().equals(segment))
                return child;
        GenericTreeNode child = new GenericTreeNode(segment);
        node.addChild(child);
        return child;
    }
}
