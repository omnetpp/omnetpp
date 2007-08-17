package org.omnetpp.inifile.editor.views;

import java.util.Stack;
import java.util.WeakHashMap;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Item;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.ui.GenericTreeUtils;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.IGotoInifile;
import org.omnetpp.inifile.editor.actions.ActionExt;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.IModuleTreeVisitor;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileHoverUtils;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.NEDTreeTraversal;
import org.omnetpp.inifile.editor.model.ParamResolution;
import org.omnetpp.inifile.editor.model.SectionKey;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.NEDTreeUtil;
import org.omnetpp.ned.model.ex.NEDElementUtilEx;
import org.omnetpp.ned.model.ex.SubmoduleNodeEx;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.IModuleTypeNode;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.pojo.CompoundModuleNode;
import org.omnetpp.ned.model.pojo.ParamNode;
import org.omnetpp.ned.model.pojo.SimpleModuleNode;
import org.omnetpp.ned.model.pojo.SubmoduleNode;

/**
 * Displays NED module hierarchy with module parameters, and
 * optionally, values assigned in ini files.
 *
 * @author Andras
 */
//XXX "like" submodule with unresolved type does not appear as such!!!
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
	 * Node contents for the GenericTreeNode tree that is displayed in the view
	 */
	private static class ModuleNode {
		String moduleFullPath;
		SubmoduleNodeEx submoduleNode; // null at the root
		IModuleTypeNode submoduleType; // CompoundModuleNode or SimpleModuleNode; null if type is unknown

		/* for convenience */
		public ModuleNode(String moduleFullPath, SubmoduleNodeEx submoduleNode, IModuleTypeNode submoduleType) {
			this.moduleFullPath = moduleFullPath;
			this.submoduleNode = submoduleNode;
			this.submoduleType = submoduleType;
			//System.out.println("PATH="+moduleFullPath+" Node="+submoduleNode+" SubmoduleType="+(submoduleNode==null ? null : submoduleNode.getType())+"  Type="+submoduleType);
		}

		/* Needed for GenericTreeUtil.treeEquals() */
		@Override
		public boolean equals(Object obj) {
			if (this == obj)
				return true;
			if (obj == null || getClass() != obj.getClass())
				return false;
			final ModuleNode other = (ModuleNode) obj;
			return moduleFullPath.equals(other.moduleFullPath) &&
				   submoduleNode == other.submoduleNode &&
				   submoduleType == other.submoduleType;
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
				if (element instanceof GenericTreeNode)
					element = ((GenericTreeNode)element).getPayload();
				if (element instanceof ModuleNode) {
					ModuleNode mn = (ModuleNode) element;
					if (mn.submoduleNode == null)
						return NEDTreeUtil.getNedModelLabelProvider().getImage(mn.submoduleType);
					if (mn.submoduleType == null)
						return NEDTreeUtil.getNedModelLabelProvider().getImage(mn.submoduleNode);
					// for a "like" submodule, use icon of the concrete module type
		            DisplayString dps = mn.submoduleNode.getDisplayString(mn.submoduleType);
		            Image image = ImageFactory.getIconImage(dps.getAsString(IDisplayString.Prop.IMAGE));
					return image!=null ? image : NEDTreeUtil.getNedModelLabelProvider().getImage(mn.submoduleNode);
				}
				else if (element instanceof ParamResolution)
					return suggestImage(((ParamResolution) element).type);
				else if (element instanceof ErrorNode)
					return ICON_ERROR;
				else
					return null;
			}

			@Override
			public String getText(Object element) {
				if (element instanceof GenericTreeNode)
					element = ((GenericTreeNode)element).getPayload();
				if (element instanceof ModuleNode) {
					ModuleNode mn = (ModuleNode) element;
					if (mn.submoduleNode == null) // this is the tree root
						return mn.submoduleType.getName();
					String typeName = mn.submoduleNode.getType();
		            String label = mn.submoduleNode.getName()+bracketizeIfNotEmpty(mn.submoduleNode.getVectorSize())+" : ";
		            if (typeName != null && !typeName.equals(""))
		            	label += typeName;
		            else if (mn.submoduleType != null)
		            	label += mn.submoduleType.getName();
		            else
		                label += "like "+mn.submoduleNode.getLikeType();
		            return label;
				}
				else if (element instanceof ParamResolution)
					return getLabelFor((ParamResolution) element, inifileDocument);
				else
					return element.toString();
			}

			private String bracketizeIfNotEmpty(String attr) {
				return (attr==null || attr.equals("")) ? "" : "["+attr+"]";
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
					if (sel != null && editor instanceof IGotoInifile)
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
						return InifileHoverUtils.getParamHoverText(res.pathModules, res.paramDeclNode, res.paramValueNode);
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
				INEDElement sel = getNEDElementFromSelection();
				if (sel != null)
					NEDResourcesPlugin.openNEDElementInEditor(sel);
			}
			public void selectionChanged(SelectionChangedEvent event) {
				INEDElement sel = getNEDElementFromSelection();
				setEnabled(sel != null);
				updateLabel(sel);
			}
			private INEDElement getNEDElementFromSelection() {
				Object element = ((IStructuredSelection)treeViewer.getSelection()).getFirstElement();
				if (element instanceof GenericTreeNode)
					element = ((GenericTreeNode)element).getPayload();
				if (element instanceof ModuleNode) {
					ModuleNode payload = (ModuleNode) element;
					return gotoDecl ? payload.submoduleType : payload.submoduleNode;
				}
				if (element instanceof ParamResolution) {
					ParamResolution res = (ParamResolution) element;
					//return gotoDecl ? res.paramDeclNode : res.paramValueNode; 
					// experimental: disable "Open NED declaration" if it's the same as "Open NED value"
					//return gotoDecl ? (res.paramDeclNode==res.paramValueNode ? null : res.paramDeclNode) : res.paramValueNode;
					// experimental: disable "Open NED Value" if it's the same as the declaration
					return gotoDecl ? res.paramDeclNode : (res.paramDeclNode==res.paramValueNode ? null : res.paramValueNode);
				}
				return null;
			}
			private void updateLabel(INEDElement node) {
				if (gotoDecl) {
					if (node instanceof ParamNode)
						setText("Open NED Declaration");
					else
						setText("Open NED Declaration");
				}
				else {
					if (node instanceof ParamNode)
						setText("Open NED Value");
					else
						setText("Open NED Submodule");
				}
			}
		};
 		final ActionExt gotoNedAction = new GotoNedFileAction(false); 
		ActionExt gotoNedDeclAction = new GotoNedFileAction(true); 
	
		treeViewer.addSelectionChangedListener(gotoInifileAction);
		treeViewer.addSelectionChangedListener(gotoNedAction);
		treeViewer.addSelectionChangedListener(gotoNedDeclAction);
	
		// add double-click support to the tree
		treeViewer.getTree().addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetDefaultSelected(SelectionEvent e) {
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

	public void buildContent(INEDElement module, final InifileAnalyzer analyzer, final String section, String key) {
		this.inifileAnalyzer = analyzer;
		this.inifileDocument = analyzer==null ? null : analyzer.getDocument();

		// build tree
        final GenericTreeNode root = new GenericTreeNode("root");
    	class TreeBuilder implements IModuleTreeVisitor {
    		private GenericTreeNode current = root;
			private Stack<String> fullPathStack = new Stack<String>();

    		public void enter(SubmoduleNode submodule, INEDTypeInfo submoduleType) {
    			String fullName = submodule==null ? submoduleType.getName() : InifileUtils.getSubmoduleFullName(submodule);
				fullPathStack.push(fullName);
				String fullPath = StringUtils.join(fullPathStack.toArray(), "."); //XXX optimize here if slow
    			current = addTreeNode(current, fullName, fullPath, submoduleType, submodule, section, analyzer);
    		}
    		public void leave() {
    			current = current.getParent();
				fullPathStack.pop();
    		}
    		public void unresolvedType(SubmoduleNode submodule, String submoduleTypeName) {
    			String fullName = submodule==null ? submoduleTypeName : InifileUtils.getSubmoduleFullName(submodule);
    			current.addChild(new GenericTreeNode(new ErrorNode(fullName+" : unresolved type '"+submoduleTypeName+"'")));
    		}
    		public void recursiveType(SubmoduleNode submodule, INEDTypeInfo submoduleType) {
    			String fullName = submodule==null ? submoduleType.getName() : InifileUtils.getSubmoduleFullName(submodule);
    			current.addChild(new GenericTreeNode(new ErrorNode(fullName+" : "+submoduleType.getName()+" -- recursive use of type '"+submoduleType.getName()+"'")));
    		}
    		public String resolveLikeType(SubmoduleNode submodule) {
    			if (analyzer == null)
    				return null;
				String moduleFullPath = StringUtils.join(fullPathStack.toArray(), ".");
				return InifileUtils.resolveLikeParam(moduleFullPath, submodule, section, analyzer, inifileDocument);
    		}
    	}

    	INEDTypeResolver nedResources = NEDResourcesPlugin.getNEDResources();
    	NEDTreeTraversal iterator = new NEDTreeTraversal(nedResources, new TreeBuilder());
        if (module instanceof SubmoduleNode) {
            SubmoduleNode submodule = (SubmoduleNode)module;
            iterator.traverse(submodule);
        }
        else if (module instanceof CompoundModuleNode){
        	CompoundModuleNode compoundModule = (CompoundModuleNode)module;
            iterator.traverse(compoundModule.getName());
        }
        else if (module instanceof SimpleModuleNode){
        	SimpleModuleNode simpleModule = (SimpleModuleNode)module;
            iterator.traverse(simpleModule.getName());
        }
        else {
        	showMessage("Please select a submodule, compound module or simple module");
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
		if (module != null) {
			if (module instanceof IHasName)
				text = StringUtils.capitalize(NEDElementUtilEx.getReadableTagName(module)) + " " + ((IHasName)module).getName(); 
			if (module instanceof SubmoduleNodeEx)
				text += " of module " + ((SubmoduleNodeEx)module).getCompoundModule().getName();
			if (getPinnedToEditor() != null)
				text += ", in " + getPinnedToEditor().getEditorInput().getName() + " (pinned)";
		}
		setContentDescription(text);
	}

	/**
	 * Adds a node to the tree. The new node describes the module and its parameters.
	 */
	private static GenericTreeNode addTreeNode(GenericTreeNode parent, String moduleFullName, String moduleFullPath, INEDTypeInfo moduleType, SubmoduleNode thisSubmodule, String activeSection, InifileAnalyzer analyzer) {
		String moduleText = moduleFullName+"  ("+moduleType.getName()+")";
		GenericTreeNode thisNode = new GenericTreeNode(new ModuleNode(moduleText, (SubmoduleNodeEx)thisSubmodule, (IModuleTypeNode)moduleType.getNEDElement()));
		parent.addChild(thisNode);

		if (analyzer == null) {
			// no inifile available, we only have NED info
			ParamResolution[] list = InifileAnalyzer.resolveModuleParameters(moduleFullPath, (SubmoduleNodeEx)thisSubmodule, moduleType);
			for (ParamResolution res : list)
				thisNode.addChild(new GenericTreeNode(res));
		}
		else {
			ParamResolution[] list = analyzer.getParamResolutionsForModule(moduleFullPath, activeSection);
			for (ParamResolution res : list)
				thisNode.addChild(new GenericTreeNode(res));
		}
		return thisNode;
	}

	protected static String getLabelFor(ParamResolution res, IInifileDocument doc) {
		String value = InifileAnalyzer.getParamValue(res, doc);
		String remark = InifileAnalyzer.getParamRemark(res, doc);
		return res.paramDeclNode.getName() + " = " + (value==null ? "" : value+" ") + "(" + remark + ")";
	}

}
