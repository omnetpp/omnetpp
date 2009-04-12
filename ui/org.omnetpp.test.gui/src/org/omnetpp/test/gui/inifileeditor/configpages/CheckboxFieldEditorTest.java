package org.omnetpp.test.gui.inifileeditor.configpages;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.access.InifileFormEditorAccess;
import org.omnetpp.test.gui.inifileeditor.InifileEditorTestCase;

import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.TableAccess;
import com.simulcraft.test.gui.access.TextAccess;
import com.simulcraft.test.gui.access.TreeAccess;
import com.simulcraft.test.gui.access.WorkbenchWindowAccess;
import com.simulcraft.test.gui.util.WorkspaceUtils;

public class CheckboxFieldEditorTest extends InifileEditorTestCase {
    private InifileFormEditorAccess prepareTest() throws Exception {
        createFileWithContent("");
        openFileFromProjectExplorerView();
        return findInifileEditor().ensureActiveFormEditor();
    }

    private void assertTextEditorContentMatches(String content) {
        findInifileEditor().ensureActiveTextEditor().assertContentIgnoringWhiteSpace(content);
    }

    public void testCheckboxField() throws Exception {
//        InifileFormEditorAccess formEditor = prepareTest();
//        formEditor.activateCategoryPage("FIXME").get...
//        assertTextEditorContentMatches(...);
    }
}
