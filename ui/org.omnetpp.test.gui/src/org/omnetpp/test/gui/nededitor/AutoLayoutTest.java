package org.omnetpp.test.gui.nededitor;

import junit.framework.Assert;

import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.test.gui.access.CompoundModuleEditPartAccess;
import org.omnetpp.test.gui.access.GraphicalNedEditorAccess;

public class AutoLayoutTest
    extends NedFileTestCase
{
    public void testNoAutoLayoutUponSubmoduleInsertionFromGraphicalEditor() throws Exception {
        createFileWithContent("simple TestSimpleModule {}\nmodule TestCompoundModule { submodules: test1: TestSimpleModule; test2: TestSimpleModule; }");
        openFileFromProjectExplorerView();
        GraphicalNedEditorAccess graphicalNedEditor = (GraphicalNedEditorAccess)findMultiPageEditor().ensureActiveEditor("Graphical");
        CompoundModuleEditPartAccess compoundModuleEditPart = graphicalNedEditor.findCompoundModule("TestCompoundModule");
        Rectangle r1 = compoundModuleEditPart.findSubmoduleFigureByName("test1").getBounds();
        Rectangle r2 = compoundModuleEditPart.findSubmoduleFigureByName("test2").getBounds();
        compoundModuleEditPart.createSubModuleWithPalette("TestSimpleModule", "test3", 100, 100);
        Assert.assertEquals(r1, compoundModuleEditPart.findSubmoduleFigureByName("test1").getBounds());
        Assert.assertEquals(r2, compoundModuleEditPart.findSubmoduleFigureByName("test2").getBounds());
    }
}
