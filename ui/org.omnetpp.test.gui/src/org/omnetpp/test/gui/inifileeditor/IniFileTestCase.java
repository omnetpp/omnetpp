package org.omnetpp.test.gui.inifileeditor;

import org.omnetpp.test.gui.access.InifileEditorAccess;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.WorkbenchWindowAccess;
import com.simulcraft.test.gui.core.ProjectFileTestCase;


public class IniFileTestCase extends ProjectFileTestCase {
    public IniFileTestCase() {
        super("test.ini");
    }

    protected InifileEditorAccess findInifileEditor() {
        WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
        return (InifileEditorAccess)workbenchWindow.findMultiPageEditorPartByTitle(fileName);
    }

    protected void createNewInifileByWizard() {
        IniFileEditorUtils.createNewIniFileByWizard1(projectName, fileName, null);
    }

//  protected void typeIntoTextualNedEditor(String inifileContent) {
//  IniFileEditorUtils.typeIntoTextualNedEditor(fileName, inifileContent);
//  }
}
