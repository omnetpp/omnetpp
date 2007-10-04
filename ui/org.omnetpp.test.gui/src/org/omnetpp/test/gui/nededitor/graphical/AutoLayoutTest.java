package org.omnetpp.test.gui.nededitor.graphical;

import junit.framework.Assert;

import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.test.gui.access.CompoundModuleEditPartAccess;
import org.omnetpp.test.gui.access.GraphicalNedEditorAccess;
import org.omnetpp.test.gui.nededitor.NedFileTestCase;

public class AutoLayoutTest
    extends NedFileTestCase
{
    public void testNoAutoLayoutUponSubmoduleInsertionFromGraphicalEditor() throws Exception {
        createFileWithContent("simple Test {}\nmodule TestCompoundModule { submodules: test1: Test; test2: Test; test3: Test; test4: Test; }");
        openFileFromProjectExplorerView();
        GraphicalNedEditorAccess graphicalNedEditor = findNedEditor().ensureActiveGraphicalEditor();
        CompoundModuleEditPartAccess compoundModuleEditPart = graphicalNedEditor.findCompoundModule("TestCompoundModule");
        Rectangle r1 = compoundModuleEditPart.findSubmoduleFigureByName("test1").getBounds();
        Rectangle r2 = compoundModuleEditPart.findSubmoduleFigureByName("test2").getBounds();
        Rectangle r3 = compoundModuleEditPart.findSubmoduleFigureByName("test3").getBounds();
        Rectangle r4 = compoundModuleEditPart.findSubmoduleFigureByName("test4").getBounds();
        compoundModuleEditPart.createSubModuleWithPalette("Test", "testNew", 200, 200);
        Assert.assertEquals(r1, compoundModuleEditPart.findSubmoduleFigureByName("test1").getBounds());
        Assert.assertEquals(r2, compoundModuleEditPart.findSubmoduleFigureByName("test2").getBounds());
        Assert.assertEquals(r3, compoundModuleEditPart.findSubmoduleFigureByName("test3").getBounds());
        Assert.assertEquals(r4, compoundModuleEditPart.findSubmoduleFigureByName("test4").getBounds());
    }
}
