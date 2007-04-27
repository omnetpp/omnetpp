package org.omnetpp.inifile.editor.views;

import java.util.Stack;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.ui.GenericTreeUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.IModuleTreeVisitor;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.NEDTreeIterator;
import org.omnetpp.inifile.editor.model.ParamResolution;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.NEDTreeUtil;
import org.omnetpp.ned.model.ex.SubmoduleNodeEx;
import org.omnetpp.ned.model.interfaces.IModuleTypeNode;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.pojo.CompoundModuleNode;
import org.omnetpp.ned.model.pojo.SimpleModuleNode;
import org.omnetpp.ned.model.pojo.SubmoduleNode;

/**
 * Displays NED module hierarchy with module parameters, and
 * optionally, values assigned in ini files.
 *
 * @author Andras
 */
//XXX create another view: Hierarchy (inheritance tree); and call this Usage? Nesting? Tree? Containment?
//XXX follow selection
//XXX context menu with "Go to NED file" and "Go to ini file"
//XXX "like" submodule with unresolved type does not appear as such!!!
public class ModuleHierarchyView extends AbstractModuleView {
	private TreeViewer treeViewer;
	private IInifileDocument inifileDocument; // corresponds to the current selection; unfortunately needed by the label provider

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
		            DisplayString dps = mn.submoduleNode.getEffectiveDisplayString(mn.submoduleType);
		            Image image = ImageFactory.getIconImage(dps.getAsStringDef(IDisplayString.Prop.IMAGE));
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

		// on double click, go to NED file or ini file
		treeViewer.getTree().addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetDefaultSelected(SelectionEvent e) {
				Object element = ((IStructuredSelection)treeViewer.getSelection()).getFirstElement();
				if (element instanceof GenericTreeNode)
					element = ((GenericTreeNode)element).getPayload();
				if (element instanceof ModuleNode) {
					ModuleNode payload = (ModuleNode) element;
					if (payload.submoduleNode != null)
						NEDResourcesPlugin.openNEDElementInEditor(payload.submoduleNode);
					else if (payload.submoduleType != null)
						NEDResourcesPlugin.openNEDElementInEditor((INEDElement)payload.submoduleType);
				}
				if (element instanceof ParamResolution) {
					ParamResolution payload = (ParamResolution) element;
					if (payload.paramValueNode != null)
						NEDResourcesPlugin.openNEDElementInEditor(payload.paramValueNode);
					else if (payload.paramDeclNode != null)
						NEDResourcesPlugin.openNEDElementInEditor(payload.paramDeclNode);
				}
			}
		});

		return treeViewer.getTree();
	}

	@Override
	protected void showMessage(String text) {
		super.showMessage(text);
		inifileDocument = null;
		treeViewer.setInput(null);
	}

	public void buildContent(INEDElement module, final InifileAnalyzer analyzer, final String section, String key) {
		final IInifileDocument doc = analyzer==null ? null : analyzer.getDocument();
		final String[] sectionChain = doc==null ? null : InifileUtils.resolveSectionChain(doc, section);

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
				return InifileUtils.resolveLikeParam(moduleFullPath, submodule, section, analyzer, doc);
    		}
    	}

    	INEDTypeResolver nedResources = NEDResourcesPlugin.getNEDResources();
    	NEDTreeIterator iterator = new NEDTreeIterator(nedResources, new TreeBuilder());
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
			this.inifileDocument = analyzer==null ? null : analyzer.getDocument();
			treeViewer.setInput(root);
			treeViewer.expandToLevel(2); //XXX collapses when switching to another editor then back. Remember selected element for each editor? use WeakHashMap?
		}
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
			thisNode.addChild(new GenericTreeNode("sorry, cannot extract parameters from NED yet")); //XXX
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
