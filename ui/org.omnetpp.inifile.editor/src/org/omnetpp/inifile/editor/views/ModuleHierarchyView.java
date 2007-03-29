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
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.InifileUtils.ParameterData;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.NEDTreeUtil;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.pojo.ParamNode;
import org.omnetpp.ned.model.pojo.SubmoduleNode;
import org.omnetpp.ned.resources.NEDResourcesPlugin;
import static org.omnetpp.inifile.editor.model.InifileUtils.Type;

/**
 * Displays NED module hierarchy with module parameters, and 
 * optionally, values assigned in ini files.
 * 
 * @author Andras
 */
public class ModuleHierarchyView extends AbstractModuleView {
	private TreeViewer treeViewer;

	/**
	 * Node contents for the GenericTreeNode tree that is displayed in the view
	 */
	private static class Payload {
		Type type;
		String text;
		NEDElement node;  // SubmoduleNode, ParameterNode etc

		/* for convenience */
		public Payload(Type type, String text, NEDElement node) {
			this.type = type;
			this.text = text;
			this.node = node;
		}

		/* needed for labelProvider */
		public String toString() { 
			return text;
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
			final Payload other = (Payload) obj;
			if (type != other.type)
				return false;
			if (text == null) {
				if (other.text != null)
					return false;
			} else if (!text.equals(other.text))
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
				if (element instanceof GenericTreeNode && ((GenericTreeNode)element).getPayload() instanceof Payload) {
					Payload payload = (Payload) ((GenericTreeNode)element).getPayload();
					return suggestImage(payload.type, payload.node);
				}
				else {
					return null;
				}
			}
		});
		treeViewer.setContentProvider(new GenericTreeContentProvider());

		//XXX turn this into an Action and add to context menu as well
		treeViewer.getTree().addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetDefaultSelected(SelectionEvent e) {
				Object element = ((IStructuredSelection)treeViewer.getSelection()).getFirstElement();
				if (element instanceof GenericTreeNode && ((GenericTreeNode)element).getPayload() instanceof Payload) {
					Payload payload = (Payload) ((GenericTreeNode)element).getPayload();
					if (payload.node != null) {
						NEDResourcesPlugin.openNEDElementInEditor(payload.node);
					}
				}				
			}
		});
		
		return treeViewer.getTree();
	}

	public void buildContent(String moduleFullPath, String moduleTypeName, IInifileDocument doc) {
		// build tree
		GenericTreeNode root = new GenericTreeNode("root");
		INEDTypeResolver nedResources = NEDResourcesPlugin.getNEDResources();
		String moduleFullName = moduleFullPath.replaceFirst("^.*\\.", "");
		buildTree(root, moduleFullName, moduleFullPath, moduleTypeName, nedResources, doc);

		// prevent collapsing all treeviewer nodes: only set it on viewer if it's different from old input
		if (!GenericTreeUtils.treeEquals(root, (GenericTreeNode)treeViewer.getInput())) 
			treeViewer.setInput(root);
	}

	private void buildTree(GenericTreeNode parent, String moduleFullName, String moduleFullPath, String moduleTypeName, INEDTypeResolver nedResources, IInifileDocument doc) {
		//FIXME detect circles!!! currently it results in stack overflow
		// dig out type info (NED declaration)
		if (StringUtils.isEmpty(moduleTypeName)) {
			String text = moduleFullName+"  (module type unknown)";
			GenericTreeNode thisNode = new GenericTreeNode(new Payload(Type.ERROR, text, null));  
			parent.addChild(thisNode);
			return;
		}
		INEDTypeInfo moduleType = nedResources.getComponent(moduleTypeName);
		if (moduleType == null) {
			String text = moduleFullName+"  ("+moduleTypeName+" - no such module type)";
			GenericTreeNode thisNode = new GenericTreeNode(new Payload(Type.ERROR, text, null));  
			parent.addChild(thisNode);
			return;
		}

		// do useful work: add tree node corresponding to this module
		GenericTreeNode thisNode = addTreeNode(parent, moduleFullName, moduleFullPath, moduleType, doc);

		// traverse submodules
		for (NEDElement node : moduleType.getSubmods().values()) {
			SubmoduleNode submodule = (SubmoduleNode) node;
			String submoduleName = InifileUtils.getSubmoduleFullName(submodule);
			String submoduleType = InifileUtils.getSubmoduleType(submodule);

			// recursive call
			buildTree(thisNode, submoduleName, moduleFullPath+"."+submoduleName, submoduleType, nedResources, doc);
		}
	}

	/**
	 * Adds a node to the tree. The new node described the module and its parameters.
	 */
	private static GenericTreeNode addTreeNode(GenericTreeNode parent, String moduleFullName, String moduleFullPath, INEDTypeInfo moduleType, IInifileDocument doc) {
		String moduleText = moduleFullName+"  ("+moduleType.getName()+")";
		GenericTreeNode thisNode = new GenericTreeNode(new Payload(Type.OTHER, moduleText, moduleType.getNEDElement()));
		parent.addChild(thisNode);

		for (NEDElement node : moduleType.getParamValues().values()) {
			ParamNode param = (ParamNode) node;

			// value in the NED file
			String nedValue = param.getValue(); //XXX what if parsed expressions?
			if (StringUtils.isEmpty(nedValue)) nedValue = null;
			boolean isDefault = param.getIsDefault(); //XXX should be used somehow

			// look up its value in the ini file
			String iniValue = null;
			if (doc != null) {
				String paramFullPath = moduleFullPath + "." + param.getName();
				String iniKey = InifileUtils.lookupParameter(paramFullPath, doc, "Parameters");
				iniValue = doc.getValue("Parameters", iniKey);
				//XXX observe "**.use-default=true" style settings as well!!!
			}

			//XXX this issue is much more complicated, as there may be multiple possibly matching 
			// inifile entries. For example, we have "net.node[*].power", and inifile contains
			// "*.node[0..4].power=...", "*.node[5..9].power=...", and "net.node[10..].power=...".
			// Current code would not match any (!!!), only "net.node[*].power=..." if it existed.
			// lookupParameter() should actually return multiple matches. 
			Type type;
			String valueText;
			if (nedValue==null && iniValue==null) {
				type = Type.UNASSIGNED_PAR;
				valueText = "(unassigned)";
			}
			else if (nedValue!=null && iniValue==null) {
				type = Type.NED_PAR;
				valueText = nedValue+" (NED)";  //XXX default(x) or not??
			}
			else if (nedValue==null && iniValue!=null) { 
				type = Type.INI_PAR;
				valueText = iniValue+" (ini)";
			}
			else if (nedValue.equals(iniValue)) {
				type = Type.INI_PAR_REDUNDANT;
				valueText = nedValue+" (same value in both NED and ini)";
			}
			else {
				type = Type.INI_PAR;
				valueText = iniValue+" (ini, overrides NED value "+nedValue+")";
			}

			String text = param.getName()+" = "+valueText;
			thisNode.addChild(new GenericTreeNode(new Payload(type, text, param)));
		}
		return thisNode;
	}

}
