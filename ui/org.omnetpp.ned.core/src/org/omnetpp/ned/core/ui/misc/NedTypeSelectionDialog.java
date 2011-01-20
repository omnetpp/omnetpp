package org.omnetpp.ned.core.ui.misc;

import java.util.Collection;

import org.eclipse.core.resources.IProject;
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
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.notification.INedChangeListener;
import org.omnetpp.ned.model.notification.NedModelEvent;

/**
 * NED type selection dialog. It can offer all types, a list of types, types visible 
 * from a project and/or filtered by a predicate. By default this dialog allows 
 * one item to be selected; call setMultipleSelection(true) to allow multiple selection.
 * 
 * @author Andras
 */
public class NedTypeSelectionDialog extends ElementListSelectionDialog {
    private INedTypeResolver.IPredicate typeFilter; 
    private IProject contextProject;
    private INedTypeInfo[] typesToOffer; // overrides previous two 


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
                updateElements(getElementsToShow());
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
        setDialogBoundsSettings(UIUtils.getDialogSettings(NedResourcesPlugin.getDefault(), "SelectNEDTypeDialog"), Dialog.DIALOG_PERSISTSIZE + Dialog.DIALOG_PERSISTLOCATION);
        INedResources nedResources = NedResourcesPlugin.getNedResources();
        nedResources.addNedModelChangeListener(nedChangeListener);
    }

    /**
     * Sets the context project. When non-null, only types visible from the given project are offered.
     */
    public void setContextProject(IProject contextProject) {
        this.contextProject = contextProject;
    }
    
    public IProject getContextProject() {
        return contextProject;
    }

    /**
     * Sets a filter predicate. When non-null, only types matching the predicate are offered. 
     * Can be combined with setContextProject().
     */
    public void setTypeFilter(INedTypeResolver.IPredicate typeFilter) {
        this.typeFilter = typeFilter;
    }

    public INedTypeResolver.IPredicate getTypeFilter() {
        return typeFilter;
    }

    /**
     * Sets the list of types to offer. This overrides the context project and the type filter.
     */
    public void setTypesToOffer(INedTypeInfo[] typesToOffer) {
        this.typesToOffer = typesToOffer;
    }
    
    public INedTypeInfo[] getTypesToOffer() {
        return typesToOffer;
    }

    protected INedTypeInfo[] getElementsToShow() {
        if (getTypesToOffer() != null)
            return getTypesToOffer();

        INedResources nedResources = NedResourcesPlugin.getNedResources();
        Collection<INedTypeInfo> result;

        if (getContextProject() == null) {
            if (getTypeFilter() == null)
                result = nedResources.getNedTypesFromAllProjects();
            else 
                result = nedResources.getNedTypesFromAllProjects(getTypeFilter());
        }
        else {
            if (getTypeFilter() == null)
                result = nedResources.getNedTypes(getContextProject());
            else 
                result = nedResources.getNedTypes(getTypeFilter(), getContextProject());
        }
        return result.toArray(new INedTypeInfo[]{});   
    }
    
    @Override
    public int open() {
        setElements(getElementsToShow());
        return super.open();
    }

    /**
     * Updates an existing dialog's contents. Most useful when NED types are being loaded
     * while the dialog is already open and the user can interact with it. (This can occur
     * just after IDE startup when no editors are open, and user hits the Open NED Type button.)   
     */
    protected void updateElements(Object[] elements) {
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
