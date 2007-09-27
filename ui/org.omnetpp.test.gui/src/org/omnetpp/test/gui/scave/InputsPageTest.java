package org.omnetpp.test.gui.scave;

import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.test.gui.access.InputsPageAccess;
import org.omnetpp.test.gui.access.ScaveEditorAccess;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.TreeAccess;
import com.simulcraft.test.gui.access.WorkbenchWindowAccess;
import com.simulcraft.test.gui.core.GUITestCase;
import com.simulcraft.test.gui.util.WorkspaceUtils;

public class InputsPageTest extends GUITestCase {
	
	protected String projectName = "test-project";
	
	protected ScaveEditorAccess editor;
	protected InputsPageAccess inputsPage;
	
	public void testInputFilesTree() {
		TreeAccess filesView = inputsPage.getInputFilesViewTree();
		filesView.assertContent(buildFilesViewContent());
	}
	
//	public void testEmptyViews() {
//		inputsPage.getFileRunViewTree().assertEmpty();
//		inputsPage.getRunFileViewTree().assertEmpty();
//		inputsPage.getLogicalViewTree().assertEmpty();
//	}
	
	public void testFileRunView() {
		TreeAccess fileRunTree = inputsPage.getFileRunViewTree();
		fileRunTree.assertContent(buildFileRunViewContent());
	}
	
	private static GenericTreeNode[] buildFilesViewContent() {
		return 	forest(
					n("file /test-project/empty.sca"),
					n("file /test-project/empty.vec"),
					n("file inputspage*.vec"),
					n("file inputspage*.sca"));
	}
	
	private static GenericTreeNode[] buildFileRunViewContent() {
		return 	forest(
					n("test-1.vec",
						n("run-1"),
						n("run-2")),
					n("test-2.vec",
						n("run-1"),
						n("run-2"))
				);
	}
	
	private static GenericTreeNode[] forest(GenericTreeNode... trees) {
		return trees;
	}
	
	private static GenericTreeNode n(String text, GenericTreeNode... children) {
		GenericTreeNode node = new GenericTreeNode(text);
		for (GenericTreeNode child : children)
			node.addChild(child);
		return node;
	}
	
	@Override
	/**
	 * These must be advised exactly once.
	 */
	protected void setUp() throws Exception {
		setUpInternal();
	}
	
	@Override
    /**
     * These must be advised exactly once.
     */
	protected void tearDown() throws Exception {
		tearDownInternal();
	}

	/**
	 * This has to be a separate method so that the aspect is not applied to it each time it is overridden.
	 */
	protected void setUpInternal() throws Exception {
		createTestFiles();
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		workbenchWindow.closeAllEditorPartsWithHotKey();
		//workbenchWindow.assertNoOpenEditorParts();
		editor = ScaveEditorUtils.openAnalysisFile(projectName, "inputspagetest.scave");
		inputsPage = editor.ensureInputsPageActive();
	}
	
    /**
     * This has to be a separate method so that the aspect is not applied to it each time it is overridden.
     */
	protected void tearDownInternal() throws Exception {
		editor = null;
		inputsPage = null;
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		workbenchWindow.closeAllEditorPartsWithHotKey();
        //workbenchWindow.assertNoOpenEditorParts();
		removeTestFiles();
	}
	
	protected void createTestFiles() throws Exception {
		createFile(
			"inputspagetest.scave",
			
			"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" +
			"<scave:Analysis xmi:version=\"2.0\" xmlns:xmi=\"http://www.omg.org/XMI\" xmlns:scave=\"http://www.omnetpp.org/omnetpp/scave\">" +
			"<inputs>" +
			"<inputs name=\"/test-project/empty.sca\"/>" +
			"<inputs name=\"/test-project/empty.vec\"/>" +
			"<inputs name=\"inputspage*.vec\"/>" +
			"<inputs name=\"inputspage*.sca\"/>" +
			"</inputs>" +
			"<datasets/>" +
			"<chartSheets/>" +
			"</scave:Analysis>");
		createFile(
			"empty.sca",
			"");
		createFile(
			"empty.vec",
			"");
		createFile(
			"inputspagetest.sca",
			
			"run run-1\n" +
			"attr experiment 1\n" +
			"attr measurement 1\n" +
			"attr replication 1\n" +
			"scalar module-1 scalar-1 1\n" +
			"run run-2\n" +
			"attr experiment 1\n" +
			"attr measurement 1\n" +
			"attr replication 2\n" +
			"scalar module-1 scalar-1 2\n");
		createFile(
			"inputspagetest.vec",
			
			"run run-1\n" +
			"attr experiment 1\n" +
			"attr measurement 1\n" +
			"attr replication 1\n" +
			"vector 1 module-2 vector-2 TV\n" +
			"1	0.0	1.0\n" +
			"1	1.0	2.0\n");
	}
	
	protected void createFile(String fileName, String content) throws Exception {
		WorkspaceUtils.createFileWithContent(projectName + "/" + fileName, content);
	}
	
	protected void removeTestFiles() throws Exception {
		removeFile("inputspagetest.scave");
		removeFile("empty.sca");
		removeFile("empty.vec");
		removeFile("inputspagetest.sca");
		removeFile("inputspagetest.vec");
	}
	
	protected void removeFile(String fileName) throws Exception {
		WorkspaceUtils.ensureProjectFileDeleted(projectName, fileName);
	}
}
