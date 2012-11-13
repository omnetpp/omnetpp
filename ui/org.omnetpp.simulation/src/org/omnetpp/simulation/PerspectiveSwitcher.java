package org.omnetpp.simulation;

import org.eclipse.ui.IEditorReference;
import org.eclipse.ui.IPageListener;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.IPerspectiveDescriptor;
import org.eclipse.ui.IWindowListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.IConstants;
import org.omnetpp.simulation.editors.SimulationEditor;

/**
 * Switches back from the Simulate perspective when the user closes a simulation "editor".
 *
 * @author Andras
 */
public class PerspectiveSwitcher {
    private IPartListener partListener;

    public PerspectiveSwitcher() {
        partListener = new IPartListener() {
            @Override
            public void partOpened(IWorkbenchPart part) {}

            @Override
            public void partClosed(IWorkbenchPart part) {
                workbenchPartClosed(part);
            }
            @Override
            public void partDeactivated(IWorkbenchPart part) {}

            @Override
            public void partBroughtToTop(IWorkbenchPart part) {}

            @Override
            public void partActivated(IWorkbenchPart part) {}
        };

        // add listener everywhere
        for (IWorkbenchWindow window : PlatformUI.getWorkbench().getWorkbenchWindows())
            for (IWorkbenchPage page : window.getPages())
                page.addPartListener(partListener);

        // be prepared for new workbench windows and pages to appear
        PlatformUI.getWorkbench().addWindowListener(new IWindowListener() {
            @Override
            public void windowOpened(IWorkbenchWindow window) {
                for (IWorkbenchPage page : window.getPages())
                    page.addPartListener(partListener);
                window.addPageListener(new IPageListener() {
                    @Override
                    public void pageOpened(IWorkbenchPage page) {
                        // note: apparently this never gets called, because since Eclipse 2.0 there's only
                        // one page per window, and it's created immediately. This would only get called
                        // for additional pages, but there can be only one...
                        page.addPartListener(partListener);
                    }

                    @Override
                    public void pageClosed(IWorkbenchPage page) {
                    }

                    @Override
                    public void pageActivated(IWorkbenchPage page) {
                    }
                });
            }

            @Override
            public void windowDeactivated(IWorkbenchWindow window) {
            }

            @Override
            public void windowClosed(IWorkbenchWindow window) {
            }

            @Override
            public void windowActivated(IWorkbenchWindow window) {
            }
        });
    }

    public void dispose() {
        for (IWorkbenchWindow window : PlatformUI.getWorkbench().getWorkbenchWindows())
            for (IWorkbenchPage page : window.getPages())
                page.removePartListener(partListener);
    }

    protected void workbenchPartClosed(IWorkbenchPart partClosed) {
        // if a simulation editor was closed and there are no more simulations open, restore old perspective
        IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        IWorkbenchPage workbenchPage = workbenchWindow.getActivePage();
        if (workbenchPage != null && (partClosed instanceof SimulationEditor) && !openSimulationEditorExists(workbenchPage)) {
            IPerspectiveDescriptor currentPerspective = workbenchPage.getPerspective();
            if (currentPerspective != null && currentPerspective.getId().equals(IConstants.SIMULATE_PERSPECTIVE_ID)) {
                // user probably doesn't expect to be asked whether to switch back
                IPerspectiveDescriptor originalPerspective = CommonPlugin.getDefault().originalPerspective;
                if (originalPerspective == null)
                    originalPerspective = workbenchWindow.getWorkbench().getPerspectiveRegistry().findPerspectiveWithId(IConstants.OMNETPP_PERSPECTIVE_ID);
                if (originalPerspective != null)
                    workbenchPage.setPerspective(originalPerspective);
            }
        }
    }

    protected boolean openSimulationEditorExists(IWorkbenchPage workbenchPage) {
        for (IEditorReference editorRef : workbenchPage.getEditorReferences())
            if (editorRef.getEditor(false) instanceof SimulationEditor)
                return true;
        return false;
    }
}
