package org.omnetpp.ide.installer;

import org.eclipse.swt.events.ControlAdapter;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IPageListener;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.internal.ViewIntroAdapterPart;
import org.eclipse.ui.intro.config.CustomizableIntroPart;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.util.ReflectionUtils;

/**
 * This class makes sure that the FirstStepsDialog is automatically
 * open as soon as the user closes or minimizes the initial welcome page of the IDE.
 * After the dialog has been open once, it's never open again automatically.
 *
 * @author levy
 */
@SuppressWarnings("restriction")
public class AutomaticFirstStepsDialogOpener implements IPageListener, IPartListener {
    protected static final String HAS_BEEN_ALREADY_OPEN = "FirstStepsDialogHasBeenAlreadyOpen";

    // IPageListener interface

    @Override
    public void pageActivated(IWorkbenchPage page) {
    }

    @Override
    public void pageClosed(IWorkbenchPage page) {
        page.removePartListener(this);
    }

    @Override
    public void pageOpened(IWorkbenchPage page) {
        page.addPartListener(this);
    }

    // IPartListener interface

    @Override
    public void partActivated(IWorkbenchPart part) {
    }

    @Override
    public void partBroughtToTop(IWorkbenchPart part) {
    }

    @Override
    public void partClosed(final IWorkbenchPart part) {
        if (part instanceof ViewIntroAdapterPart) {
            openFirstStepsDialog(part);
            removeResizeListener(part);
        }
    }

    @Override
    public void partDeactivated(IWorkbenchPart part) {
    }

    @Override
    public void partOpened(IWorkbenchPart part) {
        if (part instanceof ViewIntroAdapterPart)
            addResizeListener(part);
    }

    // hook/unhook

    public void hook() {
        IWorkbench workbench = PlatformUI.getWorkbench();
        IWorkbenchWindow activeWorkbenchWindow = workbench.getActiveWorkbenchWindow();
        if (activeWorkbenchWindow != null) {
            activeWorkbenchWindow.addPageListener(this);
            for (IWorkbenchPage page : activeWorkbenchWindow.getPages()) {
                page.addPartListener(this);
                IWorkbenchPart activePart = page.getActivePart();
                if (activePart instanceof ViewIntroAdapterPart)
                    this.addResizeListener(activePart);
            }
        }
    }

    public void unhook() {
        IWorkbench workbench = PlatformUI.getWorkbench();
        IWorkbenchWindow activeWorkbenchWindow = workbench.getActiveWorkbenchWindow();
        if (activeWorkbenchWindow != null) {
            activeWorkbenchWindow.removePageListener(this);
            for (IWorkbenchPage page : activeWorkbenchWindow.getPages()) {
                page.removePartListener(this);
                IWorkbenchPart activePart = page.getActivePart();
                if (activePart instanceof ViewIntroAdapterPart)
                    this.removeResizeListener(activePart);
            }
        }
    }

    protected void addResizeListener(final IWorkbenchPart part) {
        final CustomizableIntroPart introPart = (CustomizableIntroPart)ReflectionUtils.getFieldValue(part, "introPart");
        introPart.getControl().addControlListener(new ControlAdapter() {
            @Override
            public void controlResized(ControlEvent e) {
                openFirstStepsDialog(part);
            }
        });
    }

    protected void removeResizeListener(IWorkbenchPart part) {
        // TODO:
    }

    protected boolean hasBeenAlreadyOpen() {
        return CommonPlugin.getDefault().getDialogSettings().getBoolean(HAS_BEEN_ALREADY_OPEN);
    }

    protected void setHasBeenAlreadyOpen() {
        CommonPlugin.getDefault().getDialogSettings().put(HAS_BEEN_ALREADY_OPEN, true);
    }

    protected void openFirstStepsDialog(final IWorkbenchPart part) {
        Display.getCurrent().asyncExec(new Runnable() {
            @Override
            public void run() {
                IWorkbenchPage page = part.getSite().getPage();
                int partState = page.getPartState(page.getReference(part));
                if (!hasBeenAlreadyOpen() && partState != IWorkbenchPage.STATE_MAXIMIZED && !PlatformUI.getWorkbench().isClosing()) {
                    setHasBeenAlreadyOpen();
                    new FirstStepsDialog(null).open();
                    unhook();
                }
            }
        });
    }
}