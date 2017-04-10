/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.window.ApplicationWindow;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.DropTarget;
import org.eclipse.swt.dnd.DropTargetAdapter;
import org.eclipse.swt.dnd.DropTargetEvent;
import org.eclipse.swt.dnd.FileTransfer;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.IWorkbenchWindow;
import org.omnetpp.common.ui.FocusManager;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.IScaveAction;
import org.omnetpp.scave.charting.ChartCanvas;
import org.omnetpp.scave.editors.ScaveEditor;

/**
 * Common functionality of Scave multi-page editor pages.
 * @author andras
 */
public class ScaveEditorPage extends Composite {
    //private static final Color PALETTE_BG_COLOR = new Color(null, 234, 240, 252);
    //private static final Color BUTTONS_BG_COLOR = new Color(null, 239, 244, 253);
    private static final Color PALETTE_BG_COLOR = new Color(null, 241, 245, 253);
    private static final Color BUTTONS_BG_COLOR = new Color(null, 249, 251, 254);

    protected ScaveEditor scaveEditor = null;  // backreference to the containing editor

    private Label formTitle;
    private Composite content;
    private String pageTitle = "untitled";

    protected FocusManager focusManager = null;

    public ScaveEditorPage(Composite parent, int style, ScaveEditor scaveEditor) {
        super(parent, style);
        setLayout(new GridLayout());
        formTitle = new Label(this, SWT.NONE);
        content = new Composite(this, SWT.NONE);
        content.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        this.scaveEditor = scaveEditor;
    }

    public Label getHeader()
    {
        return formTitle;
    }

    public Composite getContent()
    {
        return content;
    }

    public void setFormTitle(String title) {
        formTitle.setFont(new Font(null, "Arial", 12, SWT.BOLD));
        formTitle.setForeground(new Color(null, 0, 128, 255));
        formTitle.setText(title);
    }

    /**
     * Return the text that should appear on the page's tab.
     */
    public String getPageTitle() {
        return pageTitle;
    }

    /**
     * Sets the text that should appear on the page's tab.
     */
    public void setPageTitle(String title) {
        pageTitle = title;
        scaveEditor.setPageTitle(this, title);
    }

    public FocusManager getFocusManager() {
        return focusManager;
    }

    public void setFocusManager(FocusManager focusManager) {
        this.focusManager = focusManager;
    }

    @Override
    public boolean setFocus() {
        // try to restore focus where it was last time
        if (getFocusManager() != null && getFocusManager().setFocus())
            return true;
        return super.setFocus();
    }

    /**
     * Updates the page title, etc.
     * Called when the model changed.
     */
    public void updatePage(Notification notification) {
    }

    /**
     * Creates palette for model object creation
     */
    protected Composite createPalette(Composite parent) {
        ScrolledComposite scrolledComposite = new ScrolledComposite(parent, SWT.BORDER | SWT.V_SCROLL);
        scrolledComposite.setLayoutData(new GridData(SWT.END, SWT.TOP, false, true));
        scrolledComposite.setBackground(PALETTE_BG_COLOR);
        Composite toolbar = new Composite(scrolledComposite, SWT.NONE);
        scrolledComposite.setContent(toolbar);
        toolbar.setLayout(new RowLayout(SWT.VERTICAL));
        ((RowLayout)toolbar.getLayout()).fill = true;
        toolbar.setBackground(PALETTE_BG_COLOR);
        new ModelObjectPalette(toolbar, BUTTONS_BG_COLOR, true, scaveEditor);
        toolbar.setSize(toolbar.computeSize(SWT.DEFAULT, SWT.DEFAULT));
        return scrolledComposite;
    }


    /**
     * Sets up the given control so that when a file is drag-dropped into it,
     * it will be added to Inputs unless it's already in there.
     */
    public void setupResultFileDropTarget(Control dropTargetControl) {
        // set up DropTarget, and add FileTransfer to it;
        Assert.isTrue(dropTargetControl.getData("DropTarget") == null);
        DropTarget dropTarget = new DropTarget(dropTargetControl, DND.DROP_DEFAULT | DND.DROP_COPY);
        dropTarget.setTransfer(new Transfer[] {FileTransfer.getInstance()});

        // add our listener to handle file transfer to the DropTarget
        dropTarget.addDropListener(new DropTargetAdapter() {
            @Override
            public void dragEnter(DropTargetEvent event) {
                if (event.detail == DND.DROP_DEFAULT)
                    event.detail = DND.DROP_COPY;
            }
            @Override
            public void dragOperationChanged(DropTargetEvent event) {
                if (event.detail == DND.DROP_DEFAULT)
                    event.detail = DND.DROP_COPY;
            }
            @Override
            public void drop(DropTargetEvent event) {
                if (event.data instanceof String[]) {
                    String [] fileNames = (String[])event.data;
                    for (int i=0; i<fileNames.length; i++)
                        addDroppedFileToInputs(fileNames[i]);
                }
            }
        });
    }

    /**
     * Adds the given file to Inputs unless it's already in there.
     * The file name should be an OS path (D:\... or /home/you/...),
     * not a workspace path!
     *
     * XXX Limitation: currently the file must be available via the
     * workspace as well. This may get improved in the future.
     */
    private void addDroppedFileToInputs(String fileName) {
        // convert OS path to workspace path
        IFile iFile = ScaveEditor.findFileInWorkspace(fileName);
        if (iFile==null) {
            IStatus error =
                ScavePlugin.getErrorStatus(0, "Path not found in the workspace" + fileName, null);
            ErrorDialog.openError(getShell(), null, null, error);
            return;
        }
        scaveEditor.addWorkspaceFileToInputs(iFile);
    }

    /**
     * Connects the button with an action, so that the action is executed
     * when the button is pressed, and the button is enabled/disabled when
     * the action becomes enabled/disabled.
     *
     * Note: ActionContributionItem is not good here because:
     *  (1) it wants to create the button itself, and thus not compatible with FormToolkit
     *  (2) the button it creates has wrong background color, and there's no way to access
     *      the button to fix it (this is solved by ActionContributionItem2, it adds a getter)
     */
    private static void doConfigureButton(final Button button, final IScaveAction action) {
        button.setText(action.getText());
        button.setToolTipText(action.getToolTipText());
        //if (action.getImageDescriptor() != null)
        //    button.setImage(action.getImageDescriptor().createImage());

        button.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                action.run();
            }
        });
        final IPropertyChangeListener propertyChangeListener = new IPropertyChangeListener() {
            @Override
            public void propertyChange(PropertyChangeEvent event) {
                if (event.getProperty().equals(IAction.ENABLED)) {
                    if (!button.isDisposed())
                        button.setEnabled(action.isEnabled());
                }
            }
        };
        action.addPropertyChangeListener(propertyChangeListener);
        button.addDisposeListener(new DisposeListener() {
            @Override
            public void widgetDisposed(DisposeEvent e) {
                action.removePropertyChangeListener(propertyChangeListener);
            }
        });
    }

    /*
     * Connects the button with an action, so that the action is executed
     * when the button is pressed, and the button is enabled/disabled when
     * the action becomes enabled/disabled.
     *
     * The action will be enabled/disabled based on a viewer's selection.
     *
     * See also: ActionContributionItem, ActionContributionItem2
     */
    public static void configureViewerButton(final Button button, final Viewer viewer, final IScaveAction action) {
        doConfigureButton(button, action);
        action.setViewer(viewer);
        viewer.addSelectionChangedListener(new ISelectionChangedListener() {
            @Override
            public void selectionChanged(SelectionChangedEvent event) {
                action.selectionChanged(event.getSelection());
            }
        });
    }

    /**
     * Like <code>configureViewerButton</code>, but action will also run
     * on double-clicking in the viewer.
     */
    public static void configureViewerDefaultButton(final Button button, final TreeViewer viewer, final IScaveAction action) {
        configureViewerButton(button, viewer, action);
        configureViewerDefaultAction(viewer, action);
    }

    /**
     * Connects the double-click in the viewer with the action.
     */
    public static void configureViewerDefaultAction(final TreeViewer viewer, final IScaveAction action) {
        action.setViewer(viewer);
        viewer.getTree().addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetDefaultSelected(SelectionEvent e) {
                action.run();
            }
        });
    }

    public void showStatusMessage(String message) {
        IWorkbenchWindow window = scaveEditor.getSite().getWorkbenchWindow();
        if (window instanceof ApplicationWindow)
            ((ApplicationWindow)window).setStatus(message);
    }

    /**
     * Notification about the activation of the page of the
     * multipage editor.
     */
    public void pageActivated() {
    }

    /**
     * Notification about the change of the workbench selection.
     * Pages are synchronizing their selection of their viewers
     * by calling {@link #setViewerSelectionNoNotify(Viewer, ISelection)}.
     */
    public void selectionChanged(ISelection selection) {
    }

    protected void setViewerSelectionNoNotify(Viewer viewer, ISelection selection) {
        scaveEditor.setViewerSelectionNoNotify(viewer, selection);
    }

    /**
     * Returns the active chart on this page.
     */
    public ChartCanvas getActiveChartCanvas() {
        return null;
    }

    public boolean gotoObject(Object object) {
        return false;
    }

    /**
     * Saves the state of the page before the editor closed.
     * Override this method when the page has some persistable state.
     */
    public void saveState(IMemento memento) {
    }

    /**
     * Restores the state of the page from the saved state.
     * Override this method when the page has some persistable state.
     */
    public void restoreState(IMemento memento) {
    }
}
