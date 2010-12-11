package org.omnetpp.ned.core.ui.misc;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.dialogs.ElementListSelectionDialog;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.DelayedJob;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.ned.core.INedResources;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.model.NedTreeUtil;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.notification.INedChangeListener;
import org.omnetpp.ned.model.notification.NedModelEvent;

/**
 * NED type selection dialog.
 * 
 * @author Andras
 */
public class NedTypeSelectionDialog extends ElementListSelectionDialog {

    INedChangeListener nedChangeListener = new INedChangeListener() {
        public void modelChanged(NedModelEvent event) {
            dialogUpdater.restartTimer();
        }
    };

    // updates the dialog contents after a 1000ms delay, provided the dialog still exists
    private DelayedJob dialogUpdater = new DelayedJob(1000) {
        public void run() {
            if (getShell() != null && !getShell().isDisposed()) {
                Debug.println("NedTypeSelectionDialog: running delayed update");
                INedResources nedResources = NedResourcesPlugin.getNedResources();
                updateElements(nedResources.getNedTypesFromAllProjects().toArray());
            }
        }
    };

    public static class NedLabelProvider extends LabelProvider {
        @Override
        public Image getImage(Object element) {
            INedTypeInfo nedType = (INedTypeInfo) element;
            return NedTreeUtil.getNedModelLabelProvider().getImage(nedType.getNedElement());
        }

        @Override
        public String getText(Object element) {
            INedTypeInfo nedType = (INedTypeInfo) element;
            String simpleName = nedType.getName();
            String fullyQualifiedName = nedType.getFullyQualifiedName();
            String typeName = nedType.getNedElement().getReadableTagName();
            if (simpleName.equals(fullyQualifiedName))
                return nedType.getName() + " -- " + typeName;
            else
                return nedType.getName() + " -- " + typeName + " (" + fullyQualifiedName + ")";
        }
        
    }

    /**
     * Creates the dialog with some default settings. Settings can be customized
     * (overwritten) using setters methods before opening the dialog.
     */
    public NedTypeSelectionDialog(Shell parent) {
        super(parent, new NedLabelProvider());
        setMessage("Select NED type:");
        setTitle("Select NED Type");
        INedResources nedResources = NedResourcesPlugin.getNedResources();
        setElements(nedResources.getNedTypesFromAllProjects().toArray());
        setDialogBoundsSettings(UIUtils.getDialogSettings(NedResourcesPlugin.getDefault(), "SelectNEDTypeDialog"), Dialog.DIALOG_PERSISTSIZE + Dialog.DIALOG_PERSISTLOCATION);
        nedResources.addNedModelChangeListener(nedChangeListener);
    }

    /**
     * Updates an existing dialog's contents
     */
    public void updateElements(Object[] elements) {
        setListElements(elements);
        if (elements.length == 0) {
            handleEmptyList();
        }
        else {
            // re-enable widgets disabled by initial handleEmptyList() call
            Label _fMessage = (Label)ReflectionUtils.getFieldValue(this, "fMessage");  // fMessage is private...
            Text _fFilterText = (Text)ReflectionUtils.getFieldValue(this, "fFilterText"); // fFilterText ditto...
            
            // inverse of handleEmptyList():
            _fMessage.setEnabled(true);
            _fFilterText.setEnabled(true);
            fFilteredList.setEnabled(true);
            updateOkState();
            
            // plus some code from AbstractElementListSelectionDialog.create():
            validateCurrentSelection();
            _fFilterText.selectAll();
            _fFilterText.setFocus();

        }
    }
    
    /** 
     * Return result[] with the correct type
     */
    @Override
    public INedTypeInfo[] getResult() {
        Object[] tmp = super.getResult();
        INedTypeInfo[] result = new INedTypeInfo[tmp.length];
        for (int i = 0; i < tmp.length; i++)
            result[i] = (INedTypeInfo) tmp[i];
        return result;
    }

    @Override
    public boolean close() {
        INedResources nedResources = NedResourcesPlugin.getNedResources();
        nedResources.removeNedModelChangeListener(nedChangeListener);
        dialogUpdater.cancel();
        return super.close();
    }

}
