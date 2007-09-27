package org.omnetpp.test.gui.nededitor;

import junit.framework.Assert;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.access.MultiPageNedEditorAccess;

import com.simulcraft.test.gui.access.EditorPartAccess;
import com.simulcraft.test.gui.access.TextEditorAccess;

public class ExecuteUndoRedoUndo
    extends NedFileTestCase
{
    private enum EditorDesignator {
        Graphical,
        Text,
    }

    public void test() throws Exception {
        doTest(new Runnable() {
            public void run() {
                try {
                    createEmptyFile();
                    openFileFromProjectExplorerView();
                    TextEditorAccess textEditor = (TextEditorAccess)findMultiPageEditor().ensureActiveEditor("Text");
                    textEditor.typeIn("simple TestSimpleModule");
                }
                catch (Exception e) {
                    throw new RuntimeException(e);
                }
            }
        }, "(?s).*TestSimpleModule.*");
    }
    
    public void doTest(Runnable runnable, String match) throws Exception {
        for (EditorDesignator firstUndoIn : EditorDesignator.values()) {
            for (EditorDesignator redoIn : EditorDesignator.values()) {
                for (EditorDesignator secondUndoIn : EditorDesignator.values()) {
                    setUpInternal();
                    assertDoesNotMatch(match);
                    runnable.run();
                    assertMatches(match);
                    undoIn(firstUndoIn);
                    assertDoesNotMatch(match);
                    redoIn(redoIn);
                    assertMatches(match);
                    undoIn(secondUndoIn);
                    assertDoesNotMatch(match);
                    tearDownInternal();
                }
            }
        }
    }

    private void undoIn(EditorDesignator designator) {
        ensureActiveEditor(designator).pressKey('Z', SWT.CONTROL);
    }

    private void redoIn(EditorDesignator designator) {
        ensureActiveEditor(designator).pressKey('Y', SWT.CONTROL);
    }

    private void assertMatches(String match) {
        Assert.assertTrue(getText().matches(match));
    }

    private void assertDoesNotMatch(String match) {
        Assert.assertTrue(!getText().matches(match));
    }

    private EditorPartAccess ensureActiveEditor(EditorDesignator designator) {
        String editorLabel = designator == EditorDesignator.Graphical ? "Graphical" : "Text";
        return findMultiPageEditor().ensureActiveEditor(editorLabel);
    }

    private String getText() {
        return ((MultiPageNedEditorAccess)findMultiPageEditor()).getTextualNedEditor().getText();
    }
}
