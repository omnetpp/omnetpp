/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import static org.omnetpp.scave.editors.forms.IScaveObjectEditForm.PARAM_SELECTED_OBJECT;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.Callable;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.forms.ScaveObjectEditFormFactory;
import org.omnetpp.scave.editors.ui.EditDialog;
import org.omnetpp.scave.engine.ResultFileManager;

/**
 * Opens an edit dialog for the selected dataset, chart, chart sheet, etc.
 */
public class EditAction extends AbstractScaveAction {
    private Map<String,Object> formParameters = null;

    /**
     * Creates the action with an default title and icon, and without parameters.
     */
    public EditAction() {
        setText("Edit...");
        setToolTipText("Edit Selected Item");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_EDIT));
    }

    /**
     * Allows passing parameters to the action; e.g. for opening the Properties dialog
     * with a specific page selected, use the the following parameter:
     * formParameters.put(ChartEditForm.PROP_DEFAULT_TAB, ChartEditForm.TAB_MAIN);
     */
    public EditAction(String text, Map<String,Object> formParameters) {
        setText(text);
        setToolTipText("Edit the properties of the selected item");
        this.formParameters = formParameters;
    }

    public void setFormParameter(String paramName, Object paramValue) {
        if (formParameters == null)
            formParameters = new HashMap<>();
        formParameters.put(paramName, paramValue);
    }

    @Override
    protected void doRun(final ScaveEditor scaveEditor, final IStructuredSelection selection) {
        ResultFileManager.callWithReadLock(scaveEditor.getResultFileManager(), new Callable<Object>() {
            public Object call() throws Exception {
                if (isApplicable(scaveEditor, selection)) {
                    EObject editedObject = getEditedObject(scaveEditor, selection);
                    Object selectedObject = selection.getFirstElement();
                    setFormParameter(PARAM_SELECTED_OBJECT, selectedObject);
                    EditDialog dialog = new EditDialog(scaveEditor.getSite().getShell(), editedObject, scaveEditor, formParameters);
                    EStructuralFeature[] features = dialog.getFeatures();
                    if (features.length > 0)
                        dialog.open();
                }
                return null;
            }
        });
    }

    @Override
    public boolean isApplicable(final ScaveEditor editor, final IStructuredSelection selection) {
        EObject editedObject = getEditedObject(editor, selection);
        return editedObject != null && ScaveObjectEditFormFactory.instance().canCreateForm(editedObject);
    }

    private EObject getEditedObject(ScaveEditor editor, IStructuredSelection selection) {
        if (selection.size() == 1 && selection.getFirstElement() instanceof EObject)
            return (EObject)selection.getFirstElement();
        return null;
    }
}
