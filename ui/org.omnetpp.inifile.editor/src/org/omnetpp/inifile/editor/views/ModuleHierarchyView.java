/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.views;

import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;
import java.util.Vector;
import java.util.WeakHashMap;

import org.eclipse.core.resources.IProject;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.viewers.DoubleClickEvent;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Item;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.ui.GenericTreeUtils;
import org.omnetpp.common.ui.HtmlHoverInfo;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverInfoProvider;
import org.omnetpp.common.util.ActionExt;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.IGotoInifile;
import org.omnetpp.inifile.editor.model.IReadonlyInifileDocument;
import org.omnetpp.inifile.editor.model.ITimeout;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileHoverUtils;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.ParamCollector;
import org.omnetpp.inifile.editor.model.ParamResolution;
import org.omnetpp.inifile.editor.model.ParamResolutionDisabledException;
import org.omnetpp.inifile.editor.model.ParamResolutionTimeoutException;
import org.omnetpp.inifile.editor.model.PropertyResolution;
import org.omnetpp.inifile.editor.model.SectionKey;
import org.omnetpp.inifile.editor.model.Timeout;
import org.omnetpp.ned.core.IModuleTreeVisitor;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.core.NedTreeTraversal;
import org.omnetpp.ned.core.ParamUtil;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;
import org.omnetpp.ned.model.pojo.ParamElement;
import org.omnetpp.ned.model.ui.NedModelLabelProvider;

/**
 * Displays NED module hierarchy with module parameters, and
 * optionally, values assigned in ini files.
 *
 * @author Andras
 */
public class ModuleHierarchyView extends AbstractModuleView {
    private TreeViewer treeViewer;
    private IReadonlyInifileDocument inifileDocument; // corresponds to the current selection; needed by the label provider
    private InifileAnalyzer inifileAnalyzer; // corresponds to the current selection; unfortunately needed by the hover
    private String section; // corresponds to the current selection; needed by incremental tree building
    private IProject contextProject; // corresponds to the current selection; needed by incremental tree building

    private MenuManager contextMenuManager = new MenuManager("#PopupMenu");

    // hashmap to save/restore view's state when switching across editors
    private WeakHashMap<IEditorInput, ISelection> selectedElements = new WeakHashMap<IEditorInput, ISelection>();

    /**
     * A payload class for the GenericTreeNode tree that is displayed in the view
     */
    private static class ErrorNode {
        String text;
        ErrorNode(String text) {
            this.text = text;
        }
        @Override
        public String toString() {
            return text;
        }
        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            final ErrorNode other = (ErrorNode) obj;
            if (text == null) {
                if (other.text != null)
                    return false;
            }
            else if (!text.equals(other.text))
                return false;
            return true;
        }
    }

    /**
     * A payload class for the GenericTreeNode tree that is displayed in the view
     */
    private static class InformationNode {
        String text;
        InformationNode(String text) {
            this.text = text;
        }
        @Override
        public String toString() {
            return text;
        }
        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            final InformationNode other = (InformationNode) obj;
            if (text == null) {
                if (other.text != null)
                    return false;
            }
            else if (!text.equals(other.text))
                return false;
            return true;
        }
    }

    /**
     * Node contents for the GenericTreeNode tree that is displayed in the view
     */
    private static class SubmoduleOrConnectionPayload {
        final String fullPath;
        final ISubmoduleOrConnection element; // null at the root
        final INedTypeElement typeElement; // null if type is unknown

        /* for convenience */
        public SubmoduleOrConnectionPayload(String moduleFullPath, ISubmoduleOrConnection element, INedTypeElement typeElement) {
            this.fullPath = moduleFullPath;
            this.element = element;
            this.typeElement = typeElement;
            //Debug.println("PATH="+moduleFullPath+" Node="+submoduleNode+" SubmoduleType="+(submoduleNode==null ? null : submoduleNode.getType())+"  Type="+submoduleType);
        }

        /* Needed for GenericTreeUtil.treeEquals() */
        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null || getClass() != obj.getClass())
                return false;
            final SubmoduleOrConnectionPayload other = (SubmoduleOrConnectionPayload) obj;
            return fullPath.equals(other.fullPath) &&
                   element == other.element &&
                   typeElement == other.typeElement;
        }
    }

    // Note: TreeNode can have TreeNode parent only!
    // This is a continuation node if children == null
    private class SubmoduleOrConnectionNode extends GenericTreeNode {
        public final String fullName;
        public final ISubmoduleOrConnection element;
        public final INedTypeInfo typeInfo;

        private String fullPath;

        public SubmoduleOrConnectionNode(Object payload, String fullName, ISubmoduleOrConnection element, INedTypeInfo typeInfo) {
            super(payload);
            this.fullName = fullName;
            this.element = element;
            this.typeInfo = typeInfo;
        }

        @Override
        public boolean hasChildren() {
            return ModuleHierarchyView.this.hasChildren(this, element);
        }

        @Override
        protected GenericTreeNode[] computeChildren() {
            ModuleHierarchyView.this.computeChildren(this, element, typeInfo);
            return internalGetChildren() != null ? internalGetChildren() : EMPTY_ARRAY;
        }

        public String getFullPath() {
            if (fullPath == null) {
                SubmoduleOrConnectionNode parent = getSubmoduleParent();
                fullPath = parent != null ? parent.getFullPath() + "." + fullName : fullName;
            }
            return fullPath;
        }

        public Vector<ISubmoduleOrConnection> getElementPath() {
            Vector<ISubmoduleOrConnection> path = new Vector<ISubmoduleOrConnection>();
            for (SubmoduleOrConnectionNode node = this; node != null; node = node.getSubmoduleParent())
                path.add(node.element);
            Collections.reverse(path);
            return path;
        }

        public Vector<INedTypeInfo> getTypeInfoPath() {
            Vector<INedTypeInfo> path = new Vector<INedTypeInfo>();
            for (SubmoduleOrConnectionNode node = this; node != null; node = node.getSubmoduleParent())
                path.add(node.typeInfo);
            Collections.reverse(path);
            return path;
        }

        private SubmoduleOrConnectionNode getSubmoduleParent() {
            GenericTreeNode parent = getParent();
            return parent instanceof SubmoduleOrConnectionNode ? (SubmoduleOrConnectionNode)parent : null;
        }
    }

    private static class ParamAssignmentGroupNode extends GenericTreeNode {
        public ParamAssignmentGroupNode(Object payload) {
            super(payload);
        }
    }

    private static class ParamAssignmentGroupElementNode extends GenericTreeNode {
        public ParamAssignmentGroupElementNode(Object payload) {
            super(payload);
        }
    }

    private static class SignalNode extends GenericTreeNode {
        public SignalNode(Object payload) {
            super(payload);
        }
    }

    private static class StatisticNode extends GenericTreeNode {
        public StatisticNode(Object payload) {
            super(payload);
        }
    }

    public ModuleHierarchyView() {
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
        treeViewer.setLabelProvider(new LabelProvider() {
            @Override
            public Image getImage(Object element) {
                if (element instanceof ParamAssignmentGroupNode)
                    // TODO: find/make a better icon
                    return InifileUtils.ICON_PAR_GROUP;
                else if (element instanceof SignalNode)
                    return InifileUtils.ICON_SIGNAL;
                else if (element instanceof StatisticNode)
                    return InifileUtils.ICON_STATISTIC;
                else if (element instanceof GenericTreeNode)
                    element = ((GenericTreeNode)element).getPayload();
                if (element instanceof SubmoduleOrConnectionPayload) {
                    SubmoduleOrConnectionPayload mn = (SubmoduleOrConnectionPayload) element;
                    if (mn.element == null)
                        return NedModelLabelProvider.getInstance().getImage(mn.typeElement);
                    if (mn.typeElement == null)
                        return NedModelLabelProvider.getInstance().getImage(mn.element);
                    // for a "like" submodule, use icon of the concrete module type
                    DisplayString dps = mn.element.getDisplayString(mn.typeElement);
                    IProject project = mn.element.getCompoundModule().getNedTypeInfo().getProject();
                    Image image = ImageFactory.of(project).getIconImage(dps.getAsString(IDisplayString.Prop.IMAGE));
                    return image!=null ? image : NedModelLabelProvider.getInstance().getImage(mn.element);
                }
                else if (element instanceof ParamResolution)
                    return InifileUtils.suggestImage(((ParamResolution) element).type);
                else if (element instanceof ErrorNode)
                    return InifileUtils.ICON_ERROR;
                else if (element instanceof InformationNode)
                    return InifileUtils.ICON_INFO;
                else
                    return null;
            }

            @Override
            public String getText(Object node) {
                Object element = node;
                if (node instanceof GenericTreeNode)
                    element = ((GenericTreeNode)element).getPayload();
                if (element instanceof SubmoduleOrConnectionPayload) {
                    SubmoduleOrConnectionPayload mn = (SubmoduleOrConnectionPayload) element;
                    if (mn.element == null) // this is the tree root
                        return mn.typeElement.getName();
                    String typeName = mn.element.getType();
                    String label = ParamUtil.getParamPathElementName(mn.element, false) + " : ";
                    if (typeName != null && !typeName.equals(""))
                        label += typeName;
                    else if (mn.typeElement != null)
                        label += mn.typeElement.getName();
                    else
                        label += "like "+mn.element.getLikeType();
                    return label;
                }
                else if (element instanceof ParamResolution)
                    return getLabelFor((ParamResolution) element, inifileDocument, !(node instanceof ParamAssignmentGroupElementNode));
                else
                    return element.toString();
            }
        });
        treeViewer.setContentProvider(new GenericTreeContentProvider());

        treeViewer.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                IEditorPart editor = getAssociatedEditor();
                if (!event.getSelection().isEmpty() && editor != null) {
                    // remember selection (we'll try to restore it after tree rebuild)
                    selectedElements.put(editor.getEditorInput(), event.getSelection());

                    // try to highlight the given element in the inifile editor
                    SectionKey sel = getSectionKeyFromSelection();
                    //XXX make sure "res" and inifile editor refer to the same IFile!!!
                    if (sel != null && editor instanceof IGotoInifile && editor != getActivePart())
                        ((IGotoInifile)editor).gotoEntry(sel.section, sel.key, IGotoInifile.Mode.AUTO);
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
                if (element instanceof ParamResolution) {
                    ParamResolution res = (ParamResolution) element;
                    if (res.section != null && res.key != null)
                        //XXX make sure "res" and inifile editor refer to the same IFile!!!
                        return new HtmlHoverInfo(InifileHoverUtils.getEntryHoverText(res.section, res.key, inifileDocument, inifileAnalyzer));
                    else
                        return new HtmlHoverInfo(InifileHoverUtils.getParamHoverText(res.elementPath, res.paramDeclaration, res.paramAssignment));
                }
                else {
                    //TODO produce some text
                }
                return null;
            }
        });
    }

    private void createActions() {
        IAction pinAction = getOrCreatePinAction();

        //XXX this is (almost) the same code as in ModuleParametersView
        final ActionExt gotoInifileAction = new ActionExt("Show in Ini File") {
            @Override
            public void run() {
                SectionKey sel = getSectionKeyFromSelection();
                IEditorPart associatedEditor = getAssociatedEditor();
                if (sel != null && associatedEditor instanceof IGotoInifile) {
                    activateEditor(associatedEditor);
                    ((IGotoInifile)associatedEditor).gotoEntry(sel.section, sel.key, IGotoInifile.Mode.AUTO);
                }
            }
            @Override
            public void selectionChanged(SelectionChangedEvent event) {
                SectionKey sel = getSectionKeyFromSelection();
                setEnabled(sel!=null);
            }
        };

        class GotoNedFileAction extends ActionExt {
            boolean gotoDecl;
            GotoNedFileAction(boolean gotoDecl) {
                super();
                this.gotoDecl = gotoDecl;
                updateLabel(null);
            }
            @Override
            public void run() {
                INedElement sel = getNedElementFromSelection();
                if (sel != null)
                    NedResourcesPlugin.openNedElementInEditor(sel);
            }
            @Override
            public void selectionChanged(SelectionChangedEvent event) {
                INedElement sel = getNedElementFromSelection();
                setEnabled(sel != null);
                updateLabel(sel);
            }
            private INedElement getNedElementFromSelection() {
                Object element = ((IStructuredSelection)treeViewer.getSelection()).getFirstElement();
                if (element instanceof GenericTreeNode)
                    element = ((GenericTreeNode)element).getPayload();
                if (element instanceof SubmoduleOrConnectionPayload) {
                    SubmoduleOrConnectionPayload payload = (SubmoduleOrConnectionPayload) element;
                    return gotoDecl ? payload.typeElement : payload.element;
                }
                if (element instanceof ParamResolution) {
                    ParamResolution res = (ParamResolution) element;
                    //return gotoDecl ? res.paramDeclNode : res.paramValueNode;
                    // experimental: disable "Open NED declaration" if it's the same as "Open NED value"
                    //return gotoDecl ? (res.paramDeclNode==res.paramValueNode ? null : res.paramDeclNode) : res.paramValueNode;
                    // experimental: disable "Open NED Value" if it's the same as the declaration
                    return gotoDecl ? res.paramDeclaration : (res.paramDeclaration==res.paramAssignment ? null : res.paramAssignment);
                }
                return null;
            }
            private void updateLabel(INedElement node) {
                if (gotoDecl) {
                    if (node instanceof ParamElement)
                        setText("Open NED Declaration");
                    else
                        setText("Open NED Declaration");
                }
                else {
                    if (node instanceof ParamElement)
                        setText("Open NED Value");
                    else
                        setText("Open NED Usage");
                }
            }
        };
        final ActionExt gotoNedAction = new GotoNedFileAction(false);
        ActionExt gotoNedDeclAction = new GotoNedFileAction(true);

        treeViewer.addSelectionChangedListener(gotoInifileAction);
        treeViewer.addSelectionChangedListener(gotoNedAction);
        treeViewer.addSelectionChangedListener(gotoNedDeclAction);

        // add double-click support to the tree
        treeViewer.addDoubleClickListener(new IDoubleClickListener() {
            public void doubleClick(DoubleClickEvent event) {
                gotoInifileAction.run();
            }
        });

        // build menus and toolbar
        contextMenuManager.add(gotoInifileAction);
        contextMenuManager.add(gotoNedAction);
        contextMenuManager.add(gotoNedDeclAction);
        contextMenuManager.add(new Separator());
        contextMenuManager.add(pinAction);

        IToolBarManager toolBarManager = getViewSite().getActionBars().getToolBarManager();
        toolBarManager.add(pinAction);

        IMenuManager menuManager = getViewSite().getActionBars().getMenuManager();
        menuManager.add(pinAction);
    }

    protected SectionKey getSectionKeyFromSelection() {
        Object element = ((IStructuredSelection)treeViewer.getSelection()).getFirstElement();
        if (element instanceof GenericTreeNode)
            element = ((GenericTreeNode)element).getPayload();
        if (element instanceof ParamResolution) {
            ParamResolution res = (ParamResolution) element;
            if (res.section != null && res.key != null)
                return new SectionKey(res.section, res.key);
        }
        return null;
    }

    @Override
    protected void showMessage(String text) {
        super.showMessage(text);
        inifileDocument = null;
        inifileAnalyzer = null;
        treeViewer.setInput(null);
    }

    @Override
    public void setFocus() {
        if (isShowingMessage())
            super.setFocus();
        else
            treeViewer.getTree().setFocus();
    }

    @Override
    public void buildContent(INedElement element, final InifileAnalyzer analyzer, final String section, String key) {
        this.inifileAnalyzer = analyzer;
        this.inifileDocument = analyzer==null ? null : analyzer.getDocument();
        this.section = section;
        this.contextProject = null;

        INedElement elementWithParameters = findAncestorWithParameters(element);
        if (elementWithParameters == null) {
            showMessage("No element with parameters selected.");
            return;
        }

        // build tree
        final GenericTreeNode root = new GenericTreeNode("root");

        NedFileElementEx nedFileElement = elementWithParameters.getContainingNedFileElement();
        INedTypeResolver nedResources = nedFileElement.getResolver();
        contextProject = nedResources.getNedFile(nedFileElement).getProject();

        if (elementWithParameters instanceof ISubmoduleOrConnection)
            computeChildren(root, (ISubmoduleOrConnection)elementWithParameters);
        else if (elementWithParameters instanceof INedTypeElement){
            INedTypeElement typeElement = (INedTypeElement)elementWithParameters;
            computeChildren(root, typeElement.getNedTypeInfo());
        }
        else {
            showMessage("Please select a submodule, a compound module, a simple module or a connection");
            return;
        }

        // prevent collapsing all treeviewer nodes: only set it on viewer if it's different from old input
        if (!GenericTreeUtils.treeEquals(root, (GenericTreeNode)treeViewer.getInput(), true)) {
            treeViewer.setInput(root);

            // open root node (useful in case preserving the selection fails)
            treeViewer.expandToLevel(2);

            // try to preserve selection
            ISelection oldSelection = selectedElements.get(getAssociatedEditor().getEditorInput());
            if (oldSelection != null)
                treeViewer.setSelection(oldSelection, true);
        }

        // refresh the viewer anyway, because e.g. parameter value changes are not reflected in the input tree
        treeViewer.refresh();

        // update label
        String text = "";
        if (elementWithParameters != null) {
            if (elementWithParameters instanceof IHasName)
                text = StringUtils.capitalize(elementWithParameters.getReadableTagName()) + " " + ((IHasName)elementWithParameters).getName();
            if (elementWithParameters instanceof SubmoduleElementEx)
                text += " of module " + ((SubmoduleElementEx)elementWithParameters).getCompoundModule().getName();
            if (getPinnedToEditor() != null)
                text += ", in " + getPinnedToEditor().getEditorInput().getName() + " (pinned)";
        }
        setContentDescription(text);
    }

    private void computeChildren(GenericTreeNode parent, INedTypeInfo typeInfo) {
        NedTreeTraversal iterator = new NedTreeTraversal(typeInfo.getResolver(), new TreeBuilder(parent, false), contextProject);
        iterator.traverse(typeInfo);
    }

    private void computeChildren(GenericTreeNode parent, ISubmoduleOrConnection element) {
        NedTreeTraversal iterator = new NedTreeTraversal(element.getResolver(), new TreeBuilder(parent, false), contextProject);
        iterator.traverse(element);
    }

    private void computeChildren(GenericTreeNode parent, ISubmoduleOrConnection element, INedTypeInfo typeInfo)
    {
        NedTreeTraversal iterator = new NedTreeTraversal(typeInfo.getResolver(), new TreeBuilder(parent, true), contextProject);
        if (element != null)
            iterator.traverse(element);
        else
            iterator.traverse(typeInfo);
    }

    class TreeBuilder implements IModuleTreeVisitor {
        private GenericTreeNode current;
        private boolean createNode;

        public TreeBuilder(GenericTreeNode root, boolean isContinued) {
            this.current = root;
            this.createNode = !isContinued;
        }

        public boolean enter(ISubmoduleOrConnection element, INedTypeInfo typeInfo) {
            //Debug.format("enter(%s,%s,%b)\n", element, typeInfo, createNode);
            if (!createNode) {
                addParamsAndProperties((SubmoduleOrConnectionNode)current, element);
                createNode = true;
                return true;
            }
            else {
                String fullName = element==null ? typeInfo.getName() : ParamUtil.getParamPathElementName(element);
                current = addTreeNode(current, fullName, element, typeInfo);
                return false;
            }
        }

        public void leave() {
            current = current.getParent();
        }

        public void unresolvedType(ISubmoduleOrConnection element, String typeName) {
            String fullName = element==null ? typeName : ParamUtil.getParamPathElementName(element);

            // skip if this is a zero-size submodule vector
            boolean isEmptySubmoduleVector = false;

            if (element != null && element instanceof SubmoduleElementEx) {
                String vectorSize = ((SubmoduleElementEx)element).getVectorSize();

                if (!StringUtils.isEmpty(vectorSize)) {
                    if (vectorSize.equals("0"))
                        isEmptySubmoduleVector = true;
                    else if (vectorSize.matches("[a-zA-Z_][a-zA-Z0-9_]*")) {  //XXX performance (precompile regex!)
                        INedTypeInfo paramDeclarationOwnerTypeInfo = current instanceof SubmoduleOrConnectionNode ? ((SubmoduleOrConnectionNode)current).typeInfo : null;

                        if (paramDeclarationOwnerTypeInfo != null) {
                            ParamElementEx paramDeclaration = paramDeclarationOwnerTypeInfo.getParamDeclarations().get(vectorSize);

                            if (paramDeclaration != null) {
                                String paramValue;
                                if (current instanceof SubmoduleOrConnectionNode) {
                                    SubmoduleOrConnectionNode treeNode = (SubmoduleOrConnectionNode)current;
                                    paramValue = resolveParamValue(treeNode.getFullPath(), treeNode.getTypeInfoPath(), treeNode.getElementPath(), paramDeclaration);
                                }
                                else
                                    paramValue = resolveParamValue("", new Vector<INedTypeInfo>(), new Vector<ISubmoduleOrConnection>(), paramDeclaration);

                                if ("0".equals(paramValue))
                                    isEmptySubmoduleVector = true;
                            }
                        }
                    }
                }
            }

            if (isEmptySubmoduleVector)
                current.addChild(new GenericTreeNode(new InformationNode(fullName + " : empty vector")));
            else
                current.addChild(new GenericTreeNode(new ErrorNode(fullName + " : unresolved type '" + typeName + "'")));
        }

        public void recursiveType(ISubmoduleOrConnection element, INedTypeInfo typeInfo) {
            String fullName = element==null ? typeInfo.getName() : ParamUtil.getParamPathElementName(element);
            current.addChild(new GenericTreeNode(new ErrorNode(fullName+" : "+typeInfo.getName()+" -- recursive use of type '"+typeInfo.getName()+"'")));
        }

        public String resolveLikeType(ISubmoduleOrConnection element) {
            try {
                if (inifileAnalyzer == null)
                    return null;  //FIXME now we have default typename, so we can do more...

                String moduleFullPath = current instanceof SubmoduleOrConnectionNode ? ((SubmoduleOrConnectionNode)current).getFullPath() : "";
                Timeout timeout = new Timeout(100); //XXX
                return InifileUtils.resolveLikeExpr(moduleFullPath, element, section, inifileAnalyzer, inifileDocument, timeout);
            } catch (ParamResolutionDisabledException e) {
                return null;
            } catch (ParamResolutionTimeoutException e) {
                return null;
            }
        }

        private String resolveParamValue(String fullPath, Vector<INedTypeInfo> typeInfoPath, Vector<ISubmoduleOrConnection> elementPath, ParamElementEx paramDeclaration) {
            ArrayList<ParamResolution> paramResolutions = new ArrayList<ParamResolution>();
            String[] sectionChain = inifileDocument.getSectionChain(section);
            ParamCollector.resolveParameter(paramResolutions, fullPath, typeInfoPath, elementPath, sectionChain, inifileDocument, paramDeclaration);

            if (paramResolutions.size() == 1) {
                return InifileUtils.getParamValue(paramResolutions.get(0), inifileDocument);
            }
            else
                return null;
        }
    }


    /**
     * Adds a node to the tree. The node children is not yet computed.
     */
    private SubmoduleOrConnectionNode addTreeNode(GenericTreeNode parent, String moduleFullName,
            ISubmoduleOrConnection element, INedTypeInfo typeInfo) {

        String moduleText = moduleFullName+"  ("+typeInfo.getName()+")";
        SubmoduleOrConnectionNode treeNode = new SubmoduleOrConnectionNode(new SubmoduleOrConnectionPayload(moduleText, element, typeInfo.getNedElement()),
                                            moduleFullName, element, typeInfo);
        parent.addChild(treeNode);
        return treeNode;
    }

    /**
     * Adds a the node parameters, signals and statistics to the tree.
     */
    private void addParamsAndProperties(SubmoduleOrConnectionNode treeNode, ISubmoduleOrConnection element) {
        Vector<ISubmoduleOrConnection> elementPath = treeNode.getElementPath();
        if (inifileAnalyzer == null) {
            // no inifile available, we only have NED info
            String fullPath = treeNode.getFullPath();
            Vector<INedTypeInfo> typeInfoPath = treeNode.getTypeInfoPath();

            ArrayList<ParamResolution> paramResolutions = new ArrayList<ParamResolution>();
            ParamCollector.resolveModuleParameters(paramResolutions, fullPath, typeInfoPath, elementPath, null, null);
            addParamResolutions(treeNode, paramResolutions.toArray(new ParamResolution[0]));

            ArrayList<PropertyResolution> propertyResolutions = new ArrayList<PropertyResolution>();
            ParamCollector.resolveModuleProperties("signal", propertyResolutions, fullPath, typeInfoPath, elementPath, null);
            addSignalResolutions(treeNode, propertyResolutions.toArray(new PropertyResolution[0]));

            propertyResolutions = new ArrayList<PropertyResolution>();
            ParamCollector.resolveModuleProperties("statistic", propertyResolutions, fullPath, typeInfoPath, elementPath, null);
            addStatisticResolutions(treeNode, propertyResolutions.toArray(new PropertyResolution[0]));
        }
        else {
            if (inifileAnalyzer.isParamResolutionEnabled()) {
                try {
                    ITimeout timeout = new Timeout(300); //XXX
                    ParamResolution[] params = inifileAnalyzer.getParamResolutionsForModule(elementPath, section, timeout);
                    PropertyResolution[] signals = inifileAnalyzer.getPropertyResolutionsForModule("signal", elementPath, section, timeout);
                    PropertyResolution[] statistics = inifileAnalyzer.getPropertyResolutionsForModule("statistic", elementPath, section, timeout);
                    addParamResolutions(treeNode, params);
                    addSignalResolutions(treeNode, signals);
                    addStatisticResolutions(treeNode, statistics);
                } catch (ParamResolutionDisabledException e) {
                    // XXX oops, turned off during this call?
                } catch (ParamResolutionTimeoutException e) {
                    treeNode.addChild(new GenericTreeNode(new InformationNode("No details, because ini file analysis has timed out.")));
                }
            }
            else {
                treeNode.addChild(new GenericTreeNode(new InformationNode("No details, because ini file analysis is turned off.")));
            }
        }
    }

    // TODO optimize: param and property resolutions can stop at first element found
    private boolean hasChildren(SubmoduleOrConnectionNode treeNode, ISubmoduleOrConnection element) {
        // check submodules and connections
        if (treeNode.typeInfo.getNedElement() instanceof CompoundModuleElementEx) {
            CompoundModuleElementEx compoundModule = (CompoundModuleElementEx)treeNode.typeInfo.getNedElement();

            List<SubmoduleElementEx> submodules = compoundModule.getSubmodules();
            if (!submodules.isEmpty())
                return true;
            if (!compoundModule.getSrcConnections().isEmpty())
                return true;
            for (SubmoduleElementEx submodule : submodules)
                if(!compoundModule.getSrcConnectionsFor(submodule.getName()).isEmpty())
                    return true;
        }

        // check parameters and properties
        if (inifileAnalyzer == null) {
            // no inifile available, we only have NED info
            String fullPath = treeNode.getFullPath();
            Vector<ISubmoduleOrConnection> elementPath = treeNode.getElementPath();
            Vector<INedTypeInfo> typeInfoPath = treeNode.getTypeInfoPath();

            ArrayList<ParamResolution> paramResolutions = new ArrayList<ParamResolution>();
            ParamCollector.resolveModuleParameters(paramResolutions, fullPath, typeInfoPath, elementPath, null, null);
            if (!paramResolutions.isEmpty())
                return true;

            ArrayList<PropertyResolution> propertyResolutions = new ArrayList<PropertyResolution>();
            ParamCollector.resolveModuleProperties("signal", propertyResolutions, fullPath, typeInfoPath, elementPath, null);
            if (!propertyResolutions.isEmpty())
                return true;

            propertyResolutions = new ArrayList<PropertyResolution>();
            ParamCollector.resolveModuleProperties("statistic", propertyResolutions, fullPath, typeInfoPath, elementPath, null);
            if (!propertyResolutions.isEmpty())
                return true;
        }
        else
        {
            if (!inifileAnalyzer.isParamResolutionEnabled())
                return true;

            try {
                Timeout timeout = new Timeout(0);
                Vector<ISubmoduleOrConnection> elementPath = treeNode.getElementPath();
                ParamResolution[] params = inifileAnalyzer.getParamResolutionsForModule(elementPath, section, timeout);
                if (params.length > 0)
                    return true;
                PropertyResolution[] signals = inifileAnalyzer.getPropertyResolutionsForModule("signal", elementPath, section, timeout);
                if (signals.length > 0)
                    return true;
                PropertyResolution[] statistics = inifileAnalyzer.getPropertyResolutionsForModule("statistic", elementPath, section, timeout);
                if (statistics.length > 0)
                    return true;
            } catch (ParamResolutionDisabledException e) {

            } catch (ParamResolutionTimeoutException e) {
                return true;
            }
        }

        return false;
    }

    protected void addParamResolutions(SubmoduleOrConnectionNode node, ParamResolution[] paramResolutions) {
        for (int i = 0; i < paramResolutions.length; i++) {
            ParamResolution paramResolution = paramResolutions[i];
            ParamAssignmentGroupNode child = new ParamAssignmentGroupNode(paramResolution.paramDeclaration.getName() + " = ");
            Set<String> values = new LinkedHashSet<String>();

            // group subsequent param resolutions that assign value to the same param declaration
            for (int j = i; j < paramResolutions.length && paramResolutions[j].paramDeclaration == paramResolution.paramDeclaration; j++) {
                ParamResolution p = paramResolutions[j];
                child.addChild(new ParamAssignmentGroupElementNode(paramResolutions[j]));
                values.add(InifileUtils.getParamValue(p, inifileDocument, false));
                i = j;
            }

            if (child.getChildCount() == 1)
                node.addChild(new GenericTreeNode(paramResolution));
            else {
                child.setPayload(child.getPayload() + StringUtils.join(values, ", "));
                node.addChild(child);
            }
        }
    }

    protected void addSignalResolutions(SubmoduleOrConnectionNode node, PropertyResolution[] signalResolutions) {
        for (PropertyResolution signalResolution : signalResolutions) {
            String label = signalResolution.propertyDeclaration.getIndex();
            String type = signalResolution.propertyDeclaration.getValue("type");
            if (type != null)
                label += " : " + type;
            label += " (signal)";
            node.addChild(new SignalNode(label));
        }
    }

    protected void addStatisticResolutions(SubmoduleOrConnectionNode node, PropertyResolution[] statisticResolutions) {
        for (PropertyResolution statisticResolution : statisticResolutions) {
            String label = statisticResolution.propertyDeclaration.getIndex();
            String title = statisticResolution.propertyDeclaration.getValue("title");
            if (!StringUtils.isEmpty(title))
                label += " \"" + title + "\"";
            label += " (statistic)";
            node.addChild(new StatisticNode(label));
        }
    }

    protected String getLabelFor(ParamResolution res, IReadonlyInifileDocument doc, boolean includeName) {
        String value = InifileUtils.getParamValue(res, doc);
        String remark = InifileUtils.getParamRemark(res, doc);
        return (includeName ? res.paramDeclaration.getName() : "") + " = " + (value==null ? "" : value+" ") + "(" + remark + ")";
    }
}
