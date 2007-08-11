package org.omnetpp.test.gui.access;

import java.util.ArrayList;

import junit.framework.Assert;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IEditorReference;
import org.eclipse.ui.IViewReference;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.test.gui.InUIThread;

public class WorkbenchWindowAccess extends CompositeAccess<Shell> {
	private IWorkbenchWindow workbenchWindow;
	
	public WorkbenchWindowAccess(IWorkbenchWindow workbenchWindow) {
		super(workbenchWindow.getShell());
		this.workbenchWindow = workbenchWindow;
	}

	public Shell getShell() {
		return widget;
	}
	
	public IWorkbenchWindow getWorkbenchWindow() {
		return workbenchWindow;
	}

	@InUIThread
	public MenuAccess getMenuBar() {
		return new MenuAccess(getWorkbenchWindow().getShell().getMenuBar());
	}
	
	/* no @InUIThread! */
	public void chooseFromMainMenu(String labelPath) {
		Assert.assertTrue("must be in a background thread for menu selection to work", Display.getCurrent()==null);

		MenuAccess menuAccess = getMenuBar();
		for (String label : labelPath.split("\\|"))
			//menuAccess = menuAccess.findMenuItemByLabel(label).activateWithMouseClick();
			menuAccess = menuAccess.activateMenuItemWithMouse(label);
	}
	
	@InUIThread
	public ViewPartAccess showViewPart(String viewId) throws PartInitException {
		return new ViewPartAccess(getWorkbenchWindow().getPages()[0].showView(viewId));
	}

	@InUIThread
	public ViewPartAccess findViewPartByPartName(String title) {
		return findViewPartByPartName(title, false);
	}

	@InUIThread
	public ViewPartAccess findViewPartByPartName(String title, boolean restore) {
		ArrayList<ViewPartAccess> result = new ArrayList<ViewPartAccess>();

		for (IWorkbenchPage page : getWorkbenchWindow().getPages()) {
			for (IViewReference viewReference : page.getViewReferences()) {
				if (debug)
					System.out.println("Looking at view part: " + viewReference.getPartName());

				if (viewReference.getPartName().matches(title))
					result.add(new ViewPartAccess((ViewPart)viewReference.getView(restore)));
			}
		}

		return (ViewPartAccess)theOnlyObject(result);
	}

	@InUIThread
	public EditorPartAccess findEditorByTitle(String title) {
		ArrayList<EditorPartAccess> result = new ArrayList<EditorPartAccess>();

		for (IWorkbenchPage page : getWorkbench().getActiveWorkbenchWindow().getPages()) {
			for (IEditorReference editorReference : page.getEditorReferences()) {
				if (debug)
					System.out.println("Looking at editor part: " + editorReference.getTitle());

				if (editorReference.getTitle().matches(title))
					result.add(new EditorPartAccess((IEditorPart)editorReference.getEditor(false)));
			}
		}

		return (EditorPartAccess)theOnlyObject(result);
	}

	@InUIThread
	public void closeAllEditorPartsWithHotKey() {
		pressKey('w', SWT.CONTROL + SWT.SHIFT);
	}

	@InUIThread
	public void saveCurrentEditorPartWithHotKey() {
		pressKey('s', SWT.CONTROL);
	}
}
