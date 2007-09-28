package org.omnetpp.test.gui.nededitor;

import junit.framework.Assert;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.MultiPageEditorPartAccess;
import com.simulcraft.test.gui.access.TextEditorAccess;

public class NedPathTest
    extends NedFileTestCase
{
    private String directoryName = "ned-path-test";

    public void testChangeNedPathWhileEditorIsOpen() throws Exception {
        setNedPath(directoryName);
        Assert.assertFalse(Access.getWorkbenchWindowAccess().hasEditorPartWithTitle(fileName));
    }

    public void testChangeNedPathWhileEditorIsOpenAndModified() throws Exception {
        MultiPageEditorPartAccess multiPageEditorPart = findMultiPageEditor();
        TextEditorAccess textEditor = (TextEditorAccess)multiPageEditorPart.ensureActiveEditor("Text");
        textEditor.typeIn("Modified");
        setNedPath(directoryName);
        Assert.assertTrue(Access.getWorkbenchWindowAccess().findEditorPartByTitle(fileName).getEditorPart().isDirty());
    }

    @Override
    protected void setUpInternal() throws Exception {
        super.setUpInternal();
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
