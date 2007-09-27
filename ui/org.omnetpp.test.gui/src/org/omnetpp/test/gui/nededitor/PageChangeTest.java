package org.omnetpp.test.gui.nededitor;

import junit.framework.Assert;

import org.omnetpp.test.gui.access.GraphicalNedEditorAccess;

import com.simulcraft.test.gui.access.MultiPageEditorPartAccess;
import com.simulcraft.test.gui.access.TextEditorAccess;


public class PageChangeTest
    extends NedFileTestCase
{
    public void testPageChangeFromTextToGraphical() throws Exception {
        MultiPageEditorPartAccess multiPageEditorPart = prepareTest();
        multiPageEditorPart.activatePage("Text");
        multiPageEditorPart.activatePage("Graphical");
    }

    public void testPageChangeFromGraphicalToText() throws Exception {
        MultiPageEditorPartAccess multiPageEditorPart = prepareTest();
        multiPageEditorPart.activatePage("Graphical");
        multiPageEditorPart.activatePage("Text");
    }

    public void testFollowChangesFromGraphicalToText() throws Exception {
        MultiPageEditorPartAccess multiPageEditorPart = prepareTest();
        multiPageEditorPart.activatePage("Text");
        GraphicalNedEditorAccess graphicalNedEditor = (GraphicalNedEditorAccess)multiPageEditorPart.activatePage("Graphical");
        graphicalNedEditor.createSimpleModuleWithPalette("NewSimpleModule");
        TextEditorAccess textEditorAccess = (TextEditorAccess)multiPageEditorPart.activatePage("Text");
        Assert.assertTrue(textEditorAccess.findStyledText().getText().matches("(?s).*NewSimpleModule.*"));
    }

    public void testFollowChangesFromTextToGraphical() throws Exception {
        MultiPageEditorPartAccess multiPageEditorPart = prepareTest();
        multiPageEditorPart.activatePage("Graphical");
        TextEditorAccess textEditorAccess = (TextEditorAccess)multiPageEditorPart.activatePage("Text");
        textEditorAccess.typeIn("simple NewSimpleModule {}\n");
        GraphicalNedEditorAccess graphicalNedEditor = (GraphicalNedEditorAccess)multiPageEditorPart.activatePage("Graphical");
        graphicalNedEditor.findSimpleModule("NewSimpleModule");
    }

    private MultiPageEditorPartAccess prepareTest() throws Exception {
        createFileWithContent("simple TestSimpleModule {}");
        openFileFromProjectExplorerView();
        MultiPageEditorPartAccess multiPageEditorPart = findMultiPageEditor();
        return multiPageEditorPart;
    }
}
