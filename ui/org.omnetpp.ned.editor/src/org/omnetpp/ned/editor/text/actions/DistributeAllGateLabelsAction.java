/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.text.actions;

import java.util.Collection;
import java.util.Comparator;

import org.eclipse.jface.text.ITextSelection;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.ViewerComparator;
import org.eclipse.jface.window.Window;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.dialogs.CheckedTreeSelectionDialog;
import org.omnetpp.common.editor.EditorUtil;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.core.refactoring.RefactoringTools;
import org.omnetpp.ned.core.refactoring.RefactoringTools.AddGateLabels;
import org.omnetpp.ned.editor.text.TextualNedEditor;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

/**
 * @author levy
 */
public class DistributeAllGateLabelsAction extends NedTextEditorAction {
	public static final String ID = "DistributeAllGateLabels";

	public DistributeAllGateLabelsAction(TextualNedEditor editor) {
		super(ID, editor);
	}

	private String getAddGateLabelsText(AddGateLabels addGateLabels) {
	    INEDTypeInfo typeInfo = addGateLabels.gate.getEnclosingTypeElement().getNEDTypeInfo();
        String packageName = typeInfo.getPackageName();
        String labels = " @labels(" + StringUtils.join(addGateLabels.labels, ",") + ")";
	    String gateName = addGateLabels.gate.getName();
	    String gateType = addGateLabels.gate.getAttribute("type");
        return typeInfo.getName() + "." + gateName + " (" + gateType + ")" + labels + "          " + packageName;
	}
	
	@Override
	public void run() {
	    TextualNedEditor textEditor = (TextualNedEditor)getTextEditor();
        ISelection selection = textEditor.getSelectionProvider().getSelection();

        if (selection instanceof ITextSelection) {
		    for (INedTypeElement typeElement : textEditor.getModel().getTopLevelTypeNodes()) {
		        final Collection<AddGateLabels> runnables = RefactoringTools.inferAllGateLabels(typeElement, true);

		        CheckedTreeSelectionDialog dialog = new CheckedTreeSelectionDialog(null,
                    new LabelProvider() {
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
                    
                    for (Object object : obejcts) {
                        AddGateLabels addGateLabels = (AddGateLabels)object;
                        NedFileElementEx element = addGateLabels.gate.getContainingNedFileElement();

                        try {
                            EditorUtil.openEditor(NEDResourcesPlugin.getNEDResources().getNedFile(element), false);
                            addGateLabels.run();
                        }
                        catch (PartInitException e) {
                        }
                    }
                }
		    }
		}
	}
}
