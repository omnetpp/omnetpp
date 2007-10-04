package org.omnetpp.test.gui.nededitor.main;

import junit.framework.Assert;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.access.GraphicalNedEditorAccess;
import org.omnetpp.test.gui.access.NedEditorAccess;
import org.omnetpp.test.gui.nededitor.NedFileTestCase;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.TextEditorAccess;


public class PageChangeTest
    extends NedFileTestCase
{
    public void testSimplePageChangeFromTextToGraphical() throws Exception {
        NedEditorAccess nedEditor = prepareTest();
        nedEditor.ensureActiveTextEditor();
        nedEditor.ensureActiveGraphicalEditor();
    }

    public void testSimplePageChangeFromGraphicalToText() throws Exception {
        NedEditorAccess nedEditor = prepareTest();
        nedEditor.ensureActiveGraphicalEditor();
        nedEditor.ensureActiveTextEditor();
    }

    public void testFollowChangesFromGraphicalToText() throws Exception {
        NedEditorAccess nedEditor = prepareTest();
        nedEditor.ensureActiveTextEditor();
        GraphicalNedEditorAccess graphicalNedEditor = nedEditor.ensureActiveGraphicalEditor();
        graphicalNedEditor.createSimpleModuleWithPalette("NewSimpleModule");
        TextEditorAccess textEditorAccess = nedEditor.ensureActiveTextEditor();
        Assert.assertTrue(textEditorAccess.findStyledText().getTextContent().matches("(?s).*NewSimpleModule.*"));
    }

    public void testFollowChangesFromTextToGraphical() throws Exception {
        NedEditorAccess nedEditor = prepareTest();
        nedEditor.ensureActiveGraphicalEditor();
        TextEditorAccess textEditor = nedEditor.ensureActiveTextEditor();
        textEditor.typeIn("simple NewSimpleModule {}\n");
        GraphicalNedEditorAccess graphicalNedEditor = nedEditor.ensureActiveGraphicalEditor();
        graphicalNedEditor.findSimpleModule("NewSimpleModule");
    }

    /**
     * Tests whether submodule name changes in the text editor are reflected in the graphical editor.
     */
    public void testFollowSubmoduleNameChangesFromTextToGraphical() throws Exception {
        createFileWithContent("simple A {}\nmodule TestCompound { submodules: name : A; }");
        openFileFromProjectExplorerView();
        NedEditorAccess nedEditor = findNedEditor();
        TextEditorAccess textEditor = nedEditor.ensureActiveTextEditor();
        textEditor.moveCursorAfter("name");
        textEditor.typeIn("x");
        GraphicalNedEditorAccess graphicalNedEditor = nedEditor.ensureActiveGraphicalEditor();
        graphicalNedEditor.findCompoundModule("TestCompound").findSubmoduleFigureByName("namex");
        nedEditor.ensureActiveTextEditor();
        textEditor.moveCursorAfter("namex");
        textEditor.pressKey(SWT.BS);
        nedEditor.ensureActiveGraphicalEditor();
        graphicalNedEditor.findCompoundModule("TestCompound").findSubmoduleFigureByName("name");
    }
    
    public void testUneditedFileIsNotDirty() throws Exception {
        // NED text below is intentionally unformatted to verify that no unnecessary reindenting/reformatting occurs
        String fileContent = "simple A {}\nmodule TestCompound { submodules: name : A; }";
        createFileWithContent(fileContent);
        openFileFromProjectExplorerView();
        NedEditorAccess nedEditor = findNedEditor();
        TextEditorAccess textEditor = nedEditor.ensureActiveTextEditor();
        Access.sleep(1);
        nedEditor.ensureActiveGraphicalEditor();
        Access.sleep(1);
        nedEditor.ensureActiveTextEditor();
        Access.sleep(1);
        textEditor.assertContent(fileContent);
        nedEditor.assertNotDirty();
    }
    
    public void testTextEditingDoesNotReformat() throws Exception {
        // NED text below is intentionally unformatted to verify that no unnecessary reindenting/reformatting occurs
        String fileContent = "simple A {}\nmodule TestCompound { submodules: name : A; }";
        createFileWithContent(fileContent);
        openFileFromProjectExplorerView();
        NedEditorAccess nedEditor = findNedEditor();
        TextEditorAccess textEditor = nedEditor.ensureActiveTextEditor();
        Access.sleep(1);
        textEditor.typeIn(" ");
        Access.sleep(1);
        nedEditor.ensureActiveGraphicalEditor();
        Access.sleep(1);
        nedEditor.ensureActiveTextEditor();
        Access.sleep(1);
        textEditor.assertContent(" "+fileContent);
        nedEditor.assertDirty();
    }

    private NedEditorAccess prepareTest() throws Exception {
        createFileWithContent("simple TestSimpleModule {}");
        openFileFromProjectExplorerView();
        NedEditorAccess nedEditor = findNedEditor();
        return nedEditor;
    }
}
