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
	
	protected static String createScalarFileContent(int runNumber) {
		return String.format(
				"run run-%1$d\n" +
				"attr run-number %1$d\n" +
				"attr config config-%1$d\n" +
				"attr experiment %1$d\n" +
				"attr measurement %1$d\n" +
				"attr replication %1$d\n" +
				"scalar module-%1$d scalar-%1$d %1$d\n",
				runNumber);
	}
	
	protected static String createVectorFileContent(int runNumber) {
		return String.format(
				"run run-%1$d\n" +
				"attr run-number %1$d\n" +
				"attr config config-%1$d\n" +
				"attr experiment %1$d\n" +
				"attr measurement %1$d\n" +
				"attr replication %1$d\n" +
				"vector 1 module-%1$d vector-%1$d TV\n" +
				"1	0.0	%2$f\n",
				runNumber, (double)runNumber);
	}
}
