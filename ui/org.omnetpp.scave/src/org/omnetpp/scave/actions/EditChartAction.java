/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import static org.omnetpp.scave.editors.forms.BaseScaveObjectEditForm.PARAM_SELECTED_OBJECT;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.Callable;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.EditDialog;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ModelObject;

/**
 * Opens an edit dialog for the selected chart
 */
public class EditChartAction extends AbstractScaveAction {
    private Map<String,Object> formParameters = null;

    /**
     * Creates the action with an default title and icon, and without parameters.
     */
    public EditChartAction() {
        setText("Edit properties");
        setToolTipText("Edit Chart");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_PROPERTIES));
    }

    /**
     * Allows passing parameters to the action; e.g. for opening the Properties dialog
     * with a specific page selected, use the the following parameter:
     * formParameters.put(ChartEditForm.PROP_DEFAULT_TAB, ChartEditForm.TAB_MAIN);
     */
    public EditChartAction(String text, Map<String,Object> formParameters) {
        setText(text);
        setToolTipText("Edit the properties of the selected chart");
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
                    ModelObject editedObject = getEditedObject(scaveEditor, selection);
                    Object selectedObject = selection.getFirstElement();
                    setFormParameter(PARAM_SELECTED_OBJECT, selectedObject);
                    EditDialog dialog = new EditDialog(scaveEditor.getSite().getShell(), editedObject, scaveEditor, formParameters);
                    dialog.open();
                }
                return null;
            }
        });
    }

    @Override
    public boolean isApplicable(final ScaveEditor editor, final IStructuredSelection selection) {
        ModelObject editedObject = getEditedObject(editor, selection);
        return editedObject instanceof Chart;
    }

    private ModelObject getEditedObject(ScaveEditor editor, IStructuredSelection selection) {
        if (selection.size() == 1 && selection.getFirstElement() instanceof ModelObject)
            return (ModelObject)selection.getFirstElement();
        return null;
    }
}
