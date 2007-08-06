package org.omnetpp.test.gui.inifileeditor;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Tree;
import org.omnetpp.test.gui.TestBase;
import org.omnetpp.test.gui.access.MenuAccess;
import org.omnetpp.test.gui.access.ShellAccess;
import org.omnetpp.test.gui.access.TreeAccess;
import org.omnetpp.test.gui.access.TreeItemAccess;
import org.omnetpp.test.gui.access.ViewPartAccess;
import org.omnetpp.test.gui.access.WorkbenchAccess;

public class IniFileEditorTest2 extends TestBase {
	protected String projectName = "test-project";
	protected String fileName = "omnetpp.ini";
	
	protected void ensureProjectFileDeleted(String projectName, String fileName) throws CoreException {
		IFile file = ResourcesPlugin.getWorkspace().getRoot().getProject(projectName).getFile(fileName);
		if (file.exists())
			file.delete(true, null);
	}

	protected void createNewIniFile() throws CoreException {
//		runStep(new Step() {
//			public void run() {
//				EventTracer.start();
//			}
//		});
		
		workbenchAccess.closeAllEditorPartsWithHotKey();

		ensureProjectFileDeleted(projectName, fileName);

		TreeAccess projectTreeAccess = workbenchAccess.findViewPartByPartName("Project Explorer", true).findTree();
		TreeItemAccess treeItem = projectTreeAccess.findTreeItemByContent(projectName);

		// Select New|Other... from context menu
		MenuAccess menuAccess = treeItem.activateContextMenuWithMouseClick();
		MenuAccess submenuAccess = menuAccess.activateMenuItemWithMouse("New");
		submenuAccess.activateMenuItemWithMouse("Other.*");

		// Open the "OMNEST/OMNeT++" category in the "New" dialog, and double-click "Ini file" in it
		ShellAccess shellAccess = workbenchAccess.findShellByTitle("New");
		TreeAccess treeAccess = new TreeAccess((Tree)shellAccess.findDescendantControl(shellAccess.getShell(), Tree.class));
		TreeItemAccess treeItemAccess = treeAccess.findTreeItemByContent(".*OMNEST.*");
		treeItemAccess.click();
		workbenchAccess.pressKey(SWT.ARROW_RIGHT); // open tree node
		treeAccess.findTreeItemByContent("Ini.*").doubleClick(); 

		// Enter the file name in the dialog, and click "Finish"
		ShellAccess shellAccess2 = workbenchAccess.findShellByTitle("New Ini File");
		shellAccess2.findTextNearLabel("File name.*").typeIn(fileName);
		shellAccess2.findButtonWithLabel("Finish").activateWithMouseClick();
		
		// make sure the file really exists
		runStepWithTimeout(3, new Step() {
			public void run() {
				IFile file = ResourcesPlugin.getWorkspace().getRoot().getProject(projectName).getFile(fileName);
				assertTrue(file.exists());
			}
		});
		
	}

	public void testCreateIniFile() throws Throwable {
		runTest(new Test() {
			public void run() throws Exception {
				createNewIniFile();
			}
		});
	}
	
	public void testWrongNetwok() throws Throwable {
		runTest(new Test() {
			public void run() throws Exception {
				createNewIniFile();

				WorkbenchAccess.sleep(3);
				
//				runStepWithTimeout(1, new Step() {
//					public void run() {
//						workbenchAccess.findEditorByTitle(fileName).activatePageInMultiPageEditorByLabel("Text");
//					}
//				});
				workbenchAccess.findEditorByTitle(fileName).activatePageInMultiPageEditorByLabel("Text");

				runStep(new Step() {
					public void run() {
						workbenchAccess.pressKey(SWT.ARROW_DOWN);
						workbenchAccess.pressKey(SWT.ARROW_DOWN);
						workbenchAccess.pressKey(SWT.END);
						workbenchAccess.typeIn(" Undefined");
						workbenchAccess.saveCurrentEditorPartWithHotKey();
					}
				});

				final ViewPartAccess viewPartAccess = (ViewPartAccess)runStepWithTimeout(1, new Step() {
					public Object runAndReturn() {
						ViewPartAccess viewPartAccess = workbenchAccess.findViewPartByPartName("Problems", true);
						viewPartAccess.activateWithMouseClick();

						return viewPartAccess;
					}
				});

				runStepWithTimeout(1, new Step() {
					public void run() {
						viewPartAccess.findTree().findTreeItemByContent(".*No such NED network.*");
					}
				});
			}
		});
	}
}
