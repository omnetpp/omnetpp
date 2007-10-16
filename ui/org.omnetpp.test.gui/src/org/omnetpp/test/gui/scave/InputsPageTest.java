package org.omnetpp.test.gui.scave;

import java.util.regex.Pattern;

import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.test.gui.access.InputsPageAccess;
import org.omnetpp.test.gui.access.ScaveEditorAccess;

import com.simulcraft.test.gui.access.TreeAccess;
import com.simulcraft.test.gui.util.WorkbenchUtils;

public class InputsPageTest extends ScaveFileTestCase {
	
	protected ScaveEditorAccess editor;
	protected InputsPageAccess inputsPage;
	
	public void testInputFilesTree() {
		TreeAccess filesView = inputsPage.getInputFilesViewTree();
		filesView.assertContent(buildFilesViewContent());
	}
	
	public void testFileRunView() {
		inputsPage.ensureFileRunViewVisible();
		TreeAccess fileRunTree = inputsPage.getFileRunViewTree();
		fileRunTree.assertContent(buildFileRunViewContent());
	}
	
	public void testRunFileView() {
		inputsPage.ensureRunFileViewVisible();
		TreeAccess runFileTree = inputsPage.getRunFileViewTree();
		runFileTree.assertContent(buildRunFileViewContent());
	}
	
	public void testLogicalView() {
		inputsPage.ensureLogicalViewVisible();
		TreeAccess logicalTree = inputsPage.getLogicalViewTree();
		logicalTree.assertContent(buildLogicalViewContent());
	}
	
	public void testRefresh() throws Exception {
		removeFile("test-inputspage.sca");
		removeFile("test-inputspage.vec");
		
		WorkbenchUtils.refreshProjectFromProjectExplorerView(projectName);
		
		inputsPage.getInputFilesViewTree().assertContent(buildFilesViewContent());
		inputsPage.getFileRunViewTree().assertContent(
			forest(
				n("/project/test-empty.sca"),
				n("/project/test-empty.vec")));
		inputsPage.getRunFileViewTree().assertEmpty();
		inputsPage.getLogicalViewTree().assertEmpty();
	}

	private static GenericTreeNode[] buildFilesViewContent() {
		return 	forest(
					n("file /project/test-empty.sca"),
					n("file /project/test-empty.vec"),
					n("file test-inputspa*.vec"),
					n("file test-inputspa*.sca"));
	}
	
	private static GenericTreeNode[] buildFileRunViewContent() {
		return 	forest(
					n("/project/test-empty.sca"),
					n("/project/test-empty.vec"),
					n("/project/test-inputspage.sca",
						n("run \"run-1\""),
						n("run \"run-2\"")),
					n("/project/test-inputspage.vec",
						n("run \"run-1\""))
				);
	}
	
	private static GenericTreeNode[] buildRunFileViewContent() {
		return 	forest(
					n("run \"run-1\"",
						n("/project/test-inputspage.sca"),
						n("/project/test-inputspage.vec")),
					n("run \"run-2\"",
						n("/project/test-inputspage.sca"))
				);
	}
	
	private static GenericTreeNode[] buildLogicalViewContent() {
		return 	forest(
					n("experiment \"1\"",
						n("measurement \"1\"",
							n("replication \"1\"",
								n("run \"run-1\"",
									n("/project/test-inputspage.sca"),
									n("/project/test-inputspage.vec"))))),
					n("experiment \"2\"",
						n("measurement \"2\"",
							n("replication \"2\"",
								n("run \"run-2\"",
									n("/project/test-inputspage.sca")))))
				);
	}
	
	private static GenericTreeNode[] forest(GenericTreeNode... trees) {
		return trees;
	}
	
	private static GenericTreeNode n(String label, GenericTreeNode... children) {
		GenericTreeNode node = new GenericTreeNode(Pattern.quote(label));
		for (GenericTreeNode child : children)
			node.addChild(child);
		return node;
	}
	
	/**
	 * This has to be a separate method so that the aspect is not applied to it each time it is overridden.
	 */
	protected void setUpInternal() throws Exception {
		super.setUpInternal();
		createTestFiles();
		editor = ScaveEditorUtils.openAnalysisFile(projectName, fileName);
		inputsPage = editor.ensureInputsPageActive();
	}
	
    /**
     * This has to be a separate method so that the aspect is not applied to it each time it is overridden.
     */
	protected void tearDownInternal() throws Exception {
		editor = null;
		inputsPage = null;
	}
	
	protected void createTestFiles() throws Exception {
		createFile(
			fileName,
			
			"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" +
			"<scave:Analysis xmi:version=\"2.0\" xmlns:xmi=\"http://www.omg.org/XMI\" xmlns:scave=\"http://www.omnetpp.org/omnetpp/scave\">" +
			"<inputs>" +
			"<inputs name=\"/project/test-empty.sca\"/>" +
			"<inputs name=\"/project/test-empty.vec\"/>" +
			"<inputs name=\"test-inputspa*.vec\"/>" +
			"<inputs name=\"test-inputspa*.sca\"/>" +
			"</inputs>" +
			"<datasets/>" +
			"<chartSheets/>" +
			"</scave:Analysis>");
		createFile(
			"test-empty.sca",
			"");
		createFile(
			"test-empty.vec",
			"");
		createFile(
			"test-inputspage.sca",
			
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
			"test-inputspage.vec",
			
			"run run-1\n" +
			"attr experiment 1\n" +
			"attr measurement 1\n" +
			"attr replication 1\n" +
			"vector 1 module-2 vector-2 TV\n" +
			"1	0.0	1.0\n" +
			"1	1.0	2.0\n");
	}
}
