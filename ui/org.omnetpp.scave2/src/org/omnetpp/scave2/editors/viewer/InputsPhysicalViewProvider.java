package org.omnetpp.scave2.editors.providers;

import java.util.List;

import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.FileList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave2.editors.ScaveEditor;

public class InputsPhysicalViewProvider extends InputsTreeViewProvider {
	
	public InputsPhysicalViewProvider(ScaveEditor editor) {
		super(editor);
	}
	
	public ITreeContentProvider getContentProvider() {
		return new ContentProvider() {
			// Inputs/File/Run
			protected void buildTree(Inputs inputs) {
				ScaveEditor editor = getEditor();
				ResultFileManager manager = editor.getResultFileManager();
				root = new TreeNode(null, inputs);
				for (File file : editor.getInputFiles()) {
					TreeNode fileNode = new TreeNode(root, file);
					root.addChild(fileNode);
					RunList runlist = manager.getRunsInFile(file);
					for (int i = 0; i < runlist.size(); ++i) {
						Run run = runlist.get(i);
						TreeNode runNode = new TreeNode(fileNode, run);
						fileNode.addChild(runNode);
					}
				}
			}
		};
	}
	
	public ILabelProvider getLabelProvider() {
		return new LabelProvider() {

			public String getText(Object element) {
				if (element instanceof TreeNode) {
					TreeNode node = (TreeNode)element;
					if (node.payload instanceof Inputs)
						return "";
					else if (node.payload instanceof File) {
						File file = (File)node.payload;
						return file.getFilePath();
					}
					else if (node.payload instanceof Run) {
						Run run = (Run)node.payload;
						return run.getRunName() + ", " + run.getDate();
					}
				}
				return null;
			}
		};
	}
}
