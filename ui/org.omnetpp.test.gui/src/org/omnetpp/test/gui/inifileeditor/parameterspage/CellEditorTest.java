package org.omnetpp.test.gui.inifileeditor.parameterspage;

import org.omnetpp.test.gui.inifileeditor.InifileEditorTestCase;

public class CellEditorTest extends InifileEditorTestCase {

    private void prepareTest(String content) throws Exception {
        createFileWithContent(content);
        openFileFromProjectExplorerView();
    }

    private void assertTextEditorContentMatches(String content) {
        findInifileEditor().ensureActiveTextEditor().assertContentIgnoringWhiteSpace(content);
    }

    public void testKeyUp1() throws Exception {
        // edit previous line
    }

    public void testKeyUp2() throws Exception {
        // edit top line (cannot move up)
    }

    public void testKeyDown() throws Exception {
        // edit next line
    }

    public void testKeyDown2() throws Exception {
        // edit bottom line (cannot move down)
    }

    public void testEscKey() throws Exception {
        // Esc should cancel editing
    }

    public void testFocusLost1() throws Exception {
        // left edge of same tree cell clicked, to select cell
    }

    public void testFocusLost2() throws Exception {
        // another cell clicked for editing
    }

    public void testFocusLost3() throws Exception {
        // another widget clicked
    }

    public void testContentAssistWithMouseSelection() throws Exception {
        // another widget clicked
    }

    public void testContentAssistWithKeyboardSelection() throws Exception {
        // another widget clicked
    }
    
}
