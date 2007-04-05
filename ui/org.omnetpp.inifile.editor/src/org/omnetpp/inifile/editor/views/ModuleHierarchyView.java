package org.omnetpp.inifile.editor.views;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.ui.GenericTreeUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.ParamResolution;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.NEDTreeUtil;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.pojo.SubmoduleNode;
import org.omnetpp.ned.resources.NEDResourcesPlugin;

/**
 * Displays NED module hierarchy with module parameters, and 
 * optionally, values assigned in ini files.
 * 
 * @author Andras
 */
//XXX create another view: Hierarchy (inheritance tree); and call this Usage? Nesting? Tree? Containment?
//XXX follow selection
//XXX context menu with "Go to NED file" and "Go to ini file"
public class ModuleHierarchyView extends AbstractModuleView {
	private TreeViewer treeViewer;
	private IInifileDocument inifileDocument; // corresponds to the current selection; unfortunately needed by the label provider

	/**
	 * Node contents for the GenericTreeNode tree that is displayed in the view
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
			} else if (!text.equals(other.text))
				return false;
			return true;
		}
	}

	/**
	 * Node contents for the GenericTreeNode tree that is displayed in the view
	 */
	private static class ModuleNode {
		String moduleFullPath;
		NEDElement node;  // SubmoduleNode (or at root: CompoundModuleNode or SimpleModuleNode)

		/* for convenience */
		public ModuleNode(String moduleFullPath, NEDElement node) {
			this.moduleFullPath = moduleFullPath;
			this.node = node;
		}

		/* Generated; needed for GenericTreeUtil.treeEquals() */
		@Override
		public boolean equals(Object obj) {
			if (this == obj)
				return true;
			if (obj == null)
				return false;
			if (getClass() != obj.getClass())
				return false;
			final ModuleNode other = (ModuleNode) obj;
			if (moduleFullPath == null) {
				if (other.moduleFullPath != null)
					return false;
			} else if (!moduleFullPath.equals(other.moduleFullPath))
				return false;
			if (node == null) {
				if (other.node != null)
					return false;
			} else if (!node.equals(other.node))
				return false;
			return true;
		}
	}

	public ModuleHierarchyView() {
	}

	@Override
	public Control createViewControl(Composite parent) {
		treeViewer = new TreeViewer(parent, SWT.SINGLE);
		treeViewer.setLabelProvider(new LabelProvider() {
			@Override
			public Image getImage(Object element) {
				if (element instanceof GenericTreeNode) 
					element = ((GenericTreeNode)element).getPayload();
				if (element instanceof ModuleNode)
					return NEDTreeUtil.getNedModelLabelProvider().getImage(((ModuleNode) element).node);
				else if (element instanceof ParamResolution)
					return suggestImage(((ParamResolution) element).type);
				else if (element instanceof ErrorNode)
					return InifileEditorPlugin.getImage(ICON_ERROR);
				else
					return null;
			}

			@Override
			public String getText(Object element) {
				if (element instanceof GenericTreeNode) 
					element = ((GenericTreeNode)element).getPayload();
				if (element instanceof ModuleNode)
					return NEDTreeUtil.getNedModelLabelProvider().getText(((ModuleNode) element).node);
				else if (element instanceof ParamResolution)
					return getLabelFor((ParamResolution) element, inifileDocument);
				else 
					return element.toString();
			}
		});
		treeViewer.setContentProvider(new GenericTreeContentProvider());

		//XXX turn this into an Action and add to context menu as well
		treeViewer.getTree().addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetDefaultSelected(SelectionEvent e) {
				Object element = ((IStructuredSelection)treeViewer.getSelection()).getFirstElement();
				if (element instanceof GenericTreeNode && ((GenericTreeNode)element).getPayload() instanceof ModuleNode) {
					ModuleNode payload = (ModuleNode) ((GenericTreeNode)element).getPayload();
					if (payload.node != null) {
						NEDResourcesPlugin.openNEDElementInEditor(payload.node);
					}
				}				
			}
		});

		return treeViewer.getTree();
	}

	@Override
	protected void displayMessage(String text) {
		super.displayMessage(text);
		inifileDocument = null;
		treeViewer.setInput(null);
	}

	public void buildContent(String moduleFullPath, String moduleTypeName, InifileAnalyzer ana) {
		// build tree
		GenericTreeNode root = new GenericTreeNode("root");
		INEDTypeResolver nedResources = NEDResourcesPlugin.getNEDResources();
		String moduleFullName = moduleFullPath.replaceFirst("^.*\\.", "");
		buildTree(root, moduleFullName, moduleFullPath, moduleTypeName, null, nedResources, ana);

		// prevent collapsing all treeviewer nodes: only set it on viewer if it's different from old input
		if (!GenericTreeUtils.treeEquals(root, (GenericTreeNode)treeViewer.getInput())) { 
			treeViewer.setInput(root);
			this.inifileDocument = ana==null ? null : ana.getDocument();
		}
	}

	private void buildTree(GenericTreeNode parent, String moduleFullName, String moduleFullPath, String moduleTypeName, SubmoduleNode thisSubmodule, INEDTypeResolver nedResources, InifileAnalyzer ana) {
		//FIXME detect circles!!! currently it results in stack overflow
		// dig out type info (NED declaration)
		if (StringUtils.isEmpty(moduleTypeName)) {
			String text = moduleFullName+"  (module type unknown)";
			GenericTreeNode thisNode = new GenericTreeNode(new ErrorNode(text));  
			parent.addChild(thisNode);
			return;
		}
		INEDTypeInfo moduleType = nedResources.getComponent(moduleTypeName);
		if (moduleType == null) {
			String text = moduleFullName+"  ("+moduleTypeName+" - no such module type)";
			GenericTreeNode thisNode = new GenericTreeNode(new ErrorNode(text));  
			parent.addChild(thisNode);
			return;
		}

		// do useful work: add tree node corresponding to this module
		GenericTreeNode thisNode = addTreeNode(parent, moduleFullName, moduleFullPath, moduleType, thisSubmodule, ana);

		// traverse submodules
		for (NEDElement node : moduleType.getSubmods().values()) {
			SubmoduleNode submodule = (SubmoduleNode) node;
			String submoduleName = InifileUtils.getSubmoduleFullName(submodule);
			String submoduleType = InifileUtils.getSubmoduleType(submodule);

			// recursive call
			buildTree(thisNode, submoduleName, moduleFullPath+"."+submoduleName, submoduleType, submodule, nedResources, ana);
		}
	}

	/**
	 * Adds a node to the tree. The new node describes the module and its parameters.
	 */
	private static GenericTreeNode addTreeNode(GenericTreeNode parent, String moduleFullName, String moduleFullPath, INEDTypeInfo moduleType, SubmoduleNode thisSubmodule, InifileAnalyzer ana) {
		String moduleText = moduleFullName+"  ("+moduleType.getName()+")";
		GenericTreeNode thisNode = new GenericTreeNode(new ModuleNode(moduleText, thisSubmodule==null ? moduleType.getNEDElement() : thisSubmodule));
		parent.addChild(thisNode);

		if (ana == null) {
			// no inifile available, we only have NED info
			thisNode.addChild(new GenericTreeNode("sorry, cannot extract parameters from NED yet")); //XXX
		}
		else {
			ParamResolution[] list = ana.getParamResolutionsForModule(moduleFullPath, "Parameters"); //XXX
			for (ParamResolution res : list)
				thisNode.addChild(new GenericTreeNode(res));
		}
		return thisNode;
	}

	protected static String getLabelFor(ParamResolution res, IInifileDocument doc) {
		String[] tmp = getValueAndRemark(res, doc);
		String value = tmp[0];
		String remark = tmp[1];
		return res.paramNode.getName() + " = " + (value==null ? "" : value+" ") + "(" + remark + ")"; 
	}

}
