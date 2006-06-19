package org.omnetpp.scave2.editors.providers;

import org.eclipse.core.resources.IFile;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * This class configures the viewer of the physical input tree.
 *  
 * @author Tomi
 */
public class InputsPhysicalViewProvider extends InputsTreeViewProvider {
	
	public InputsPhysicalViewProvider(ScaveEditor editor) {
		super(editor);
	}
	
	public ITreeContentProvider getContentProvider() {
		return new ContentProvider() {
			// Inputs/File/Run
			protected GenericTreeNode buildTree(Inputs inputs) {
				ResultFileManager manager = editor.getResultFileManager();
				GenericTreeNode root = new GenericTreeNode(null, inputs);
				for (File file : editor.getInputFiles()) {
					GenericTreeNode fileNode = new GenericTreeNode(root, file);
					root.addChild(fileNode);
					RunList runlist = manager.getRunsInFile(file);
					for (int i = 0; i < runlist.size(); ++i) {
						Run run = runlist.get(i);
						GenericTreeNode runNode = new GenericTreeNode(fileNode, run);
						fileNode.addChild(runNode);
					}
				}
				return root;
			}
		};
	}
	
	public ILabelProvider getLabelProvider() {
		return new LabelProvider() {

			public String getText(Object element) {
				if (element instanceof GenericTreeNode) {
					GenericTreeNode node = (GenericTreeNode)element;
					if (node.getPayload() instanceof Inputs)
						return "";
					else if (node.getPayload() instanceof File) {
						File file = (File)node.getPayload();
						IFile ifile = ScaveEditor.findFileInWorkspace(file.getFilePath());
						return ifile != null ? ifile.getFullPath().toString() : file.getFilePath();
					}
					else if (node.getPayload() instanceof Run) {
						Run run = (Run)node.getPayload();
						return run.getRunName() + ", " + run.getDate();
					}
				}
				return null;
			}
		};
	}
}
