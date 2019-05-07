package org.omnetpp.scave.editors.forms;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.core.runtime.Status;
import org.omnetpp.scave.model.ModelObject;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Base class for IScaveObjectEditForm.
 * Implements listener list management.
 *
 * @author tomi
 */
abstract class BaseScaveObjectEditForm implements IScaveObjectEditForm {

    protected ModelObject object; // the edited or created object
    private ListenerList listeners = new ListenerList();

    protected BaseScaveObjectEditForm(ModelObject object) {
        Assert.isNotNull(object);
        this.object = object;
    }

    public String getTitle() {
        return "Edit " + object.getClass().getName();
    }

    public String getDescription() {
        return ScaveModelUtil.getDescription(object);
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
