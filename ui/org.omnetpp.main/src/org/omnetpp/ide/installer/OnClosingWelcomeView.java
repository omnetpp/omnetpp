package org.omnetpp.ide.installer;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ControlAdapter;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.ControlListener;
import org.eclipse.swt.internal.SWTEventListener;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.TypedListener;
import org.eclipse.ui.IPageListener;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IViewReference;
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
    protected boolean hasBeenRun;
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
        if (part instanceof ViewIntroAdapterPart)
            onCloseOrResize(part, true);
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
                for (IViewReference viewReference : page.getViewReferences()) {
                    IViewPart viewPart = viewReference.getView(false);
                    if (viewPart instanceof ViewIntroAdapterPart)
                        addResizeListener(viewPart);
                }
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
                for (IViewReference viewReference : page.getViewReferences()) {
                    IViewPart viewPart = viewReference.getView(false);
                    if (viewPart instanceof ViewIntroAdapterPart)
                        removeResizeListener(viewPart);
                }
            }
        }
    }

    protected void addResizeListener(final IWorkbenchPart part) {
        CustomizableIntroPart introPart = (CustomizableIntroPart)ReflectionUtils.getFieldValue(part, "introPart");
        introPart.getControl().addControlListener(new ControlAdapter() {
            @Override
            public void controlResized(ControlEvent e) {
                onCloseOrResize(part, false);
            }
        });
    }

    protected void removeResizeListener(IWorkbenchPart part) {
        CustomizableIntroPart introPart = (CustomizableIntroPart)ReflectionUtils.getFieldValue(part, "introPart");
        Control control = introPart.getControl();
        for (Listener listener : control.getListeners(SWT.Resize)) {
            if (listener instanceof TypedListener) {
                TypedListener typedListener = (TypedListener)listener;
                SWTEventListener eventListener = typedListener.getEventListener();
                if (eventListener instanceof ControlListener)
                    control.removeControlListener((ControlListener)eventListener);
            }
        }
    }

    protected void onCloseOrResize(final IWorkbenchPart part, final boolean closed) {
        Display.getCurrent().syncExec(new Runnable() {
            @Override
            public void run() {
                IWorkbenchPage page = part.getSite().getPage();
                int partState = page == null ? IWorkbenchPage.STATE_MAXIMIZED : page.getPartState(page.getReference(part));
                // KLUDGE: we need to delay this check because the part state will change only after the control resize event is handled
                if (!hasBeenRun && (closed || partState != IWorkbenchPage.STATE_MAXIMIZED) && !PlatformUI.getWorkbench().isClosing()) {
                    try {
                        // have to set the flag first because the runnable might open a dialog that runs a new event loop
                        hasBeenRun = true;
                        runnable.run();
                    }
                    catch (Exception e) {
                        OmnetppMainPlugin.logError(e);
                    }
                    finally {
                        // have to delay unhook this late to be sure that the runnable is actually called once
                        unhook();
                    }
                }
            }
        });
    }
}