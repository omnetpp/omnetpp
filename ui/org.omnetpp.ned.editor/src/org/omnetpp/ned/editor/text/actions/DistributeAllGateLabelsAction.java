/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.text.actions;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Comparator;

import org.eclipse.jface.text.ITextSelection;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.ViewerComparator;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IEditorReference;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.dialogs.CheckedTreeSelectionDialog;
import org.eclipse.ui.part.FileEditorInput;
import org.omnetpp.common.editor.EditorUtil;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.core.refactoring.RefactoringTools;
import org.omnetpp.ned.core.refactoring.RefactoringTools.AddGateLabels;
import org.omnetpp.ned.editor.NedEditor;
import org.omnetpp.ned.editor.NedEditorPlugin;
import org.omnetpp.ned.editor.text.TextualNedEditor;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;

/**
 * @author levy
 */
public class DistributeAllGateLabelsAction extends NedTextEditorAction {
    public static final String ID = "DistributeAllGateLabels";

    public DistributeAllGateLabelsAction(TextualNedEditor editor) {
        super(ID, editor);
    }

    private String getAddGateLabelsText(AddGateLabels addGateLabels) {
        INedTypeInfo typeInfo = addGateLabels.gate.getEnclosingTypeElement().getNedTypeInfo();
        String packageName = typeInfo.getPackageName();
        String labels = " @labels(" + StringUtils.join(addGateLabels.labels, ",") + ")";
        String gateName = addGateLabels.gate.getName();
        String gateType = addGateLabels.gate.getAttribute("type");
        return typeInfo.getName() + "." + gateName + " (" + gateType + ")" + labels + "          " + packageName;
    }

    @Override
    protected void doRun() {
        TextualNedEditor textEditor = (TextualNedEditor)getTextEditor();
        ISelection selection = textEditor.getSelectionProvider().getSelection();

        if (selection instanceof ITextSelection) {
            IWorkbenchWindow workbenchWindow = getTextEditor().getEditorSite().getWorkbenchWindow();
            for (IWorkbenchPage page : workbenchWindow.getPages()) {
                for (IEditorReference editorReference : page.getEditorReferences()) {
                    if (editorReference.isDirty()) {
                        MessageBox errorMsg = new MessageBox(workbenchWindow.getShell(), SWT.ICON_ERROR | SWT.OK);
                        errorMsg.setText("Cannot distribute labels due to unsaved changes");
                        errorMsg.setMessage("Please save all changes in all editors and try again");
                        errorMsg.open();
                        return;
                    }
                }
            }

            final Collection<AddGateLabels> runnables = new ArrayList<AddGateLabels>();

            for (INedTypeElement typeElement : textEditor.getModel().getTopLevelTypeNodes())
                runnables.addAll(RefactoringTools.inferAllGateLabels(typeElement, true));

            CheckedTreeSelectionDialog dialog = new CheckedTreeSelectionDialog(null,
                new LabelProvider() {
                    @Override
                    public String getText(Object element) {
                        return getAddGateLabelsText((AddGateLabels)element);
                    }
                },
                new ITreeContentProvider() {
                    public Object[] getChildren(Object parentElement) {
                        return null;
                    }

                    public Object getParent(Object element) {
                        return runnables;
                    }

                    public boolean hasChildren(Object element) {
                        return false;
                    }

                    public Object[] getElements(Object inputElement) {
                        return runnables.toArray();
                    }

                    public void dispose() {
                        // void
                    }

                    public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
                        // void
                    }

            });
            dialog.setSize(100, 40);
            dialog.setTitle("Select Gates");
            dialog.setMessage("Select labels that should be added:");
            dialog.setHelpAvailable(false);
            dialog.setInput(runnables);
            dialog.setStatusLineAboveButtons(false);
            dialog.setInitialSelections(runnables.toArray());
            dialog.setComparator(new ViewerComparator() {
                @Override
                @SuppressWarnings("unchecked")
                protected Comparator getComparator() {
                    return new Comparator() {
                        public int compare(Object o1, Object o2) {
                            return ((String)o1).compareTo((String)o2);
                        }
                    };
                }
            });

            if (dialog.open() == Window.OK) {
                Object[] obejcts = dialog.getResult();
                // keeps track of NED editors that are switched to graphical editor to be able to switch them back
                ArrayList<NedEditor> switchedEditors = new ArrayList<NedEditor>();

                for (Object object : obejcts) {
                    AddGateLabels addGateLabels = (AddGateLabels)object;
                    NedFileElementEx nedFileElement = addGateLabels.gate.getContainingNedFileElement();

                    try {
                        FileEditorInput editorInput = new FileEditorInput(NedResourcesPlugin.getNedResources().getNedFile(nedFileElement));
                        IEditorPart editor = workbenchWindow.getActivePage().findEditor(editorInput);

                        // we cannot modify the model while the NED text editor is active due to the way it handles changes
                        // workaround by switching to the graphical editor and back
                        if (editor instanceof NedEditor) {
                            NedEditor nedEditor = (NedEditor)editor;

                            // NOTE: we must switch to the graphical editor, otherwise changes are lost and not reflected in the text editor
                            if (!nedEditor.isActiveEditor(nedEditor.getGraphEditor())) {
                                nedEditor.setActiveEditor(nedEditor.getGraphEditor());
                                switchedEditors.add(nedEditor);
                            }
                        }
                        else
                            EditorUtil.openEditor(NedResourcesPlugin.getNedResources().getNedFile(nedFileElement), false);

                        addGateLabels.run();
                    }
                    catch (PartInitException e) {
                        NedEditorPlugin.logError(e);
                    }
                }

                for (NedEditor nedEditor : switchedEditors)
                    nedEditor.setActiveEditor(nedEditor.getTextEditor());
            }
        }
    }
}
