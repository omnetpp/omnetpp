package org.omnetpp.test.gui.inifileeditor;

import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.ui.IPageLayout;
import org.omnetpp.test.gui.TestBase;
import org.omnetpp.test.gui.access.ButtonAccess;
import org.omnetpp.test.gui.access.MenuAccess;
import org.omnetpp.test.gui.access.ShellAccess;
import org.omnetpp.test.gui.access.TextAccess;
import org.omnetpp.test.gui.access.TreeAccess;
import org.omnetpp.test.gui.access.TreeItemAccess;

public class IniFileEditorTest extends TestBase {
	public void testCreateIniFile() throws Throwable {
		runTest(new Test() {
			public void run() throws Exception {
				final String projectName = "queuenet";
				final String fileName = "omnetpp.ini";
				
				ResourcesPlugin.getWorkspace().getRoot().getProject(projectName).getFile(fileName).delete(true, null);

				runStep(new Step() {
					public void run() throws Exception {
						workbenchAccess.showViewPart(IPageLayout.ID_RES_NAV);
					}
				});

				final MenuAccess menuAccess = (MenuAccess)runStep(new Step() {
					public Object runAndReturn() {
						TreeAccess treeAccess = workbenchAccess.findViewPartByLabel("Navigator").findTree();
						TreeItemAccess treeItemAccess = treeAccess.findTreeItemByLabel(projectName);
						return treeItemAccess.activateContextMenuWithMouseClick();
					}
				});

				final MenuAccess menuAccessInner = (MenuAccess)runStep(new Step() {
					public Object runAndReturn() {
						return menuAccess.findMenuItemByLabel("New").activateWithMouseClick();
					}
				});

				runStep(new Step() {
					public void run() {
						menuAccessInner.findMenuItemByLabel("Initialization File.*").activateWithMouseClick();
					}
				});

				runStep(new Step() {
					public void run() {
						ShellAccess shellAccess = workbenchAccess.findShellByTitle("New Ini File");
						TextAccess textAccess = shellAccess.findTextNearLabel("File name.*");
						textAccess.typeIn(fileName);
						ButtonAccess buttonAccess = shellAccess.findButtonWithLabel("Finish");
						buttonAccess.activateWithMouseClick();
					}
				});
			}
		});
	}
}
