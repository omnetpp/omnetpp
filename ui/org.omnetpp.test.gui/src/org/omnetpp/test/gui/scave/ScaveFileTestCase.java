package org.omnetpp.test.gui.scave;

import java.util.regex.Pattern;

import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.test.gui.access.ScaveEditorAccess;
import org.omnetpp.test.gui.core.ProjectFileTestCase;

import com.simulcraft.test.gui.util.WorkspaceUtils;

public class ScaveFileTestCase
    extends ProjectFileTestCase
{
	protected ScaveEditorAccess editor;
	
    public ScaveFileTestCase() {
        super("test.anf");
    }
    
	protected void createFile(String fileName, String content) throws Exception {
		WorkspaceUtils.createFileWithContent(projectName + "/" + fileName, content);
	}

	protected void removeFile(String fileName) throws Exception {
		String filePath = projectName + "/" + fileName;
		WorkspaceUtils.assertFileExists(filePath);
		WorkspaceUtils.ensureFileNotExists(filePath);
	}
	
	protected static GenericTreeNode[] forest(GenericTreeNode... trees) {
		return trees;
	}
	
	protected static GenericTreeNode n(String label, GenericTreeNode... children) {
		GenericTreeNode node = new GenericTreeNode(Pattern.quote(label));
		for (GenericTreeNode child : children)
			node.addChild(child);
		return node;
	}
}
