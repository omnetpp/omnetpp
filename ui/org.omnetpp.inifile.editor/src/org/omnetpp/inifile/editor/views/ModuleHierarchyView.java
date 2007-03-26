package org.omnetpp.inifile.editor.views;

import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.NEDTreeUtil;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.pojo.ParamNode;
import org.omnetpp.ned.model.pojo.SubmoduleNode;
import org.omnetpp.ned.resources.NEDResourcesPlugin;

/**
 * View.
 * 
 * @author Andras
 */
public class ModuleHierarchyView extends AbstractModuleView {
	private TreeViewer treeViewer;
	
	private static class Payload {
		String text;
		NEDElement node;  // SubmoduleNode, ParameterNode etc

		public Payload(String text, NEDElement node) {
			this.text = text;
			this.node = node;
		}

		public String toString() { 
			return text;
		}
	}
	
	public ModuleHierarchyView() {
	}

	@Override
	public void createPartControl(Composite parent) {
		GridLayout layout = new GridLayout();
		layout.marginWidth = layout.marginHeight = 0;
		parent.setLayout(layout);
		
		treeViewer = new TreeViewer(parent, SWT.SINGLE);
		treeViewer.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		treeViewer.setLabelProvider(new LabelProvider() {
			@Override
			public Image getImage(Object element) {
				Object payload = ((GenericTreeNode)element).getPayload();
				if (payload instanceof Payload) {
					Object nedElement = ((Payload)payload).node;
					return NEDTreeUtil.getNedModelLabelProvider().getImage(nedElement);
				}
				else {
					return null;
				}
			}
		});
		treeViewer.setContentProvider(new GenericTreeContentProvider());
		hookSelectionChangedListener();
	}

	protected Control getPartControl() {
		return treeViewer.getTree();
	}

	public void buildContent(String moduleFullPath, String moduleTypeName, IInifileDocument doc) {
		GenericTreeNode root = new GenericTreeNode("root");
		INEDTypeResolver nedResources = NEDResourcesPlugin.getNEDResources();
		String moduleFullName = moduleFullPath.replaceFirst("^.*\\.", "");
		buildTree(root, moduleFullName, moduleFullPath, moduleTypeName, nedResources, doc);
		treeViewer.setInput(root);
	}

	private void buildTree(GenericTreeNode parent, String moduleFullName, String moduleFullPath, String moduleTypeName, INEDTypeResolver nedResources, IInifileDocument doc) {
		// dig out type info (NED declaration)
		if (StringUtils.isEmpty(moduleTypeName)) {
			String text = moduleFullName+"  (module type unknown)";
			GenericTreeNode thisNode = new GenericTreeNode(text);  
			parent.addChild(thisNode); //XXX display with "error" icon
			return;
		}
		INEDTypeInfo moduleType = nedResources.getComponent(moduleTypeName);
		if (moduleType == null) {
			String text = moduleFullName+"  ("+moduleTypeName+" - no such module type)";
			GenericTreeNode thisNode = new GenericTreeNode(text);
			parent.addChild(thisNode); //XXX display with "error" icon
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
		GenericTreeNode thisNode = new GenericTreeNode(new Payload(moduleText, moduleType.getNEDElement()));
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
			String valueText;
			if (nedValue==null && iniValue==null)
				valueText = "(unassigned)";
			else if (nedValue!=null && iniValue==null)
				valueText = nedValue+" (NED)";  //XXX default(x) or not??
			else if (nedValue==null && iniValue!=null)
				valueText = iniValue+" (ini)";
			else if (nedValue.equals(iniValue))
				valueText = nedValue+" (NED, ini)";
			else
				valueText = iniValue+" (ini, overrides NED value "+nedValue+")";
				
			String text = param.getName()+" = "+valueText;
			thisNode.addChild(new GenericTreeNode(new Payload(text, param)));
		}
		return thisNode;
	}

}
