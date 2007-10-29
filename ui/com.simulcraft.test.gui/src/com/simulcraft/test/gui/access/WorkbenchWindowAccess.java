package com.simulcraft.test.gui.access;

import java.util.ArrayList;
import java.util.List;

import junit.framework.Assert;

import org.eclipse.swt.SWT;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IEditorReference;
import org.eclipse.ui.IPerspectiveDescriptor;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IViewReference;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.common.util.IPredicate;

import com.simulcraft.test.gui.core.UIStep;
import com.simulcraft.test.gui.core.InBackgroundThread;


public class WorkbenchWindowAccess extends Access {
	private IWorkbenchWindow workbenchWindow;
	
	public WorkbenchWindowAccess(IWorkbenchWindow workbenchWindow) {
		this.workbenchWindow = workbenchWindow;
	}
	
    @UIStep
	public ShellAccess getShell() {
		return new ShellAccess(workbenchWindow.getShell());
	}

	@UIStep
	public MenuAccess getMenuBar() {
		return getShell().getMenuBar();
	}
	
    @UIStep
    public IPerspectiveDescriptor getPerspective() {
        return workbenchWindow.getWorkbench().getActiveWorkbenchWindow().getActivePage().getPerspective();
    }

	@InBackgroundThread
	public void chooseFromMainMenu(String labelPath) {
	    getShell().chooseFromMainMenu(labelPath);
	}
	
	@UIStep
	public EditorPartAccess getActiveEditorPart() {
		return new EditorPartAccess(workbenchWindow.getPages()[0].getActiveEditor());
	}

	@UIStep
	public WorkbenchPartAccess getActivePart() {
		IWorkbenchPart activePart = workbenchWindow.getPages()[0].getActivePart();
		if (activePart instanceof IEditorPart)
			return new EditorPartAccess((IEditorPart)activePart);
		else 
			return (WorkbenchPartAccess)createAccess((IViewPart)activePart);
	}

	@UIStep
	public ViewPartAccess showViewPart(String viewId) throws PartInitException {
		return new ViewPartAccess(workbenchWindow.getPages()[0].showView(viewId));
	}

	@UIStep
	public ViewPartAccess findViewPartByTitle(String title) {
		return findViewPartByTitle(title, true);
	}

	@UIStep
	public ViewPartAccess findViewPartByTitle(String title, boolean restore) {
	    IPredicate predicate = getViewPartsByTitlePredicate(title);
		return (ViewPartAccess)theOnlyObject(collectViewParts(predicate, restore), predicate);
	}

	@UIStep
    public List<ViewPartAccess> collectViewPartsByTitle(String title, boolean restore) {
        return collectViewParts(getViewPartsByTitlePredicate(title), restore);
    }

    protected IPredicate getViewPartsByTitlePredicate(final String title) {
        return new IPredicate() {
            public boolean matches(Object object) {
                return ((IViewReference)object).getPartName().matches(title);
            }
        };
    }

    @UIStep
    public List<ViewPartAccess> collectViewParts(IPredicate predicate, boolean restore) {
        ArrayList<ViewPartAccess> result = new ArrayList<ViewPartAccess>();

        for (IWorkbenchPage page : workbenchWindow.getPages()) {
            for (IViewReference viewReference : page.getViewReferences()) {
                log(debug, "  checking viewpart: " + viewReference.getPartName());
                if (predicate.matches(viewReference))
                    result.add(new ViewPartAccess((ViewPart)viewReference.getView(restore)));
            }
        }

        return result;
    }

	@UIStep
	public EditorPartAccess findEditorPartByTitle(String title) {
		return findEditorPartByTitle(title, true);
	}
	
	@UIStep
	public MultiPageEditorPartAccess findMultiPageEditorPartByTitle(String title) {
		return (MultiPageEditorPartAccess)findEditorPartByTitle(title, true);
	}
	
	@UIStep
	public EditorPartAccess findEditorPartByTitle(final String title, boolean restore) {
	    IPredicate predicate = getEditorPartsByTitlePredicate(title);
		return (EditorPartAccess)theOnlyObject(collectEditorParts(predicate, restore), predicate);
	}

	@UIStep
    public boolean hasEditorPartWithTitle(String title) {
	    return hasEditorPartWithTitle(title, false);
	}

    @UIStep
    public boolean hasEditorPartWithTitle(String title, boolean restore) {
        return collectEditorPartsByTitle(title, restore).size() == 1;
    }

    @UIStep
    public List<EditorPartAccess> collectEditorPartsByTitle(String title, boolean restore) {
        return collectEditorParts(getEditorPartsByTitlePredicate(title), restore);
    }

    protected IPredicate getEditorPartsByTitlePredicate(final String title) {
        return new IPredicate() {
            public boolean matches(Object object) {
                return ((IEditorReference)object).getTitle().matches(title);
            }
        };
    }

    @UIStep
    public List<EditorPartAccess> collectEditorParts(IPredicate predicate, boolean restore) {
        ArrayList<EditorPartAccess> result = new ArrayList<EditorPartAccess>();

        for (IWorkbenchPage page : workbenchWindow.getPages()) {
            for (IEditorReference editorReference : page.getEditorReferences()) {
                log(debug, "  checking editorpart: " + editorReference.getTitle());
                if (predicate.matches(editorReference))
                    result.add((EditorPartAccess)createAccess(editorReference.getEditor(restore)));
            }
        }

        return result;
    }

	@UIStep
	public void closeAllEditorPartsWithHotKey() {
		pressKey('w', SWT.CONTROL + SWT.SHIFT);
	}

    @SuppressWarnings("unused")
    public void ensureAllEditorPartsAreClosed() {
        for (IWorkbenchPage page : workbenchWindow.getPages()) {
            for (IEditorReference editorReference : page.getEditorReferences()) {
                closeAllEditorPartsWithHotKey();
                return;
            }
        }
    }

    @UIStep
    public void assertNoOpenEditorParts() {
        for (IWorkbenchPage page : workbenchWindow.getPages()) {
            for (IEditorReference editorReference : page.getEditorReferences())
                Assert.fail("Editor part " + editorReference.getTitle() + " still open");
        }
    }

    @UIStep
    public void saveAllEditorPartsWithHotKey() {
        pressKey('s', SWT.CONTROL + SWT.SHIFT);
    }

    public void ensureAllEditorPartsAreSaved() {
        for (IWorkbenchPage page : workbenchWindow.getPages()) {
            for (IEditorReference editorReference : page.getEditorReferences()) {
                IEditorPart editorPart = editorReference.getEditor(false);
                if (editorPart != null && editorPart.isDirty()) {
                    saveAllEditorPartsWithHotKey();
                    return;
                }
            }
        }
    }

    @UIStep
    public void assertNoDirtyEditorParts() {
        for (IWorkbenchPage page : workbenchWindow.getPages()) {
            for (IEditorReference editorReference : page.getEditorReferences()) {
                IEditorPart editorPart = editorReference.getEditor(false);
                Assert.assertTrue("Editor part " + editorReference.getTitle() + " still dirty", editorPart == null || !editorPart.isDirty());
            }
        }
    }
}
