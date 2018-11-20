/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Callable;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.IPath;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.edit.ui.dnd.LocalTransfer;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.util.LocalSelectionTransfer;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.DropTargetAdapter;
import org.eclipse.swt.dnd.DropTargetEvent;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.part.FileEditorInput;
import org.omnetpp.common.Debug;
import org.omnetpp.common.ui.FocusManager;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.scave.actions.CollapseSubtreeAction;
import org.omnetpp.scave.actions.CollapseTreeAction;
import org.omnetpp.scave.actions.ExpandSubtreeAction;
import org.omnetpp.scave.actions.NewInputFileAction;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ScaveEditorContributor;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engineext.IResultFilesChangeListener;
import org.omnetpp.scave.engineext.ResultFileManagerChangeEvent;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave.model2.ScaveModelUtil;

//TODO remove page description; display "empty-table message" instead (stacklayout?)
//TODO under each Run, display: "12 vectors", "35 scalars", etc.
public class InputsPage extends FormEditorPage {
    private InputsTree treeViewer;

    private Runnable scheduledUpdate;

    public InputsPage(Composite parent, ScaveEditor scaveEditor) {
        super(parent, SWT.NONE, scaveEditor);
        initialize();
    }

    private void initialize() {
        // set up UI
        setPageTitle("Inputs");
        setFormTitle("Inputs");
        getContent().setLayout(new GridLayout());

        Label label = new Label(getContent(), SWT.WRAP);
        label.setText("Add or drag'n'drop result files (sca,vec) or folders that serve as input to the analysis. Wildcards (*,?) are accepted.");
        label.setBackground(this.getBackground());
        label.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false));

        IContainer baseFolder = ((FileEditorInput)scaveEditor.getEditorInput()).getFile().getParent();
        treeViewer = new InputsTree(getContent(), scaveEditor.getAnalysis(), scaveEditor.getResultFileManager(), baseFolder, true);
        treeViewer.getControl().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        // toolbar
        ScaveEditorContributor contributor = ScaveEditorContributor.getDefault();
        addToToolbar(new NewInputFileAction());
        addToToolbar(contributor.getEditInputFileAction());
        addToToolbar(contributor.getRemoveAction());
        addSeparatorToToolbar();
        addToToolbar(new CollapseTreeAction(treeViewer));

        treeViewer.getTree().setFocus();

        //TODO treeViewer.addSelectionChangedListener(selectionChangedListener);

        // context menu
        UIUtils.createContextMenuFor(treeViewer.getTree(), true, new IMenuListener() {
            @Override
            public void menuAboutToShow(IMenuManager menuManager) {
                menuManager.add(new NewInputFileAction());
                menuManager.add(contributor.getEditInputFileAction());
                menuManager.add(contributor.getRemoveAction());
                menuManager.add(new Separator());
                menuManager.add(contributor.getUndoRetargetAction());
                menuManager.add(new Separator());
                menuManager.add(new ExpandSubtreeAction(treeViewer));
                menuManager.add(new CollapseSubtreeAction(treeViewer));
                // TODO needed? scaveEditor.menuAboutToShow(menuManager);
            }
        });

        // expand/collapse on double-click
        treeViewer.getTree().addSelectionListener(new SelectionAdapter() {
            public void widgetDefaultSelected(SelectionEvent e) {
                Object element = treeViewer.getStructuredSelection().getFirstElement();
                if (element != null) {
                    if (treeViewer.getExpandedState(element))
                        treeViewer.collapseToLevel(element, 1);
                    else
                        treeViewer.expandToLevel(element, 1);
                }
            }
        });

        // set up drag & drop of .sca and .vec files into the viewers
        int dndOperations = DND.DROP_COPY | DND.DROP_MOVE | DND.DROP_LINK;
        Transfer[] transfers = new Transfer[] { LocalTransfer.getInstance(), LocalSelectionTransfer.getTransfer() }; // note: FileTransfer causes exception
        treeViewer.addDropSupport(dndOperations, transfers, new DropTargetAdapter() {
            @Override
            public void drop(DropTargetEvent event) {
                if (event.data instanceof IStructuredSelection)
                    handleDrop((IStructuredSelection)event.data);
            }
        });

        // ensure that focus gets restored correctly after user goes somewhere else and then comes back
        setFocusManager(new FocusManager(this));

        // refresh on workspace changes
        scaveEditor.getResultFileManager().addChangeListener(new IResultFilesChangeListener() {
            @Override
            public void resultFileManagerChanged(ResultFileManagerChangeEvent event) {
                switch (event.getChangeType()) {
                case LOAD:
                case UNLOAD:
                    final ResultFileManager manager = event.getResultFileManager();
                    if (scheduledUpdate == null) {
                        scheduledUpdate = new Runnable() {
                            @Override
                            public void run() {
                                scheduledUpdate = null;
                                if (!isDisposed()) {
                                    ResultFileManager.callWithReadLock(manager, new Callable<Object>() {
                                        @Override
                                        public Object call()  {
                                            getTreeViewer().refresh();
                                            return null;
                                        }
                                    });
                                }
                            }
                        };
                        getDisplay().asyncExec(scheduledUpdate);
                    }
                default: // nothing
                }
            }
        });

        getTreeViewer().addSelectionChangedListener(scaveEditor.getSelectionChangedListener());
    }

    public InputsTree getTreeViewer() {
        return treeViewer;
    }

    @Override
    public boolean gotoObject(Object object) {
        if (object instanceof InputFile) {
            getTreeViewer().reveal(object);
            return true;
        }
        else if (object instanceof Inputs) {
            return true;
        }

        return false;
    }

    @Override
    public void updatePage(Notification notification) {
        if (ScaveModelUtil.isInputsChange(notification))
            getTreeViewer().refresh();
    }

    @Override
    public void selectionChanged(ISelection selection) {
        Debug.println(selection.toString());
        //TODO if selection contains InputFiles, select them in the tree?
    }

    protected void handleDrop(IStructuredSelection droppedItems) {
        // extract list of patterns to add
        List<String> list = new ArrayList<>();
        for (Object droppedItem : droppedItems.toList()) {
            if (droppedItem instanceof IAdaptable)
                droppedItem = ((IAdaptable) droppedItem).getAdapter(IResource.class);
            if (droppedItem instanceof IFile) {
                IFile file = (IFile)droppedItem;
                String fileExtension = file.getFileExtension();
                if (fileExtension != null && fileExtension.equals("sca") || fileExtension.equals("vec"))
                    list.add(makeRelative(file.getFullPath()).toString());
            }
            else if (droppedItem instanceof IContainer) {
                IContainer folder = (IContainer)droppedItem;
                list.add(makeRelative(folder.getFullPath().append("*.vec")).toString());
                list.add(makeRelative(folder.getFullPath().append("*.sca")).toString());
            }
        }

        // add them
        ScaveModelUtil.addInputFiles(scaveEditor.getEditingDomain(), scaveEditor.getAnalysis(), list);
    }

    protected IPath makeRelative(IPath path) {
        IPath baseDir = ((FileEditorInput)scaveEditor.getEditorInput()).getFile().getParent().getFullPath();
        return baseDir.isPrefixOf(path) ? path.removeFirstSegments(baseDir.segmentCount()) : path;
    }

}
