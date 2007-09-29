package org.omnetpp.test.gui.inifileeditor;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.test.gui.access.InifileEditorAccess;

import com.simulcraft.test.gui.util.WorkspaceUtils;

public class NewInifileWizardTest extends InifileEditorTestCase {

    protected void createUsingWizard(String projectName, String fileName, String networkName) throws Exception {
        WorkspaceUtils.ensureFileNotExists(projectName, fileName);
        InifileEditorUtils.createNewIniFileByWizard2(projectName, fileName, networkName);

        String expectedContent = "[General]\nnetwork = "+StringUtils.nullToEmpty(networkName)+"\n";
        WorkspaceUtils.assertFileExistsWithContent(filePath, expectedContent);
        
        InifileEditorAccess inifileEditor = findInifileEditor();
        inifileEditor.activateTextEditor().assertContent(expectedContent);
        inifileEditor.assertNotDirty();
    }

    public void testWizardWithDefaultNetwork() throws Exception {
        createUsingWizard(projectName, fileName, null);
    }

    public void testWizardWithEmptyNetwork() throws Exception {
        createUsingWizard(projectName, fileName, "");
    }

    public void testWizardWithNetworkFilledIn() throws Exception {
        createUsingWizard(projectName, fileName, "TestNetwork");
    }

}
