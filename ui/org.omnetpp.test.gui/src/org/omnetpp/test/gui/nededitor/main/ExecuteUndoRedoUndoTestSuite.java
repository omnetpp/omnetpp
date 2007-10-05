package org.omnetpp.test.gui.nededitor.main;

import junit.framework.Assert;
import junit.framework.TestSuite;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.EditorPartAccess;
import com.simulcraft.test.gui.access.TextEditorAccess;
import com.simulcraft.test.gui.core.GUITestCase;
import com.simulcraft.test.gui.core.InUIThread;

import org.eclipse.swt.SWT;

import org.omnetpp.test.gui.access.GraphicalNedEditorAccess;
import org.omnetpp.test.gui.nededitor.NedFileTestCase;

public class ExecuteUndoRedoUndoTestSuite
    extends GUITestCase
{
    private enum EditorDesignator {
        Graphical,
        Text,
    }

    public static junit.framework.Test suite() {
        TestSuite testSuite = new TestSuite();
        addTests(testSuite, new ExecuteUndoRedoUndoTest() {
            public void execute() throws Exception {
                try {
                    TextEditorAccess textEditor = findNedEditor().ensureActiveTextEditor();
                    textEditor.typeIn("simple TestSimpleModule {}");
                }
                catch (Exception e) {
                    throw new RuntimeException(e);
                }
            }
        }, "(?s).*TestSimpleModule.*");
        addTests(testSuite, new ExecuteUndoRedoUndoTest() {
            public void execute() throws Exception {
                try {
                    GraphicalNedEditorAccess graphicalNedEditor = findNedEditor().ensureActiveGraphicalEditor();
                    graphicalNedEditor.createSimpleModuleWithPalette(null);
                }
                catch (Exception e) {
                    throw new RuntimeException(e);
                }
            }
        }, "(?s).*Unnamed.*");

        return testSuite;
    }

    public static void addTests(TestSuite testSuite, ExecuteUndoRedoUndoTest test, String match) {
        for (double sleepTime = 0; sleepTime <= 1; sleepTime++)
            for (final EditorDesignator firstUndoIn : EditorDesignator.values()) {
                for (final EditorDesignator redoIn : EditorDesignator.values()) {
                    for (final EditorDesignator secondUndoIn : EditorDesignator.values()) {
                        try {
                            ExecuteUndoRedoUndoTest testClone = (ExecuteUndoRedoUndoTest)test.clone();
                            testClone.sleepTime = sleepTime;
                            testClone.match = match;
                            testClone.firstUndoIn = firstUndoIn;
                            testClone.redoIn = redoIn;
                            testClone.secondUndoIn = secondUndoIn;
                            testSuite.addTest(testClone);
                        }
                        catch (CloneNotSupportedException e) {
                            // void
                        }
                    }
                }
            }
    }

    private static abstract class ExecuteUndoRedoUndoTest
        extends NedFileTestCase implements Cloneable
    {
        private double sleepTime;

        private String match;
        
        private EditorDesignator firstUndoIn;

        private EditorDesignator redoIn;
        
        private EditorDesignator secondUndoIn;

        public abstract void execute() throws Exception;
        
        @Override
        protected void setUpInternal() throws Exception {
            super.setUpInternal();
            createEmptyFile();
            openFileFromProjectExplorerView();
        }
        
        @Override
        public String getName() {
            return "ExecuteUndoRedoUndo, sleepTime: " + sleepTime + ", firstUndoIn: " + firstUndoIn + ", redoIn: " + redoIn + ", secondUndoIn: " + secondUndoIn + ", match: " + match;
        }
        
        @Override
        public Object clone() throws CloneNotSupportedException {
            return super.clone();
        }

        @Override
        protected void runTest() throws Throwable {
            test();
        }

        public void test() throws Exception {
            assertDoesNotMatch(match);
            execute();
            assertMatches(match);
            Access.sleep(sleepTime);
            undoIn(firstUndoIn);
            assertDoesNotMatch(match);
            Access.sleep(sleepTime);
            redoIn(redoIn);
            assertMatches(match);
            Access.sleep(sleepTime);
            undoIn(secondUndoIn);
            assertDoesNotMatch(match);
        }

        private void undoIn(EditorDesignator designator) {
            ensureActiveEditor(designator).pressKey('Z', SWT.CONTROL);
        }

        private void redoIn(EditorDesignator designator) {
            ensureActiveEditor(designator).pressKey('Y', SWT.CONTROL);
        }

        @InUIThread
        private void assertMatches(String content) {
            Assert.assertTrue(getText().matches(content));
        }

        @InUIThread
        private void assertDoesNotMatch(String content) {
            Assert.assertFalse(getText().matches(content));
        }

        private EditorPartAccess ensureActiveEditor(EditorDesignator designator) {
            String editorLabel = designator == EditorDesignator.Graphical ? "Graphical" : "Text";
            return findNedEditor().ensureActiveEditor(editorLabel);
        }

        private String getText() {
            return findNedEditor().getTextualNedEditor().getTextContent();
        }
    }
}
