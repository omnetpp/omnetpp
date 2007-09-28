package org.omnetpp.test.gui.nededitor;

import junit.framework.Assert;

import com.simulcraft.test.gui.access.MultiPageEditorPartAccess;
import com.simulcraft.test.gui.access.TextEditorAccess;

import org.eclipse.swt.SWT;

import org.omnetpp.test.gui.access.GraphicalNedEditorAccess;


public class PageChangeTest
    extends NedFileTestCase
{
    public void testSimplePageChangeFromTextToGraphical() throws Exception {
        MultiPageEditorPartAccess multiPageEditorPart = prepareTest();
        multiPageEditorPart.ensureActiveEditor("Text");
        multiPageEditorPart.ensureActiveEditor("Graphical");
    }

    public void testSimplePageChangeFromGraphicalToText() throws Exception {
        MultiPageEditorPartAccess multiPageEditorPart = prepareTest();
        multiPageEditorPart.ensureActiveEditor("Graphical");
        multiPageEditorPart.ensureActiveEditor("Text");
    }

    public void testFollowChangesFromGraphicalToText() throws Exception {
        MultiPageEditorPartAccess multiPageEditorPart = prepareTest();
        multiPageEditorPart.ensureActiveEditor("Text");
        GraphicalNedEditorAccess graphicalNedEditor = (GraphicalNedEditorAccess)multiPageEditorPart.ensureActiveEditor("Graphical");
        graphicalNedEditor.createSimpleModuleWithPalette("NewSimpleModule");
        TextEditorAccess textEditorAccess = (TextEditorAccess)multiPageEditorPart.ensureActiveEditor("Text");
        Assert.assertTrue(textEditorAccess.findStyledText().getText().matches("(?s).*NewSimpleModule.*"));
    }

    public void testFollowChangesFromTextToGraphical() throws Exception {
        MultiPageEditorPartAccess multiPageEditorPart = prepareTest();
        multiPageEditorPart.ensureActiveEditor("Graphical");
        TextEditorAccess textEditor = (TextEditorAccess)multiPageEditorPart.ensureActiveEditor("Text");
        textEditor.typeIn("simple NewSimpleModule {}\n");
        GraphicalNedEditorAccess graphicalNedEditor = (GraphicalNedEditorAccess)multiPageEditorPart.ensureActiveEditor("Graphical");
        graphicalNedEditor.findSimpleModule("NewSimpleModule");
    }

    /**
     * Tests whether submodule name changes in the text editor are reflected in the graphical editor.
     */
    public void testFollowSubmoduleNameChangesFromTextToGraphical() throws Exception {
        createFileWithContent("simple A {}\nmodule TestCompound { submodules: name : A; }");
        openFileFromProjectExplorerView();
        MultiPageEditorPartAccess multiPageEditorPart = findMultiPageEditor();
        TextEditorAccess textEditor = (TextEditorAccess)multiPageEditorPart.ensureActiveEditor("Text");
        textEditor.moveCursorAfter("name");
        textEditor.typeIn("x");
        GraphicalNedEditorAccess graphicalNedEditor = (GraphicalNedEditorAccess)multiPageEditorPart.ensureActiveEditor("Graphical");
        graphicalNedEditor.findCompoundModule("TestCompound").findSubmoduleFigureByName("namex");
        multiPageEditorPart.ensureActiveEditor("Text");
        textEditor.moveCursorAfter("namex");
        textEditor.pressKey(SWT.BS);
        multiPageEditorPart.ensureActiveEditor("Graphical");
        graphicalNedEditor.findCompoundModule("TestCompound").findSubmoduleFigureByName("name");
    }
    
    private MultiPageEditorPartAccess prepareTest() throws Exception {
        createFileWithContent("simple TestSimpleModule {}");
        openFileFromProjectExplorerView();
        MultiPageEditorPartAccess multiPageEditorPart = findMultiPageEditor();
        return multiPageEditorPart;
    }
}
