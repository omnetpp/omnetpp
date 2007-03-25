package org.omnetpp.inifile.editor.views;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.common.engine.PatternMatcher;
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.pojo.ParamNode;
import org.omnetpp.ned.model.pojo.SubmoduleNode;
import org.omnetpp.ned.resources.NEDResourcesPlugin;

public class ModuleTreeView extends ViewPart {
	private TreeViewer treeViewer;
	private ISelectionListener selectionChangedListener;
	private String oldNetworkName;
	
	public ModuleTreeView() {
	}

	@Override
	public void createPartControl(Composite parent) {
		treeViewer = new TreeViewer(parent, SWT.SINGLE);
		treeViewer.setLabelProvider(new LabelProvider()); //XXX for now
		treeViewer.setContentProvider(new GenericTreeContentProvider()); //XXX for now
		hookSelectionChangedListener();
	}

	@Override
	public void dispose() {
		unhookSelectionChangedListener();
		super.dispose();
	}
	
	private void hookSelectionChangedListener() {
		selectionChangedListener = new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				setViewerInput(selection);
			}
		};
		getSite().getPage().addPostSelectionListener(selectionChangedListener);
	}
	
	private void unhookSelectionChangedListener() {
		if (selectionChangedListener != null)
			getSite().getPage().removePostSelectionListener(selectionChangedListener);
	}
	
	@Override
	public void setFocus() {
		treeViewer.getTree().setFocus();
	}

	public void setViewerInput(ISelection selection) {
		GenericTreeNode root = new GenericTreeNode("root");
		IEditorPart editor = getSite().getWorkbenchWindow().getActivePage().getActiveEditor();
		if (editor instanceof InifileEditor) {
			InifileEditor inifileEditor = (InifileEditor) editor;
			IInifileDocument doc = inifileEditor.getEditorData().getInifileDocument();

			//XXX consider changing the return type of NEDResourcesPlugin.getNEDResources() to INEDTypeResolver
			INEDTypeResolver nedResources = NEDResourcesPlugin.getNEDResources();

			String networkName = doc.getValue("General", "network");
			if (networkName == null) {
				displayMessage("Network not specified (no [General]/network= setting)");
				return;
			}

			if (!networkName.equals(oldNetworkName)) {  //XXX or something changed in the NED tree
				dumpModule(networkName, networkName, networkName, root, nedResources, doc);
				treeViewer.setInput(root);
				oldNetworkName = networkName;
			}
		}
		else {
			displayMessage(editor==null ? "No editor is open." : "Editor is not an inifile editor.");
		}
	}

	private void displayMessage(String text) {
		GenericTreeNode root = new GenericTreeNode("root");
		root.addChild(new GenericTreeNode(text));
		treeViewer.setInput(root);
	}

	private void dumpModule(String moduleTypeName, String moduleFullPath, String moduleName, GenericTreeNode parent, INEDTypeResolver nedResources, IInifileDocument doc) {
		// dig out type info (NED declaration)
		if (isEmpty(moduleTypeName)) {
			String text = moduleName+"  (module type unknown)";
			GenericTreeNode thisNode = new GenericTreeNode(text);
			parent.addChild(thisNode);
			return;
		}
		INEDTypeInfo moduleType = nedResources.getComponent(moduleTypeName);
		if (moduleType == null) {
			String text = moduleName+"  ("+moduleTypeName+" - no such module type)";
			GenericTreeNode thisNode = new GenericTreeNode(text);
			parent.addChild(thisNode);
			return;
		}

		String text = moduleName+"  ("+moduleTypeName+")";
		GenericTreeNode thisNode = new GenericTreeNode(text);
		parent.addChild(thisNode);

		dumpParameters(moduleType, thisNode, moduleFullPath, doc);
		
		for (NEDElement node : moduleType.getSubmods().values()) {
			SubmoduleNode submodule = (SubmoduleNode) node;

			// produce submodule name; if vector, append [*]
			String submoduleName = submodule.getName();
			if (!isEmpty(submodule.getVectorSize())) //XXX what if parsed expressions are in use?
				submoduleName += "[*]";
			
			// produce submodule type: if "like", use like type
			//XXX should try to evaluate "like" expression and use result as type (if possible)
			String submoduleType = submodule.getType();
			if (isEmpty(submoduleType))
				submoduleType = submodule.getLikeType();
			
			// recursive call
			dumpModule(submoduleType, moduleFullPath+"."+submoduleName, submoduleName, thisNode, nedResources, doc);
		}
	}
	
	private static void dumpParameters(INEDTypeInfo moduleType, GenericTreeNode parent, String moduleFullPath, IInifileDocument doc) {
		for (NEDElement node : moduleType.getParamValues().values()) {
			ParamNode param = (ParamNode) node;
			
			// value in the NED file
			String nedValue = param.getValue(); //XXX what if parsed expressions?
			if (isEmpty(nedValue)) nedValue = null;
			boolean isDefault = param.getIsDefault();

			// look up its value in the ini file
			String paramFullPath = moduleFullPath + "." + param.getName();
			String iniKey = lookupParameter(paramFullPath, doc, "Parameters");
			String iniValue = doc.getValue("Parameters", iniKey);
			//XXX look up use-default as well!!!
			
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
			parent.addChild(new GenericTreeNode(text));
		}
	}

	/**
	 * Given a parameter's fullPath, returns the key of the matching
	 * inifile entry in the given section, or null if it's not 
	 * in the inifile. 
	 */
	private static String lookupParameter(String paramFullPath, IInifileDocument doc, String section) {
		String[] keys = doc.getKeys(section);
		for (String key : keys)
			if (new PatternMatcher(key, true, true, true).matches(paramFullPath))
				return key;
		return null;
	}

	private static boolean isEmpty(String string) {
		return string==null || "".equals(string);
	}
}
