package org.omnetpp.test.gui.inifileeditor;

import org.omnetpp.test.gui.access.InifileEditorAccess;

import com.simulcraft.test.gui.util.WorkspaceUtils;

public class NewInifileWizardTest extends InifileEditorTestCase {

    public void testWizardResult() throws Exception {
        WorkspaceUtils.ensureFileNotExists(projectName, fileName);
        InifileEditorUtils.createNewIniFileByWizard2(projectName, fileName, "some-network");
        InifileEditorAccess inifileEditor = findInifileEditor();
        inifileEditor.activateTextEditor().assertContent("bla-bla");
    }

    //TODO

}
