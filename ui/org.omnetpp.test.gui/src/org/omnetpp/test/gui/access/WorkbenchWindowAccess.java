package org.omnetpp.test.gui.access;

import java.util.ArrayList;

import org.eclipse.swt.SWT;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IEditorReference;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IViewReference;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.test.gui.core.InUIThread;
import org.omnetpp.test.gui.core.NotInUIThread;

public class WorkbenchWindowAccess extends ShellAccess {
	private IWorkbenchWindow workbenchWindow;
	
	public WorkbenchWindowAccess(IWorkbenchWindow workbenchWindow) {
		super(workbenchWindow.getShell());
		this.workbenchWindow = workbenchWindow;
	}

	public IWorkbenchWindow getWorkbenchWindow() {
		return workbenchWindow;
	}

	@InUIThread
	public MenuAccess getMenuBar() {
		return new MenuAccess(getWorkbenchWindow().getShell().getMenuBar());
	}
	
	@NotInUIThread
	public void chooseFromMainMenu(String labelPath) {
		MenuAccess menuAccess = getMenuBar();
		for (String label : labelPath.split("\\|"))
			//menuAccess = menuAccess.findMenuItemByLabel(label).activateWithMouseClick();
			menuAccess = menuAccess.activateMenuItemWithMouse(label);
	}
	
	@InUIThread
	public EditorPartAccess getActiveEditorPart() {
		return new EditorPartAccess(getWorkbenchWindow().getPages()[0].getActiveEditor());
	}

	@InUIThread
	public WorkbenchPartAccess getActivePart() {
		IWorkbenchPart activePart = getWorkbenchWindow().getPages()[0].getActivePart();
		if (activePart instanceof IEditorPart)
			return new EditorPartAccess((IEditorPart)activePart);
		else 
			return new ViewPartAccess((IViewPart)activePart);
	}

	@InUIThread
	public ViewPartAccess showViewPart(String viewId) throws PartInitException {
		return new ViewPartAccess(getWorkbenchWindow().getPages()[0].showView(viewId));
	}

	@InUIThread
	public ViewPartAccess findViewPartByTitle(String title) {
		return findViewPartByTitle(title, true);
	}

	@InUIThread
	public ViewPartAccess findViewPartByTitle(String title, boolean restore) {
		ArrayList<ViewPartAccess> result = new ArrayList<ViewPartAccess>();

		for (IWorkbenchPage page : getWorkbenchWindow().getPages()) {
			for (IViewReference viewReference : page.getViewReferences()) {
				System.out.println("  checking viewpart: " + viewReference.getPartName());
				if (viewReference.getPartName().matches(title))
					result.add(new ViewPartAccess((ViewPart)viewReference.getView(restore)));
			}
		}

		return (ViewPartAccess)theOnlyObject(result);
	}

	@InUIThread
	public EditorPartAccess findEditorPartByTitle(String title) {
		return findEditorPartByTitle(title, true);
	}
	
	@InUIThread
	public EditorPartAccess findEditorPartByTitle(String title, boolean restore) {
		ArrayList<EditorPartAccess> result = new ArrayList<EditorPartAccess>();

		for (IWorkbenchPage page : getWorkbenchWindow().getPages()) {
			for (IEditorReference editorReference : page.getEditorReferences()) {
				System.out.println("  checking editorpart: " + editorReference.getTitle());
				if (editorReference.getTitle().matches(title))
					result.add(new EditorPartAccess((IEditorPart)editorReference.getEditor(restore)));
			}
		}
		return (EditorPartAccess)theOnlyObject(result);
	}

	@InUIThread
	public void closeAllEditorPartsWithHotKey() {
		pressKey('w', SWT.CONTROL + SWT.SHIFT);
	}
}
