package org.omnetpp.test.gui.nededitor;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.access.GraphicalNedEditorAccess;

public class DeleteFromGraphicalEditor
    extends NedFileTestCase
{
    public void testDeleteSimpleModule() {
        GraphicalNedEditorAccess graphicalNedEditor = (GraphicalNedEditorAccess)findMultiPageEditor().activatePage("Graphical");
        graphicalNedEditor.clickModuleFigureWithName("TestSimple");
        graphicalNedEditor.pressKey(SWT.DEL);
    }
    
    public void testDeleteCompoundModule() {
        GraphicalNedEditorAccess graphicalNedEditor = (GraphicalNedEditorAccess)findMultiPageEditor().activatePage("Graphical");
        graphicalNedEditor.clickModuleFigureWithName("TestCompound");
        graphicalNedEditor.pressKey(SWT.DEL);
    }
    
    public void testDeleteSubmodule() {
        GraphicalNedEditorAccess graphicalNedEditor = (GraphicalNedEditorAccess)findMultiPageEditor().activatePage("Graphical");
        graphicalNedEditor.clickModuleFigureWithName("test");
        graphicalNedEditor.pressKey(SWT.DEL);
    }
    
    public void testDeleteConnection() {
        GraphicalNedEditorAccess graphicalNedEditor = (GraphicalNedEditorAccess)findMultiPageEditor().activatePage("Graphical");
    }
    
    public void testDeleteMultipleSubmodules() {
        GraphicalNedEditorAccess graphicalNedEditor = (GraphicalNedEditorAccess)findMultiPageEditor().activatePage("Graphical");
        graphicalNedEditor.holdDownModifiers(SWT.CONTROL);
        graphicalNedEditor.clickModuleFigureWithName("test");
        graphicalNedEditor.clickModuleFigureWithName("test1");
        graphicalNedEditor.clickModuleFigureWithName("test2");
        graphicalNedEditor.releaseUpModifiers(SWT.CONTROL);
        graphicalNedEditor.pressKey(SWT.DEL);
    }
    
    public void testDeleteAll() {
        GraphicalNedEditorAccess graphicalNedEditor = (GraphicalNedEditorAccess)findMultiPageEditor().activatePage("Graphical");
        graphicalNedEditor.pressKey('a', SWT.CONTROL);
        graphicalNedEditor.pressKey(SWT.DEL);
    }
    
    @Override
    protected void setUpInternal() throws Exception {
        super.setUpInternal();
        createFileWithContent("simple TestSimple { gates: inout g; }\nmodule TestCompound { submodules: test: TestSimple; test1: TestSimple; test2: TestSimple; connections: test1.g <--> test2.g; }");
        openFileFromProjectExplorerView();
    }
}
