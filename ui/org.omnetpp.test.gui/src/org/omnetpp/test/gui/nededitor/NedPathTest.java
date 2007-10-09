package org.omnetpp.test.gui.nededitor;

import junit.framework.Assert;

import org.omnetpp.test.gui.access.NedEditorAccess;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.TextEditorAccess;
import com.simulcraft.test.gui.util.WorkspaceUtils;

public class NedPathTest
    extends NedFileTestCase
{
    private String directoryName = "test-ned-path-folder";

    // editor should close if the NED file is no longer under the NED-path
    public void testChangeNedPathWhileEditorIsOpen() throws Exception {
        setNedPath(directoryName);
        Assert.assertFalse(Access.getWorkbenchWindowAccess().hasEditorPartWithTitle(fileName));
    }

    // NED file should stay open but loaded in a text editor if it is no longer under NED-Path
    public void testChangeNedPathWhileEditorIsOpenAndModified() throws Exception {
        NedEditorAccess nedEditor = findNedEditor();
        TextEditorAccess textEditor = nedEditor.ensureActiveTextEditor();
        textEditor.typeIn("Modified");
        setNedPath(directoryName);
        Assert.assertTrue(Access.getWorkbenchWindowAccess().findEditorPartByTitle(fileName).getPart().isDirty());
    }

    @Override
    protected void setUpInternal() throws Exception {
        super.setUpInternal();
        WorkspaceUtils.ensureFolderExists(projectName, directoryName);
        setDefaultNedPath();
        createFileWithContent("simple Test {}");
        openFileFromProjectExplorerView();
    }

    @Override
    protected void tearDownInternal() throws Exception {
        super.tearDownInternal();
        setDefaultNedPath();
    }

    private void setDefaultNedPath() {
        NedEditorUtils.setDefaultNedPath(projectName);
    }

    private void setNedPath(String path) {
        NedEditorUtils.setNedPath(projectName, path);
    }
}
