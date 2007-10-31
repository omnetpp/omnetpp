package org.omnetpp.test.gui.core;


import java.util.ArrayList;
import java.util.List;

import junit.framework.Assert;

import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.EditorPartAccess;
import com.simulcraft.test.gui.access.ToolItemAccess;
import com.simulcraft.test.gui.access.WorkbenchWindowAccess;
import com.simulcraft.test.gui.core.GUITestCase;
import com.simulcraft.test.gui.util.WorkbenchUtils;
import com.simulcraft.test.gui.util.WorkspaceUtils;


public class ProjectFileTestCase
	extends GUITestCase
{
	protected String projectName = "project";
	protected String fileName;
	protected String filePath;

	public ProjectFileTestCase(String fileName) {
		setFileName(fileName);
	}
	
	public void setFileName(String fileName) {
	    // I think the following assert does not always hold true, because there might be files 
	    // checked in without starting with 'test' so that they don't get deleted in cleanup
	    // FIXME: remove this commented assert eventually
	    // Assert.assertTrue("Filename must start with 'test'", fileName.startsWith("test"));
		this.fileName = fileName;
		this.filePath = projectName + "/" + fileName;
	}

	protected void createFileWithContent(String content) throws Exception {
		WorkspaceUtils.createFileWithContent(filePath, content);
	}
	
	protected void createEmptyFile() throws Exception {
	    createFileWithContent("");
	}
	
	protected void openFileFromProjectExplorerView() {
		WorkbenchUtils.findInProjectExplorerView(filePath).reveal().doubleClick();
	}

    protected EditorPartAccess findEditorPart() {
        return Access.getWorkbenchWindow().findEditorPartByTitle(fileName);
    }

    protected ToolItemAccess findToolItemWithToolTip(String text) {
        return findEditorPart().getToolBarManager().getToolBar().findToolItemWithTooltip(text);
    }

    protected void assertFileExists() {
		WorkspaceUtils.assertFileExists(filePath);
	}

    /**
     * Deletes all test generated files, folders and projects. (those starting with "test*") 
     */
    protected void cleanupWorkspace() {
        try {
            final List<IResource> toBeDeleted = new ArrayList<IResource>();

            ResourcesPlugin.getWorkspace().getRoot().accept(new IResourceVisitor() {
                public boolean visit(IResource resource) throws CoreException {
                    if (resource.getName().startsWith("test")) {
                        toBeDeleted.add(resource);
                        return false;
                    }
                    return true;
                }
            });

            for (IResource res : toBeDeleted)
                res.delete(true, null);
        }
        catch (CoreException e) {
            throw new RuntimeException(e);
        }
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
        ensureAllEditorPartsAreSaved();
        ensureAllEditorPartsAreClosed();
		cleanupWorkspace();
		assertProjectCorrect();
	}
    
    /**
     * This has to be a separate method so that the aspect is not applied to it each time it is overridden.
     */
    protected void tearDownInternal() throws Exception {
        ensureEditorPartIsSaved();
        ensureAllEditorPartsAreClosed();
    }

    private void assertProjectCorrect() throws Exception {
        WorkspaceUtils.assertProjectExists(projectName);
		String projectFileName = ResourcesPlugin.getWorkspace().getRoot().getProject(projectName).getLocation().toOSString()+"/.project";
		String projectFileContent = FileUtils.readTextFile(projectFileName);
        Assert.assertTrue("Project file content differs from expected", 
                          StringUtils.areEqualIgnoringWhiteSpace(projectFileContent,
                "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n" + 
        		"<projectDescription>\r\n" + 
        		"    <name>project</name>\r\n" + 
        		"    <comment></comment>\r\n" + 
        		"    <projects>\r\n" + 
        		"    </projects>\r\n" + 
        		"    <buildSpec>\r\n" + 
        		"        <buildCommand>\r\n" + 
        		"            <name>org.omnetpp.scave.builder.vectorfileindexer</name>\r\n" + 
        		"            <arguments>\r\n" + 
        		"            </arguments>\r\n" + 
        		"        </buildCommand>\r\n" + 
        		"    </buildSpec>\r\n" + 
        		"    <natures>\r\n" + 
        		"        <nature>org.omnetpp.main.omnetppnature</nature>\r\n" + 
        		"    </natures>\r\n" + 
        		"</projectDescription>\r\n"
        		));
		WorkspaceUtils.ensureFileNotExists(projectName+"/.nedfolders");
    }

    protected void ensureAllEditorPartsAreSaved() {
        WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindow();
        workbenchWindow.ensureAllEditorPartsAreSaved();
        workbenchWindow.assertNoDirtyEditorParts();
    }

    protected void ensureAllEditorPartsAreClosed() {
        WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindow();
        workbenchWindow.ensureAllEditorPartsAreClosed();
        workbenchWindow.assertNoOpenEditorParts();
    }
    
    protected void ensureEditorPartIsSaved() {
        WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindow();
        if (workbenchWindow.hasEditorPartWithTitle(fileName)) {
            EditorPartAccess editorPart = findEditorPart();
            editorPart.saveWithHotKey();
            editorPart.assertNotDirty();
        }
    }
}
