/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.views;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.WeakHashMap;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.viewers.DoubleClickEvent;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Item;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeLabelProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.ui.GenericTreeUtils;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.HtmlHoverInfo;
import org.omnetpp.common.ui.IHoverInfoProvider;
import org.omnetpp.common.util.ActionExt;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;
import org.omnetpp.ned.model.pojo.ExtendsElement;
import org.omnetpp.ned.model.pojo.InterfaceNameElement;
import org.omnetpp.ned.model.ui.NedModelLabelProvider;

/**
 * Displays the inheritance tree of a NED type.
 *
 * @author Andras
 */
//XXX currently doesn't show inner types
//XXX add tooltip (see fixme below)
public class NedInheritanceView extends AbstractModuleView {
    private TreeViewer treeViewer;

    private MenuManager contextMenuManager = new MenuManager("#PopupMenu");

    // hashmap to save/restore view's state when switching across editors
    private WeakHashMap<IEditorInput, ISelection> selectedElements = new WeakHashMap<IEditorInput, ISelection>();

    public NedInheritanceView() {
    }

    @Override
    public Control createViewControl(Composite parent) {
        createTreeViewer(parent);
        createActions();
        return treeViewer.getTree();
    }

    private void createTreeViewer(Composite parent) {
        treeViewer = new TreeViewer(parent, SWT.SINGLE);

        // set label provider and content provider
        treeViewer.setLabelProvider(new GenericTreeLabelProvider(NedModelLabelProvider.getInstance()));
        treeViewer.setContentProvider(new GenericTreeContentProvider());

        treeViewer.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                IEditorPart editor = getAssociatedEditor();
                if (!event.getSelection().isEmpty() && editor != null) {
                    // remember selection (we'll try to restore it after tree rebuild)
                    selectedElements.put(editor.getEditorInput(), event.getSelection());
                }
            }
        });

        // create context menu
        getViewSite().registerContextMenu(contextMenuManager, treeViewer);
        treeViewer.getTree().setMenu(contextMenuManager.createContextMenu(treeViewer.getTree()));

        // add tooltip support to the tree
        new HoverSupport().adapt(treeViewer.getTree(), new IHoverInfoProvider() {
            public HtmlHoverInfo getHoverFor(Control control, int x, int y) {
                Item item = treeViewer.getTree().getItem(new Point(x,y));
                Object element = item==null ? null : item.getData();
                if (element instanceof GenericTreeNode)
                    element = ((GenericTreeNode)element).getPayload();
                //FIXME produce some text
                return null;
            }
        });

    }

    private void createActions() {
        IAction pinAction = getOrCreatePinAction();

        class GotoNedFileAction extends ActionExt {
            GotoNedFileAction() {
                setText("Open NED Declaration");
            }
            @Override
            public void run() {
                INedElement sel = getNedElementFromSelection();
                if (sel != null)
                    NedResourcesPlugin.openNedElementInEditor(sel);
            }
            public void selectionChanged(SelectionChangedEvent event) {
                INedElement sel = getNedElementFromSelection();
                setEnabled(sel != null);
            }
            private INedElement getNedElementFromSelection() {
                Object element = ((IStructuredSelection)treeViewer.getSelection()).getFirstElement();
                if (element instanceof GenericTreeNode)
                    element = ((GenericTreeNode)element).getPayload();
                return (INedElement)element;
            }
        };

        final ActionExt gotoNedAction = new GotoNedFileAction();
        treeViewer.addSelectionChangedListener(gotoNedAction);

        // add double-click support to the tree
        treeViewer.addDoubleClickListener(new IDoubleClickListener() {
            public void doubleClick(DoubleClickEvent event) {
                gotoNedAction.run();
            }
        });

        // build menus and toolbar
        contextMenuManager.add(gotoNedAction);
        contextMenuManager.add(pinAction);

        IToolBarManager toolBarManager = getViewSite().getActionBars().getToolBarManager();
        toolBarManager.add(pinAction);

        IMenuManager menuManager = getViewSite().getActionBars().getMenuManager();
        menuManager.add(pinAction);
    }

    @Override
    protected void showMessage(String text) {
        super.showMessage(text);
        treeViewer.setInput(null);
    }

    @Override
    public void setFocus() {
        if (isShowingMessage())
            super.setFocus();
        else
            treeViewer.getTree().setFocus();
    }

    public void buildContent(INedElement selectedElement, final InifileAnalyzer analyzer, final String section, String key) {
        INedTypeInfo inputNedType = null;

        // find first usable parent
        while (inputNedType == null && selectedElement != null) {
            if (selectedElement instanceof INedTypeElement)
                inputNedType = ((INedTypeElement)selectedElement).getNedTypeInfo();
            else if (selectedElement instanceof ISubmoduleOrConnection)
                inputNedType = ((ISubmoduleOrConnection)selectedElement).getNedTypeInfo();
            selectedElement = selectedElement.getParent();
        }

        if (inputNedType == null) {
            showMessage("No NED type selected.");
            return;
        }

        // build tree: first the inheritance chain, then the subtype hierarchy
        final GenericTreeNode rootNode = new GenericTreeNode("root");
        GenericTreeNode currentNode = rootNode;
        List<INedTypeInfo> extendsChain = inputNedType.getInheritanceChain();
        for (INedTypeInfo nedType : extendsChain.reversed()) {
            if (nedType != inputNedType) {
                GenericTreeNode newNode = new GenericTreeNode(nedType.getNedElement());
                currentNode.addChild(newNode);
                currentNode = newNode;
            }
        }
        GenericTreeNode inputNedTypeNode = buildInheritanceTreeOf(inputNedType, currentNode, new HashSet<INedTypeInfo>());

        // prevent collapsing all treeviewer nodes: only set it on viewer if it's different from old input
        if (!GenericTreeUtils.treeEquals(rootNode, (GenericTreeNode)treeViewer.getInput())) {
            treeViewer.setInput(rootNode);
            treeViewer.reveal(rootNode);
            treeViewer.reveal(inputNedTypeNode);
            treeViewer.setSelection(new StructuredSelection(inputNedTypeNode));
            treeViewer.setExpandedState(inputNedTypeNode, true);
        }

        // refresh the viewer anyway, because e.g. parameter value changes are not reflected in the input tree
        treeViewer.refresh();
        setContentDescription("");
    }

    private GenericTreeNode buildInheritanceTreeOf(INedTypeInfo typeInfo, GenericTreeNode parentNode, Set<INedTypeInfo> visited) {
        GenericTreeNode node = new GenericTreeNode(typeInfo.getNedElement());
        parentNode.addChild(node);

        if (!visited.contains(typeInfo)) {  // cycle detection
            visited.add(typeInfo);
            List<INedTypeInfo> subtypes = getSubtypesOf(typeInfo);
            subtypes.sort(Comparator.comparing(INedTypeInfo::getName));
            for (INedTypeInfo type : subtypes)
                buildInheritanceTreeOf(type, node, visited);
        }
        return node;
    }

    private List<INedTypeInfo> getSubtypesOf(INedTypeInfo inputType) {
        List<INedTypeInfo> result = new ArrayList<INedTypeInfo>();
        INedTypeResolver res = inputType.getResolver();

        // examine all NED types we know of, whether they extend/implement this one (not too efficient)
        for (INedTypeInfo type : res.getToplevelNedTypesFromAllProjects()) {
            INedTypeLookupContext lookupContext = type.getNedElement().getParentLookupContext();
            for (INedElement child : type.getNedElement()) {
                String superName = null;
                if (child instanceof ExtendsElement)
                    superName = ((ExtendsElement)child).getName();
                else if (child instanceof InterfaceNameElement)
                    superName = ((InterfaceNameElement)child).getName();
                if (superName != null) {
                    INedTypeInfo superType = res.lookupNedType(superName, lookupContext);
                    if (superType == inputType)
                        result.add(type);
                }
            }
        }
        return result;
    }

}
