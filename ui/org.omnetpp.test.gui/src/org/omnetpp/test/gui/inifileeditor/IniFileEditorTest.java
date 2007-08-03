package org.omnetpp.test.gui.inifileeditor;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.TestBase;
import org.omnetpp.test.gui.access.MenuAccess;
import org.omnetpp.test.gui.access.ShellAccess;
import org.omnetpp.test.gui.access.TreeAccess;
import org.omnetpp.test.gui.access.ViewPartAccess;

public class IniFileEditorTest extends TestBase {
	protected String projectName = "queuenet";
	protected String fileName = "omnetpp.ini";
	
	protected void ensureProjectFileDeleted(String projectName, String fileName) throws CoreException {
		IFile file = ResourcesPlugin.getWorkspace().getRoot().getProject(projectName).getFile(fileName);
		
		if (file.exists())
			file.delete(true, null);
	}

	protected void createNewIniFile() throws CoreException {
		runStep(new Step() {
			public void run() {
				workbenchAccess.closeAllEditorPartsWithHotKey();
			}
		});

		ensureProjectFileDeleted(projectName, fileName);

		final MenuAccess menuAccess = (MenuAccess)runStep(new Step() {
			public Object runAndReturn() {
				TreeAccess treeAccess = workbenchAccess.findViewPartByPartName("Navigator", true).findTree();
				return treeAccess.findTreeItemByContent(projectName).activateContextMenuWithMouseClick();
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
				shellAccess.findTextNearLabel("File name.*").typeIn(fileName);
				shellAccess.findButtonWithLabel("Finish").activateWithMouseClick();
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
				
				//workbenchAccess.sleep(1);

				runStepWithTimeout(1, new Step() {
					public void run() {
						workbenchAccess.findEditorByTitle(fileName).activatePageInMultiPageEditorByLabel("Text");
					}
				});

				runStep(new Step() {
					public void run() {
						workbenchAccess.pressKey(SWT.ARROW_DOWN);
						workbenchAccess.pressKey(SWT.ARROW_DOWN);
						workbenchAccess.pressKey(SWT.END);
						workbenchAccess.typeIn(" Undefined");
						workbenchAccess.saveCurrentEditorPartWithHotKey();
					}
				});
				
				//workbenchAccess.sleep(1);

				final ViewPartAccess viewPartAccess = (ViewPartAccess)runStepWithTimeout(1, new Step() {
					public Object runAndReturn() {
						ViewPartAccess viewPartAccess = workbenchAccess.findViewPartByPartName("Problems", true);
						viewPartAccess.activateWithMouseClick();

						return viewPartAccess;
					}
				});

				//workbenchAccess.sleep(1);

				runStepWithTimeout(1, new Step() {
					public void run() {
						viewPartAccess.findTree().findTreeItemByContent(".*No such NED network.*");
					}
				});
			}
		});
	}
}
