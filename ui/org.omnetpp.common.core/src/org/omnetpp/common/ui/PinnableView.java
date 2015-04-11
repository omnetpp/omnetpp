/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.common.CommonCorePlugin;
import org.omnetpp.common.util.ActionExt;
import org.omnetpp.common.util.DelayedJob;

/**
 * Abstract base class for views that normally show information that
 * belongs to the active editor's selection, but can be pinned to
 * a given editor. Subclasses are expected to implement the doBuildContent()
 * method, which will be invoked whenever the selection changes.
 *
 * @author Andras
 */
//XXX ne csak editorhoz hanem viewhoz is
//XXX ne legyen delay
public abstract class PinnableView extends ViewWithMessagePart {
    private IEditorPart pinnedToEditor = null;
    private ISelection pinnedToEditorSelection = null;
    private IAction pinAction;

    private ISelectionListener selectionChangedListener;
    private IPartListener partListener;
    private DelayedJob rebuildContentJob = new DelayedJob(200) {
        public void run() {
            if (!PinnableView.this.isDisposed())
                rebuildContent();
        }
    };

    public PinnableView() {
    }

    @Override
    public void createPartControl(Composite parent) {
        super.createPartControl(parent);
        hookListeners();
        scheduleRebuildContent();
    }

    @Override
    public void dispose() {
        unhookListeners();
        super.dispose();
    }

    protected void hookListeners() {
        // Listen on selection changes
        selectionChangedListener = new ISelectionListener() {
            public void selectionChanged(IWorkbenchPart part, ISelection selection) {
                if (part != PinnableView.this) // ignore our own selection changes
                    // Debug.println("*** AbstractModule selection changed from: "+part);
                    workbenchSelectionChanged();
            }
        };
        getSite().getPage().addPostSelectionListener(selectionChangedListener);
        getSite().getPage().addSelectionListener(selectionChangedListener);

        // Listens to workbench changes, and invokes activeEditorChanged() whenever the
        // active editor changes. Listening on workbench changes is needed because
        // editors don't always send selection changes when they get opened or closed.
        //
        // NOTE: the view only gets workbench events while it is visible. So we also
        // need to update our contents on getting activated.
        //
        final IEditorPart initialEditor = getActiveEditor();
        partListener = new IPartListener() {
            private IEditorPart activeEditor = initialEditor;

            public void partActivated(IWorkbenchPart part) {
                if (part == PinnableView.this) {
                    viewActivated();
                }
                if (part instanceof IEditorPart) {
                    activeEditor = (IEditorPart) part;
                    activeEditorChanged();
                }
            }

            public void partBroughtToTop(IWorkbenchPart part) {
            }

            public void partClosed(IWorkbenchPart part) {
                if (part == pinnedToEditor) {
                    // unpin on closing of the editor we're pinned to
                    unpin();
                    activeEditorChanged();
                }
                if (part == activeEditor) {
                    activeEditor = null;
                    activeEditorChanged();
                }
            }

            public void partDeactivated(IWorkbenchPart part) {
            }

            public void partOpened(IWorkbenchPart part) {
            }
        };
        getSite().getPage().addPartListener(partListener);
    }

    protected void unhookListeners() {
        if (selectionChangedListener != null)
            getSite().getPage().removePostSelectionListener(selectionChangedListener);
        if (selectionChangedListener != null)
            getSite().getPage().removeSelectionListener(selectionChangedListener);
        if (partListener != null)
            getSite().getPage().removePartListener(partListener);
    }

    public void workbenchSelectionChanged() {
        scheduleRebuildContent();
    }

    protected void viewActivated() {
        //Debug.println("*** VIEW ACTIVATED");
        scheduleRebuildContent();
    }

    protected void activeEditorChanged() {
        //Debug.println("*** ACTIVE EDITOR CHANGED");
        scheduleRebuildContent();
    }

    public void scheduleRebuildContent() {
        rebuildContentJob.restartTimer();
    }

    protected IAction getOrCreatePinAction() {
        if (pinAction == null) {
            pinAction = new ActionExt("Pin", IAction.AS_CHECK_BOX, CommonCorePlugin.getImageDescriptor("icons/elcl16/pin.png")) {
                @Override
                public void run() {
                    if (isChecked())
                        pin();
                    else
                        unpin();
                }
            };
        }
        return pinAction;
    }

    /**
     * Returns the editor the view is pinned to, or the active editor if the view is not pinned.
     */
    protected IEditorPart getAssociatedEditor() {
        return pinnedToEditor != null ? pinnedToEditor : getActiveEditor();
    }

    /**
     * Returns the pinned selection if the view is pinned, or the active editor's selection
     * if the view is not pinned.
     */
    protected ISelection getAssociatedEditorSelection() {
        return pinnedToEditorSelection != null ? pinnedToEditorSelection : getActiveEditorSelection();
    }

    /**
     * Pin the view to the current editor's current selection.
     */
    public void pin() {
        pinnedToEditor = getActiveEditor();
        pinnedToEditorSelection = getActiveEditorSelection();
        pinAction.setChecked(true);
        rebuildContent();  // to update label: "Pinned to: ..."
    }

    /**
     * Unpin the view: let it follow the active editor selection.
     */
    public void unpin() {
        pinnedToEditor = null;
        pinnedToEditorSelection = null;
        pinAction.setChecked(false);
        rebuildContent();
    }

    /**
     * Returns the editor this view is pinned to (see Pin action), or null if view is not pinned.
     */
    public IEditorPart getPinnedToEditor() {
        return pinnedToEditor;
    }

    /**
     * Returns the selection this view is pinned to (see Pin action), or null if view is not pinned.
     */
    public ISelection getPinnedToEditorSelection() {
        return pinnedToEditorSelection;
    }

    /**
     * Rebuild the content according to the selection.
     */
    abstract protected void rebuildContent();
}
