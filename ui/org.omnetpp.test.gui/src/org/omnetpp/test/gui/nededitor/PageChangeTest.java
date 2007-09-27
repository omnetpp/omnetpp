package org.omnetpp.test.gui.nededitor;

import junit.framework.Assert;

import org.omnetpp.test.gui.access.GraphicalNedEditorAccess;

import com.simulcraft.test.gui.access.MultiPageEditorPartAccess;
import com.simulcraft.test.gui.access.TextEditorAccess;


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
        TextEditorAccess textEditorAccess = (TextEditorAccess)multiPageEditorPart.ensureActiveEditor("Text");
        textEditorAccess.typeIn("simple NewSimpleModule {}\n");
        GraphicalNedEditorAccess graphicalNedEditor = (GraphicalNedEditorAccess)multiPageEditorPart.ensureActiveEditor("Graphical");
        graphicalNedEditor.findSimpleModule("NewSimpleModule");
    }

    private MultiPageEditorPartAccess prepareTest() throws Exception {
        createFileWithContent("simple TestSimpleModule {}");
        openFileFromProjectExplorerView();
        MultiPageEditorPartAccess multiPageEditorPart = findMultiPageEditor();
        return multiPageEditorPart;
    }
}
