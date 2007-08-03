package org.omnetpp.test.gui.access;

import java.util.ArrayList;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IEditorReference;
import org.eclipse.ui.IViewReference;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.ViewPart;

public class WorkbenchAccess extends Access
{
	public ShellAccess findShellByTitle(final String title) {
		return new ShellAccess((Shell)findWidget(getDisplay().getShells(), new IPredicate() {
			public boolean matches(Object object) {
				Shell shell = (Shell)object;
				
				if (debug)
					System.out.println("Trying to collect shell: " + shell.getText());

				return shell.getText().matches(title);
			}
		}));
	}

	public ViewPartAccess showViewPart(String viewId) throws PartInitException {
		return new ViewPartAccess(getWorkbench().getActiveWorkbenchWindow().getPages()[0].showView(viewId));
	}
	
	public ViewPartAccess findViewPartByPartName(String title) {
		return findViewPartByPartName(title, false);
	}

	public ViewPartAccess findViewPartByPartName(String title, boolean restore) {
		ArrayList<ViewPartAccess> result = new ArrayList<ViewPartAccess>();

		for (IWorkbenchPage page : getWorkbench().getActiveWorkbenchWindow().getPages()) {
			for (IViewReference viewReference : page.getViewReferences()) {
				if (debug)
					System.out.println("Looking at view part: " + viewReference.getPartName());

				if (viewReference.getPartName().matches(title))
					result.add(new ViewPartAccess((ViewPart)viewReference.getView(restore)));
			}
		}

		return (ViewPartAccess)theOnlyObject(result);
	}

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

	public void closeAllEditorPartsWithHotKey() {
		pressKey('w', SWT.CONTROL + SWT.SHIFT);
	}

	public void saveCurrentEditorPartWithHotKey() {
		pressKey('s', SWT.CONTROL);
	}
}
