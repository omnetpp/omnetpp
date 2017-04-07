package org.omnetpp.scave.editors.forms;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.core.runtime.Status;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.jface.viewers.ILabelProvider;
import org.omnetpp.scave.editors.ui.ModelObjectPalette;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model2.IScaveEditorContext;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Base class for IScaveObjectEditForm.
 * Implements listener list management.
 *
 * @author tomi
 */
abstract class BaseScaveObjectEditForm implements IScaveObjectEditForm {

    protected EObject object; // the edited or created object
    protected EObject parent; // the parent of the edited object
    private ListenerList listeners = new ListenerList();

    protected BaseScaveObjectEditForm(EObject object, EObject parent) {
        Assert.isNotNull(object);
        this.object = object;
        this.parent = parent;
    }

    public String getTitle() {
        return object.eContainer() == null ?
                "Create '" + getEditedObjectName() + "' node" :
                "Edit '" + getEditedObjectName() + "' node";
    }

    protected String getEditedObjectName() {
        IScaveEditorContext editorContext = ScaveModelUtil.getScaveEditorContextFor(parent);
        Assert.isNotNull(editorContext);
        ILabelProvider labelProvider = editorContext.getScaveModelLavelProvider();
        // ask the label of a fresh instance, otherwise the value of the label feature also added to the label
        ScaveModelFactory factory = ScaveModelFactory.eINSTANCE;
        return labelProvider.getText(factory.create(object.eClass()));
    }


    public String getDescription() {
        return ModelObjectPalette.getDescription(object.eClass());
    }

    public IStatus validate() {
        return Status.OK_STATUS;
    }

    public void addChangeListener(Listener listener) {
        listeners.add(listener);
    }

    public void removeChangeListener(Listener listener) {
        listeners.remove(listener);
    }

    protected void fireChangeNotification() {
        for (Object listener : listeners.getListeners())
            ((Listener)listener).editFormChanged(this);
    }
}
