package org.omnetpp.test.gui.core;

import org.omnetpp.test.gui.access.Access;
import org.omnetpp.test.gui.access.WorkbenchWindowAccess;
import org.omnetpp.test.gui.util.WorkbenchUtils;
import org.omnetpp.test.gui.util.WorkspaceUtils;


public class ProjectFileTestCase
	extends GUITestCase
{
	protected String projectName = "test-project";
	
	protected String fileName;

	protected String filePath;

	public ProjectFileTestCase(String fileName) {
		setFileName(fileName);
	}
	
	public void setFileName(String fileName) {
		this.fileName = fileName;
		this.filePath = projectName + "/" + fileName;
	}

	protected void createFileWithContent(String content) throws Exception {
		WorkspaceUtils.createFileWithContent(filePath, content);
	}
	
	protected void openFileFromProjectExplorerView() {
		WorkbenchUtils.findInProjectExplorerView(filePath).reveal().doubleClick();
	}

	protected void assertFileExists() {
		WorkspaceUtils.assertFileExists(filePath);
	}

	@Override
	/**
	 * These must be advised exactly once.
	 */
	protected void setUp() throws Exception {
		super.setUp();
		setUpInternal();
	}
	
	@Override
    /**
     * These must be advised exactly once.
     */
	protected void tearDown() throws Exception {
		super.tearDown();
		tearDownInternal();
	}

	/**
	 * This has to be a separate method so that the aspect is not applied to it each time it is overridden.
	 */
	protected void setUpInternal() throws Exception {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		workbenchWindow.closeAllEditorPartsWithHotKey();
		WorkspaceUtils.ensureProjectFileDeleted(projectName, fileName);
	}
	
    /**
     * This has to be a separate method so that the aspect is not applied to it each time it is overridden.
     */
	protected void tearDownInternal() throws Exception {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		if (workbenchWindow.hasEditorPartWithTitle(fileName))
		    workbenchWindow.findEditorPartByTitle(fileName).saveWithHotKey();
		workbenchWindow.closeAllEditorPartsWithHotKey();
		WorkspaceUtils.ensureProjectFileDeleted(projectName, fileName);
	}
}
