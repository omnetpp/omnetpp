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
import org.omnetpp.inifile.editor.model.InifileAnalyzer.ParamResolution;
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
public class ModuleHierarchyView extends AbstractModuleView {
	private TreeViewer treeViewer;

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

		/* needed for labelProvider */
		@Override
		public String toString() { 
			return moduleFullPath;
		}

		/* needed for GenericTreeUtil.treeEquals() */
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
					return createLabelFor((ParamResolution) element, null); //XXX null??
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

	public void buildContent(String moduleFullPath, String moduleTypeName, InifileAnalyzer ana) {
		// build tree
		GenericTreeNode root = new GenericTreeNode("root");
		INEDTypeResolver nedResources = NEDResourcesPlugin.getNEDResources();
		String moduleFullName = moduleFullPath.replaceFirst("^.*\\.", "");
		buildTree(root, moduleFullName, moduleFullPath, moduleTypeName, null, nedResources, ana);

		// prevent collapsing all treeviewer nodes: only set it on viewer if it's different from old input
		if (!GenericTreeUtils.treeEquals(root, (GenericTreeNode)treeViewer.getInput())) 
			treeViewer.setInput(root);
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
	
	protected static String createLabelFor(ParamResolution res, IInifileDocument doc) {
			// value in the NED file
			String nedValue = res.paramNode.getValue(); //XXX what if parsed expressions?
			if (StringUtils.isEmpty(nedValue)) 
				nedValue = null;

			// look up its value in the ini file
			String iniValue = null;
			if (doc != null && res.key != null)
				iniValue = doc.getValue(res.section, res.key);

			String valueText;
			switch (res.type) {
				case UNASSIGNED: valueText = "(unassigned)"; break;
				case NED: valueText = nedValue+" (NED)"; break;  
				case NED_DEFAULT: valueText = nedValue+"(NED default applied)"; break;
				case INI: valueText = iniValue+" (ini)"; break;
				case INI_OVERRIDE: valueText = iniValue+" (ini, overrides NED default "+nedValue+")"; break;
				case INI_NEDDEFAULT: valueText = nedValue+" (ini, sets same value as NED default)"; break;
				default: throw new IllegalStateException("invalid param resolution type: "+res.type);
			}

			String text = res.paramNode.getName()+" = "+valueText;
			return text;
	}

}
