/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.forms;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.swt.widgets.Composite;

/**
 * Common interface of the forms for editing scave objects.
 *
 * EditDialog and NewScaveObjectWizard uses this interface.
 * ScaveObjectEditFormFactory can be used to create the
 * edit forms for scave objects.
 *
 * @author tomi
 */
public interface IScaveObjectEditForm {

    /**
     * Key for accessing the selected object as a form parameter.
     */
    final String PARAM_SELECTED_OBJECT = "SelectedObject";

    /**
     * Returns the string displayed in the title are of the dialog.
     */
    String getTitle();

    /**
     * Returns the string displayed below the title.
     */
    String getDescription();

    /**
     * Adds the controls of the form to the panel.
     */
    void populatePanel(Composite panel);

    /**
     * Returns the features edited on this form.
     * Do not modify the returned array.
     */
    EStructuralFeature[] getFeatures();

    /**
     * Returns the value of the control associated with the specified feature.
     */
    Object getValue(EStructuralFeature feature);

    /**
     * Sets the value of the control associated with the specified feature.
     */
    void setValue(EStructuralFeature feature, Object value);

    /**
     * Validates the content of the form.
     * Returns a status object, that can be a simple {@link IStatus},
     * or a {@link org.eclipse.core.runtime.MultiStatus MultiStatus}
     * containing multiple errors and warnings.
     */
    IStatus validate();

    /**
     * Add a listener to the listener list.
     * Listeners receive notifications when some field in the form changes.
     */
    void addChangeListener(Listener listener);

    /**
     * Removes a listener from the listener list.
     */
    void removeChangeListener(Listener listener);

    /**
     * Interface to receive change notifications.
     */
    interface Listener {
        void editFormChanged(IScaveObjectEditForm form);
    }
}
