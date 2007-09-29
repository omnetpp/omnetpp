package org.omnetpp.test.gui.inifileeditor;

import org.omnetpp.test.gui.access.InifileEditorAccess;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.WorkbenchWindowAccess;
import com.simulcraft.test.gui.core.ProjectFileTestCase;


public class InifileTestCase extends ProjectFileTestCase {
    public InifileTestCase() {
        super("test.ini");
    }

    protected InifileEditorAccess findInifileEditor() {
        WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
        return (InifileEditorAccess)workbenchWindow.findMultiPageEditorPartByTitle(fileName);
    }

    protected void createNewInifileByWizard() {
        InifileEditorUtils.createNewIniFileByWizard1(projectName, fileName, null);
    }

//  protected void typeIntoTextualNedEditor(String inifileContent) {
//  InifileEditorUtils.typeIntoTextualNedEditor(fileName, inifileContent);
//  }
}
