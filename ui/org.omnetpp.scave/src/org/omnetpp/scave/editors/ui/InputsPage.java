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
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.util.LocalSelectionTransfer;
import org.eclipse.jface.viewers.ICellModifier;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
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
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.part.FileEditorInput;
import org.omnetpp.common.ui.FocusManager;
import org.omnetpp.common.ui.LocalTransfer;
import org.omnetpp.common.ui.TableTextCellEditor;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.scave.actions.CollapseSubtreeAction;
import org.omnetpp.scave.actions.CollapseTreeAction;
import org.omnetpp.scave.actions.ExpandSubtreeAction;
import org.omnetpp.scave.actions.NewInputFileAction;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ScaveEditorActions;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engineext.IResultFilesChangeListener;
import org.omnetpp.scave.engineext.ResultFileManagerChangeEvent;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave.model.ModelChangeEvent;
import org.omnetpp.scave.model.commands.CommandStack;
import org.omnetpp.scave.model.commands.SetInputFileCommand;
import org.omnetpp.scave.model2.ScaveModelUtil;

public class InputsPage extends FormEditorPage {
    private InputsTree treeViewer;
    protected CommandStack commandStack = new CommandStack("InputsPage");

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

        treeViewer = new InputsTree(getContent(), scaveEditor.getAnalysis(), scaveEditor.getResultFileManager(), true);
        treeViewer.getControl().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        // toolbar
        ScaveEditorActions actions = scaveEditor.getActions();
        addToToolbar(new NewInputFileAction());
        addToToolbar(actions.editInputFileAction);
        addToToolbar(actions.removeAction);
        addToToolbar(actions.reloadResultFilesAction);
        addSeparatorToToolbar();
        addToToolbar(new CollapseTreeAction(treeViewer));

        treeViewer.getTree().setFocus();

        //TODO treeViewer.addSelectionChangedListener(selectionChangedListener);

        // context menu
        UIUtils.createContextMenuFor(treeViewer.getTree(), true, new IMenuListener() {
            @Override
            public void menuAboutToShow(IMenuManager menuManager) {
                menuManager.add(new NewInputFileAction());
                menuManager.add(actions.editInputFileAction);
                menuManager.add(actions.removeAction);
                menuManager.add(new Separator());
                menuManager.add(actions.undoAction);
                menuManager.add(actions.redoAction);
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

        // set up cell editing
        treeViewer.setColumnProperties(new String[] { "pattern" });
        final TableTextCellEditor editors[] = new TableTextCellEditor[1];
        editors[0] = new TableTextCellEditor(treeViewer, 0);

        treeViewer.setCellEditors(editors);
        treeViewer.setCellModifier(new ICellModifier() {
            public boolean canModify(Object element, String property) {
                return element instanceof InputFile;
            }

            public Object getValue(Object element, String property) {
                if (!(element instanceof InputFile))
                    return null;
                return ((InputFile)element).getName();
            }

            public void modify(Object element, String property, Object value) {
                if (element instanceof Item)
                    element = ((Item) element).getData();
                if (!(element instanceof InputFile))
                    return;

                String newValue = value.toString();
                InputFile inputFile = (InputFile)element;
                if (!inputFile.getName().equals(newValue))
                    commandStack.execute(new SetInputFileCommand(inputFile, newValue));
            }
        });

        // ensure that focus gets restored correctly after user goes somewhere else and then comes back
        setFocusManager(new FocusManager(this));

        // refresh on workspace changes
        scaveEditor.getResultFilesTracker().addChangeListener(new IResultFilesChangeListener() {
            @Override
            public void resultFileManagerChanged(ResultFileManagerChangeEvent event) {
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
                    getDisplay().asyncExec(scheduledUpdate); //TODO remove??
                }
            }
        });

        getTreeViewer().addSelectionChangedListener(scaveEditor.getSelectionChangedListener());
    }

    public InputsTree getTreeViewer() {
        return treeViewer;
    }

    public CommandStack getCommandStack() {
		return commandStack;
	}

    @Override
    public boolean gotoObject(Object object) {
        if (object instanceof InputFile) {
            getTreeViewer().setSelection(new StructuredSelection(object));
            return true;
        }
        else if (object instanceof Inputs) {
            return true;
        }

        return false;
    }

    @Override
    public void updatePage(ModelChangeEvent event) {
        if (ScaveModelUtil.isInputsChange(event))
            getTreeViewer().refresh();
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
        ScaveModelUtil.addInputFiles(commandStack, scaveEditor.getAnalysis(), list);
    }

    protected IPath makeRelative(IPath path) {
        IPath baseDir = ((FileEditorInput)scaveEditor.getEditorInput()).getFile().getParent().getFullPath();
        return baseDir.isPrefixOf(path) ? path.removeFirstSegments(baseDir.segmentCount()) : path;
    }

}
