package com.simulcraft.test.gui.access;

import java.util.ArrayList;
import java.util.List;

import junit.framework.Assert;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Table;
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

import com.simulcraft.test.gui.core.InUIThread;
import com.simulcraft.test.gui.core.NotInUIThread;


public class WorkbenchWindowAccess extends Access {
	private IWorkbenchWindow workbenchWindow;
	
	public WorkbenchWindowAccess(IWorkbenchWindow workbenchWindow) {
		this.workbenchWindow = workbenchWindow;
	}
	
    @InUIThread
	public ShellAccess getShell() {
		return new ShellAccess(workbenchWindow.getShell());
	}

	@InUIThread
	public MenuAccess getMenuBar() {
		return getShell().getMenuBar();
	}
	
    @InUIThread
    public IPerspectiveDescriptor getPerspective() {
        return workbenchWindow.getWorkbench().getActiveWorkbenchWindow().getActivePage().getPerspective();
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
		return new EditorPartAccess(workbenchWindow.getPages()[0].getActiveEditor());
	}

	@InUIThread
	public WorkbenchPartAccess getActivePart() {
		IWorkbenchPart activePart = workbenchWindow.getPages()[0].getActivePart();
		if (activePart instanceof IEditorPart)
			return new EditorPartAccess((IEditorPart)activePart);
		else 
			return new ViewPartAccess((IViewPart)activePart);
	}

	@InUIThread
	public ViewPartAccess showViewPart(String viewId) throws PartInitException {
		return new ViewPartAccess(workbenchWindow.getPages()[0].showView(viewId));
	}

	@InUIThread
	public ViewPartAccess findViewPartByTitle(String title) {
		return findViewPartByTitle(title, true);
	}

	@InUIThread
	public ViewPartAccess findViewPartByTitle(String title, boolean restore) {
		return (ViewPartAccess)theOnlyObject(collectViewPartsByTitle(title, restore));
	}

	@InUIThread
    public List<ViewPartAccess> collectViewPartsByTitle(final String title, boolean restore) {
        return collectViewParts(new IPredicate() {
            public boolean matches(Object object) {
                return ((IViewReference)object).getPartName().matches(title);
            }
        }, restore);
    }

    @InUIThread
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

	@InUIThread
	public EditorPartAccess findEditorPartByTitle(String title) {
		return findEditorPartByTitle(title, true);
	}
	
	@InUIThread
	public MultiPageEditorPartAccess findMultiPageEditorPartByTitle(String title) {
		return (MultiPageEditorPartAccess)findEditorPartByTitle(title, true);
	}
	
	@InUIThread
	public EditorPartAccess findEditorPartByTitle(final String title, boolean restore) {
		return (EditorPartAccess)theOnlyObject(collectEditorPartsByTitle(title, restore));
	}

	@InUIThread
    public boolean hasEditorPartWithTitle(String title) {
	    return hasEditorPartByTitle(title, false);
	}

    @InUIThread
    public boolean hasEditorPartByTitle(String title, boolean restore) {
        return collectEditorPartsByTitle(title, restore).size() == 1;
    }

    @InUIThread
    public List<EditorPartAccess> collectEditorPartsByTitle(final String title, boolean restore) {
        return collectEditorParts(new IPredicate() {
            public boolean matches(Object object) {
                return ((IEditorReference)object).getTitle().matches(title);
            }
        }, restore);
    }
	
    @InUIThread
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

	@InUIThread
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

    @InUIThread
    public void assertNoOpenEditorParts() {
        for (IWorkbenchPage page : workbenchWindow.getPages()) {
            for (IEditorReference editorReference : page.getEditorReferences())
                Assert.fail("Editor part " + editorReference.getTitle() + " still open");
        }
    }

    @InUIThread
    public static TableAccess findContentAssistPopup() {
        // Content Assist popup is a Table in a Composite in a nameless Shell
        Shell shell = (Shell)Access.findObject(Access.getDisplay().getShells(), new IPredicate() {
            public boolean matches(Object object) {
                Access.log(Access.debug, "Trying to content assist popup");
                Shell shell = (Shell)object;
                if (shell.getText().equals("")) {
                    //shell.getStyle() == 0x020004F0
                    if (shell.getChildren().length==1 && shell.getChildren()[0] instanceof Composite) {
                        Composite composite = (Composite)shell.getChildren()[0];
                        if (composite.getChildren().length==1 && composite.getChildren()[0] instanceof Table) {
                            Table table = (Table)composite.getChildren()[0];
                            if (table.getColumnCount() == 0) 
                                return true;
                        }
                    }
                }
                return false;
            }
        });
        return new TableAccess((Table) ((Composite)shell.getChildren()[0]).getChildren()[0] );
    }
    
    @NotInUIThread
    public static void chooseFromContentAssistPopupWithMouse(String label) {
        WorkbenchWindowAccess.findContentAssistPopup().findTableItemByContent(label).reveal().doubleClick();
    }

    @NotInUIThread
    public static void chooseFromContentAssistPopupWithKeyboard(String label) {
        TableAccess table = WorkbenchWindowAccess.findContentAssistPopup();
        TableItemAccess item = table.findTableItemByContent(label);

        // press Down until we get there (wraps if we reach the bottom)
        while (table.getSelection().length!=1 || table.getSelection()[0].getWidget()!=item.getWidget()) {
            Access.sleep(0.1);
            table.pressKey(SWT.ARROW_DOWN);  // usually not visible in the animation because the text is the focus control!
        }
        table.pressEnter();
    }

}
