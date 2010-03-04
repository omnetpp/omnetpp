/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.views;

import java.util.ArrayList;
import java.util.LinkedHashSet;
import java.util.Set;
import java.util.Stack;
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
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.common.util.ActionExt;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.IGotoInifile;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileHoverUtils;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.ParamResolution;
import org.omnetpp.inifile.editor.model.PropertyResolution;
import org.omnetpp.inifile.editor.model.SectionKey;
import org.omnetpp.ned.core.IModuleTreeVisitor;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.core.NedTreeTraversal;
import org.omnetpp.ned.core.ParamUtil;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.NedTreeUtil;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IChannelKindTypeElement;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.IModuleKindTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;
import org.omnetpp.ned.model.pojo.ParamElement;

/**
 * Displays NED module hierarchy with module parameters, and
 * optionally, values assigned in ini files.
 *
 * @author Andras
 */
public class ModuleHierarchyView extends AbstractModuleView {
	private TreeViewer treeViewer;
	private IInifileDocument inifileDocument; // corresponds to the current selection; needed by the label provider
	private InifileAnalyzer inifileAnalyzer; // corresponds to the current selection; unfortunately needed by the hover

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
	private static class SubmoduleOrConnectionNode {
		String fullPath;
		ISubmoduleOrConnection element; // null at the root
		INedTypeElement typeElement; // null if type is unknown

		/* for convenience */
		public SubmoduleOrConnectionNode(String moduleFullPath, ISubmoduleOrConnection element, INedTypeElement typeElement) {
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
			final SubmoduleOrConnectionNode other = (SubmoduleOrConnectionNode) obj;
			return fullPath.equals(other.fullPath) &&
				   element == other.element &&
				   typeElement == other.typeElement;
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
				if (element instanceof SubmoduleOrConnectionNode) {
					SubmoduleOrConnectionNode mn = (SubmoduleOrConnectionNode) element;
					if (mn.element == null)
						return NedTreeUtil.getNedModelLabelProvider().getImage(mn.typeElement);
					if (mn.typeElement == null)
						return NedTreeUtil.getNedModelLabelProvider().getImage(mn.element);
					// for a "like" submodule, use icon of the concrete module type
		            DisplayString dps = mn.element instanceof SubmoduleElementEx ?
	                    ((SubmoduleElementEx)mn.element).getDisplayString((IModuleKindTypeElement)mn.typeElement) :
	                    ((ConnectionElementEx)mn.element).getDisplayString((IChannelKindTypeElement)mn.typeElement);
		            Image image = ImageFactory.getIconImage(dps.getAsString(IDisplayString.Prop.IMAGE));
					return image!=null ? image : NedTreeUtil.getNedModelLabelProvider().getImage(mn.element);
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
				if (element instanceof SubmoduleOrConnectionNode) {
					SubmoduleOrConnectionNode mn = (SubmoduleOrConnectionNode) element;
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
 		new HoverSupport().adapt(treeViewer.getTree(), new IHoverTextProvider() {
			public String getHoverTextFor(Control control, int x, int y, SizeConstraint outPreferredSize) {
				Item item = treeViewer.getTree().getItem(new Point(x,y));
				Object element = item==null ? null : item.getData();
				if (element instanceof GenericTreeNode)
					element = ((GenericTreeNode)element).getPayload();
				if (element instanceof ParamResolution) {
					ParamResolution res = (ParamResolution) element;
					if (res.section != null && res.key != null)
						//XXX make sure "res" and inifile editor refer to the same IFile!!!
						return InifileHoverUtils.getEntryHoverText(res.section, res.key, inifileDocument, inifileAnalyzer);
					else
						return InifileHoverUtils.getParamHoverText(res.elementPath, res.paramDeclaration, res.paramAssignment);
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
				if (element instanceof SubmoduleOrConnectionNode) {
					SubmoduleOrConnectionNode payload = (SubmoduleOrConnectionNode) element;
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

		INedElement elementWithParameters = findAncestorWithParameters(element);
		if (elementWithParameters == null) {
		    showMessage("No element with parameters selected.");
		    return;
		}

		// build tree
        final GenericTreeNode root = new GenericTreeNode("root");
    	class TreeBuilder implements IModuleTreeVisitor {
    		private GenericTreeNode current = root;
			private Stack<String> fullPathStack = new Stack<String>();
			private Stack<ISubmoduleOrConnection> elementPath = new Stack<ISubmoduleOrConnection>();
			private Stack<INedTypeInfo> typeInfoPath = new Stack<INedTypeInfo>();

    		public boolean enter(ISubmoduleOrConnection element, INedTypeInfo typeInfo) {
    			String fullName = element==null ? typeInfo.getName() : ParamUtil.getParamPathElementName(element);
				fullPathStack.push(fullName);
				elementPath.push(element);
				typeInfoPath.push(typeInfo);
				String fullPath = StringUtils.join(fullPathStack.toArray(), "."); //XXX optimize here if slow
    			current = addTreeNode(current, fullName, fullPath, elementPath, typeInfoPath, section, analyzer);
    			return true;
    		}

    		public void leave() {
    			current = current.getParent();
				fullPathStack.pop();
				elementPath.pop();
				typeInfoPath.pop();
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
                            INedTypeInfo paramDeclarationOwnerTypeInfo = typeInfoPath.size() != 0 ? typeInfoPath.get(typeInfoPath.size() - 1) : null;

                            if (paramDeclarationOwnerTypeInfo != null) {
                                ParamElementEx paramDeclaration = paramDeclarationOwnerTypeInfo.getParamDeclarations().get(vectorSize);

                                if (paramDeclaration != null) {
                                    String fullPath = StringUtils.join(fullPathStack, ".");

                                    if ("0".equals(resolveParamValue(fullPath, typeInfoPath, elementPath, paramDeclaration)))
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
    			if (analyzer == null)
    				return null;
				String moduleFullPath = StringUtils.join(fullPathStack.toArray(), ".");
				return InifileUtils.resolveLikeParam(moduleFullPath, element, section, analyzer, inifileDocument);
    		}

            private String resolveParamValue(String fullPath, Vector<INedTypeInfo> typeInfoPath, Vector<ISubmoduleOrConnection> elementPath, ParamElementEx paramDeclaration) {
                ArrayList<ParamResolution> paramResolutions = new ArrayList<ParamResolution>();
                String[] sectionChain = InifileUtils.resolveSectionChain(inifileDocument, section);
                InifileAnalyzer.resolveParameter(paramResolutions, fullPath, typeInfoPath, elementPath, sectionChain, inifileDocument, paramDeclaration);

                if (paramResolutions.size() == 1) {
                    return InifileAnalyzer.getParamValue(paramResolutions.get(0), inifileDocument);
                }
                else
                    return null;
            }
    	}

    	INedTypeResolver nedResources = NedResourcesPlugin.getNedResources();
    	IProject contextProject = nedResources.getNedFile(elementWithParameters.getContainingNedFileElement()).getProject();
    	NedTreeTraversal iterator = new NedTreeTraversal(nedResources, new TreeBuilder());
        if (elementWithParameters instanceof ISubmoduleOrConnection)
            iterator.traverse((ISubmoduleOrConnection)elementWithParameters);
        else if (elementWithParameters instanceof INedTypeElement){
            INedTypeElement typeElement = (INedTypeElement)elementWithParameters;
            iterator.traverse(typeElement.getNedTypeInfo().getFullyQualifiedName(), contextProject);
        }
        else {
        	showMessage("Please select a submodule, a compound module, a simple module or a connection");
        	return;
        }

		// prevent collapsing all treeviewer nodes: only set it on viewer if it's different from old input
		if (!GenericTreeUtils.treeEquals(root, (GenericTreeNode)treeViewer.getInput())) {
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

	/**
	 * Adds a node to the tree. The new node describes the module and its parameters.
	 */
	private GenericTreeNode addTreeNode(GenericTreeNode parent, String moduleFullName, String fullPath, Vector<ISubmoduleOrConnection> elementPath, Vector<INedTypeInfo> typeInfoPath, String activeSection, InifileAnalyzer analyzer) {
	    INedTypeInfo typeInfo = typeInfoPath.lastElement();
		String moduleText = moduleFullName+"  ("+typeInfo.getName()+")";
		ISubmoduleOrConnection lastElement = elementPath.lastElement();
        GenericTreeNode treeNode = new GenericTreeNode(new SubmoduleOrConnectionNode(moduleText, lastElement, typeInfo.getNedElement()));
		parent.addChild(treeNode);

		if (analyzer == null) {
			// no inifile available, we only have NED info
		    ArrayList<ParamResolution> paramResolutions = new ArrayList<ParamResolution>();
			InifileAnalyzer.resolveModuleParameters(paramResolutions, fullPath, typeInfoPath, elementPath);
			addParamResolutions(treeNode, paramResolutions.toArray(new ParamResolution[0]));
            ArrayList<PropertyResolution> propertyResolutions = new ArrayList<PropertyResolution>();
            InifileAnalyzer.resolveModuleProperties("signal", propertyResolutions, fullPath, typeInfoPath, elementPath);
            addSignalResolutions(treeNode, propertyResolutions.toArray(new PropertyResolution[0]));
            propertyResolutions = new ArrayList<PropertyResolution>();
            InifileAnalyzer.resolveModuleProperties("statistic", propertyResolutions, fullPath, typeInfoPath, elementPath);
            addStatisticResolutions(treeNode, propertyResolutions.toArray(new PropertyResolution[0]));
		}
		else {
		    addParamResolutions(treeNode, analyzer.getParamResolutionsForModule(lastElement, activeSection));
		    addSignalResolutions(treeNode, analyzer.getPropertyResolutionsForModule("signal", lastElement, activeSection));
            addStatisticResolutions(treeNode, analyzer.getPropertyResolutionsForModule("statistic", lastElement, activeSection));
		}

		return treeNode;
	}

	protected void addParamResolutions(GenericTreeNode node, ParamResolution[] paramResolutions) {
        for (int i = 0; i < paramResolutions.length; i++) {
            ParamResolution paramResolution = paramResolutions[i];
            ParamAssignmentGroupNode child = new ParamAssignmentGroupNode(paramResolution.paramDeclaration.getName() + " = ");
            Set<String> values = new LinkedHashSet<String>();

            // group subsequent param resolutions that assign value to the same param declaration
            for (int j = i; j < paramResolutions.length && paramResolutions[j].paramDeclaration == paramResolution.paramDeclaration; j++) {
                ParamResolution p = paramResolutions[j];
                child.addChild(new ParamAssignmentGroupElementNode(paramResolutions[j]));
                values.add(InifileAnalyzer.getParamValue(p, inifileDocument, false));
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

	protected void addSignalResolutions(GenericTreeNode node, PropertyResolution[] signalResolutions) {
        for (PropertyResolution signalResolution : signalResolutions) {
            String label = signalResolution.propertyDeclaration.getIndex();
            String type = signalResolution.propertyDeclaration.getValue("type");
            if (type != null)
                label += " : " + type;
            node.addChild(new SignalNode(label));
        }
	}

    protected void addStatisticResolutions(GenericTreeNode node, PropertyResolution[] statisticResolutions) {
        for (PropertyResolution statisticResolution : statisticResolutions) {
            String label = statisticResolution.propertyDeclaration.getIndex() + " : ";
            String record = statisticResolution.propertyDeclaration.getValue("record");
            if (record != null)
                label += record;
            String source = statisticResolution.propertyDeclaration.getValue("source");
            if (source != null)
                label += " of " + source;
            node.addChild(new StatisticNode(label));
        }
    }

	protected String getLabelFor(ParamResolution res, IInifileDocument doc, boolean includeName) {
		String value = InifileAnalyzer.getParamValue(res, doc);
		String remark = InifileAnalyzer.getParamRemark(res, doc);
		return (includeName ? res.paramDeclaration.getName() : "") + " = " + (value==null ? "" : value+" ") + "(" + remark + ")";
	}
}
