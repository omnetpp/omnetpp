package org.omnetpp.test.gui.nededitor;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.access.GraphicalNedEditorAccess;

public class DeleteWithGraphicalEditorTest
    extends NedFileTestCase
{
    private GraphicalNedEditorAccess graphicalNedEditor;

    public void testDeleteSimpleModule() {
        graphicalNedEditor.clickLabelFigure("TestSimple");
        graphicalNedEditor.pressKey(SWT.DEL);
    }
    
    public void testDeleteCompoundModule() {
        graphicalNedEditor.clickLabelFigure("TestCompound");
        graphicalNedEditor.pressKey(SWT.DEL);
    }
    
    public void testDeleteSubmodule() {
        graphicalNedEditor.clickLabelFigure("test");
        graphicalNedEditor.pressKey(SWT.DEL);
    }
    
    public void testDeleteConnection() {
        graphicalNedEditor.clickConnectionFigure("test1", "test2");
        graphicalNedEditor.pressKey(SWT.DEL);
    }
    
    public void testDeleteMultipleSubmodules() {
        graphicalNedEditor.holdDownModifiers(SWT.CONTROL);
        graphicalNedEditor.clickLabelFigure("test");
        graphicalNedEditor.clickLabelFigure("test1");
        graphicalNedEditor.clickLabelFigure("test2");
        graphicalNedEditor.releaseUpModifiers(SWT.CONTROL);
        graphicalNedEditor.pressKey(SWT.DEL);
    }
    
    public void testDeleteAll() {
        graphicalNedEditor.pressKey('a', SWT.CONTROL);
        graphicalNedEditor.pressKey(SWT.DEL);
    }
    
    @Override
    protected void setUpInternal() throws Exception {
        super.setUpInternal();
        createFileWithContent("simple TestSimple { gates: inout g; }\nmodule TestCompound { submodules: test: TestSimple; test1: TestSimple; test2: TestSimple; connections: test1.g <--> test2.g; }");
        openFileFromProjectExplorerView();
        graphicalNedEditor = (GraphicalNedEditorAccess)findMultiPageEditor().activatePageEditor("Graphical");
    }
}
