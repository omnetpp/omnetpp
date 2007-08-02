package org.omnetpp.test.gui.access;

import java.util.ArrayList;

import junit.framework.Assert;

import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IViewReference;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.ViewPart;

public class WorkbenchAccess extends Access
{
	public ViewPartAccess showViewPart(String viewId) throws PartInitException {
		return new ViewPartAccess(getWorkbench().getActiveWorkbenchWindow().getPages()[0].showView(viewId));
	}
	
	public ViewPartAccess findViewPartByLabel(String label) {
		ArrayList<ViewPartAccess> result = new ArrayList<ViewPartAccess>();

		for (IWorkbenchPage page : getWorkbench().getActiveWorkbenchWindow().getPages())
			for (IViewReference viewReference : page.getViewReferences())
				if (viewReference.getTitle().matches(label))
					result.add(new ViewPartAccess((ViewPart)viewReference.getView(false)));

		Assert.assertTrue(result.size() == 1);
		
		return result.get(0);
	}

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
}
