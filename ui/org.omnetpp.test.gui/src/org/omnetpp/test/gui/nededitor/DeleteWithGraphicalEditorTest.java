package org.omnetpp.test.gui.nededitor;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.access.GraphicalNedEditorAccess;

public class DeleteWithGraphicalEditorTest
    extends NedFileTestCase
{
    private GraphicalNedEditorAccess graphicalNedEditor;

    public void testDeleteSimpleModule() {
        graphicalNedEditor.cliclLabelFigure("TestSimple");
        graphicalNedEditor.pressKey(SWT.DEL);
    }
    
    public void testDeleteCompoundModule() {
        graphicalNedEditor.cliclLabelFigure("TestCompound");
        graphicalNedEditor.pressKey(SWT.DEL);
    }
    
    public void testDeleteSubmodule() {
        graphicalNedEditor.cliclLabelFigure("test");
        graphicalNedEditor.pressKey(SWT.DEL);
    }
    
    public void testDeleteConnection() {
        graphicalNedEditor.cliclConnectionFigure("test1", "test2");
        graphicalNedEditor.pressKey(SWT.DEL);
    }
    
    public void testDeleteMultipleSubmodules() {
        graphicalNedEditor.holdDownModifiers(SWT.CONTROL);
        graphicalNedEditor.cliclLabelFigure("test");
        graphicalNedEditor.cliclLabelFigure("test1");
        graphicalNedEditor.cliclLabelFigure("test2");
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
        graphicalNedEditor = (GraphicalNedEditorAccess)findMultiPageEditor().activatePage("Graphical");
    }
}
