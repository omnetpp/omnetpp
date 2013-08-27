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
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.ide.OmnetppMainPlugin;

/**
 * This class runs the given runnable as soon as the user closes or minimizes 
 * the initial welcome page of the IDE.
 *
 * @author levy
 */
@SuppressWarnings("restriction")
public class OnClosingWelcomeView implements IPageListener, IPartListener {
    protected Runnable runnable;

    /**
     * Remember the runnable to be run as soon as the user closes or minimizes 
     * the initial welcome page of the IDE.
     */
    public OnClosingWelcomeView(Runnable runnable) {
        this.runnable = runnable;
    }

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
            introPartResized(part);
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
                introPartResized(part);
            }
        });
    }

    protected void removeResizeListener(IWorkbenchPart part) {
        // TODO:
    }

    protected void introPartResized(final IWorkbenchPart part) {
        Display.getCurrent().asyncExec(new Runnable() {
            @Override
            public void run() {
                IWorkbenchPage page = part.getSite().getPage();
                int partState = page.getPartState(page.getReference(part));
                if (partState != IWorkbenchPage.STATE_MAXIMIZED && !PlatformUI.getWorkbench().isClosing()) {
                    try {
                        runnable.run();
                    }
                    catch (Exception e) {
                        OmnetppMainPlugin.logError(e);
                    }
                    finally {
                        unhook();
                    }
                }
            }
        });
    }
}