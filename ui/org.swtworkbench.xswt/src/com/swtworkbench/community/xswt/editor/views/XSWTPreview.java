package com.swtworkbench.community.xswt.editor.views;

import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.PrintWriter;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.ViewPart;

import com.swtworkbench.community.xswt.XSWT;
import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.metalogger.Logger;

public class XSWTPreview extends ViewPart {
    public static final String VIEW_ID = "com.swtworkbench.community.xswt.editor.views.XSWTPreview";

    public XSWTPreview() {
    }

    public void dispose() {
        getSite().getWorkbenchWindow().getPartService().removePartListener(partListener);
        ResourcesPlugin.getWorkspace().removeResourceChangeListener(resourceChangeListener);
    }

    private static boolean equals(Object left, Object right) {
        return left != null ? left.equals(right) : right == null;
    }

    private void refresh(boolean force) {
        IWorkbenchWindow workbenchWindow = getViewSite().getWorkbenchWindow();
        if (workbenchWindow == null)
            return;
        IWorkbenchPage workbenchPage = workbenchWindow.getActivePage();
        if (workbenchPage == null)
            return;
        IEditorPart editorPart = workbenchPage.getActiveEditor();
        Logger.log().debug(com.swtworkbench.community.xswt.editor.views.XSWTPreview.class, "Refreshing...");
        if (force || !equals(editorPart, lastEditorPart)) {
            if (editorPart != null && "com.swtworkbench.community.xswt.editors.XSWTEditor".equals(editorPart.getSite().getId())) {
                final IFile inputFile = (IFile) editorPart.getEditorInput().getAdapter(org.eclipse.core.resources.IFile.class);
                Display.getDefault().asyncExec(new Runnable() {

                    public void run() {
                        try {
                            refresh(inputFile.getContents());
                        }
                        catch (CoreException eCore) {
                            Logger.log().error(eCore, "Unable to refresh");
                        }
                    }

                });
            }
            else {
                refresh(((InputStream) (null)));
            }
            lastEditorPart = editorPart;
        }
    }

    private void disposeChildren() {
        Display.getDefault().syncExec(new Runnable() {
            public void run() {
                if (compositeMain != null) {
                    Control children[] = compositeMain.getChildren();
                    for (int i = 0; i < children.length; i++)
                        children[i].dispose();
                }
            }
        });
    }

    private void refresh(InputStream inputStream) {
        disposeChildren();
        if (inputStream != null)
            try {
                XSWT.create(compositeMain, inputStream);
            }
            catch (XSWTException e) {
                disposeChildren();
                Text errMsg = new Text(compositeMain, SWT.READ_ONLY|SWT.MULTI|SWT.WRAP);
                ByteArrayOutputStream s = new ByteArrayOutputStream();
                e.printStackTrace(new PrintWriter(s));
                String stackdump = e.toString();
                errMsg.setText(stackdump);
            }
        compositeMain.layout();
    }

    public void createPartControl(Composite parent) {
        // in the original XSWT code, there were two SashForms here, to allow resizing the
        // composite. However, sashes were not easily discoverable by the user, and also awkward
        // because the sashes could not be minimized again (see hardcoded DRAG_MINIMUM=20 in SashForm),
        // and also the whole thing seems unnecessary because the view itself can be resized as well,
        // so I removed the sashes. --Andras
        this.parent = parent;
        getSite().getWorkbenchWindow().getPartService().addPartListener(partListener);
        IWorkspace workspace = ResourcesPlugin.getWorkspace();
        workspace.addResourceChangeListener(resourceChangeListener, 1);

        Composite compositeFrame0 = new Composite(parent, SWT.NONE);
        GridLayout gridLayout = new GridLayout();
        gridLayout.marginWidth = 0;
        gridLayout.marginHeight = 0;
        compositeFrame0.setLayout(gridLayout);

        Composite compositeFrame = new Composite(compositeFrame0, SWT.NONE);
        compositeFrame.setBackground(new Color(compositeFrame.getDisplay(), 100, 200, 120)); // green frame
        gridLayout = new GridLayout();
        gridLayout.marginWidth = 2;
        gridLayout.marginHeight = 2;
        compositeFrame.setLayout(gridLayout);
        compositeFrame.setLayoutData(new GridData(1808));

        compositeMain = new Composite(compositeFrame, SWT.NONE);
        compositeMain.setLayout(new GridLayout());
        compositeMain.setLayoutData(new GridData(1808));
    }

    private void contributeToActionBars() {
        IActionBars bars = getViewSite().getActionBars();
        fillLocalPullDown(bars.getMenuManager());
        fillLocalToolBar(bars.getToolBarManager());
    }

    private void fillLocalPullDown(IMenuManager manager) {
        manager.add(actionPack);
    }

    private void fillLocalToolBar(IToolBarManager manager) {
        manager.add(actionPack);
    }

    private void makeActions() {
        actionPack = new Action() {
            public void run() {
                compositeMain.getParent().pack();
            }
        };
        actionPack.setText("Pack");
        actionPack.setToolTipText("Pack");
        actionPack.setImageDescriptor(PlatformUI.getWorkbench().getSharedImages().getImageDescriptor("IMG_OBJS_INFO_TSK"));
    }

    public void setFocus() {
        if (compositeMain != null)
            compositeMain.setFocus();
        else
            parent.setFocus();
    }

    private Action actionPack;
    private Composite compositeMain;
    private IEditorPart lastEditorPart;
    private Composite parent;

    private final IPartListener partListener = new IPartListener() {
        public void partActivated(IWorkbenchPart workbenchPart) {
            Logger.log().debug(getClass(), "PartActivated");
            refresh(false);
            return;
        }

        public void partBroughtToTop(IWorkbenchPart workbenchPart) {
            Logger.log().debug(getClass(), "PartBroughtToTop");
            refresh(false);
            return;
        }

        public void partClosed(IWorkbenchPart workbenchPart) {
            Logger.log().debug(getClass(), "partClosed");
            refresh(false);
            return;
        }

        public void partDeactivated(IWorkbenchPart workbenchPart) {
            Logger.log().debug(getClass(), "PartDeactivated");
            refresh(false);
            return;
        }

        public void partOpened(IWorkbenchPart workbenchPart) {
            Logger.log().debug(getClass(), "partOpened");
            refresh(true);
            return;
        }
    };

    private final IResourceChangeListener resourceChangeListener = new IResourceChangeListener() {
        public void resourceChanged(IResourceChangeEvent event) {
            Logger.log().debug(getClass(), "resourceChanged");
            Display.getDefault().asyncExec(new Runnable() {
                public void run() {
                    refresh(true);
                }
            });
            return;
        }
    };

}
