package org.omnetpp.ned.core.ui.misc;

import java.util.ArrayList;
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
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.interfaces.INedTypeResolver.IPredicate;
import org.omnetpp.ned.model.notification.INedChangeListener;
import org.omnetpp.ned.model.notification.NedModelEvent;
import org.omnetpp.ned.model.ui.NedModelLabelProvider;

/**
 * NED type selection dialog. It can offer types from all projects, or alternatively,
 * combine NED types from three sources:
 * <ul>
 *   <li> toplevel types visible from a given project (see setContextProject())
 *   <li> inner types of a given type (see setEnclosingNedType())
 *   <li> types in an explicitly passed in type list (see setTypeList())
 * </ul>
 *
 * If all such parameters are null, the dialog offers all types from all projects.
 * In all cases, the list can be filtered with a predicate.
 *
 * If new types become available while the dialog is open (e.g due to lazy loading of
 * NED files), the dialogs is kept up to date.
 *
 * By default this dialog allows one item to be selected; call setMultipleSelection(true)
 * to allow multiple selection.
 *
 * @author Andras
 */
public class NedTypeSelectionDialog extends ElementListSelectionDialog {
    private IProject contextProject; // if non-null: show toplevel types visible from this project
    private INedTypeInfo enclosingNedType; // if non-null: show inner types of this type
    private INedTypeInfo[] typeList; // if non-null: show these types
    private INedTypeResolver.IPredicate typeFilter;  // show only matching types
    private NedLabelProvider labelProvider;


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
        private boolean showProject;  // whether to show which project the type was defined in

        @Override
        public Image getImage(Object element) {
            INedTypeInfo nedType = (INedTypeInfo) element;
            return NedModelLabelProvider.getInstance().getImage(nedType.getNedElement());
        }

        @Override
        public String getText(Object element) {
            INedTypeInfo nedType = (INedTypeInfo) element;
            String simpleName = nedType.getName();
            String fullyQualifiedName = nedType.getFullyQualifiedName();
            String kindName = nedType.getNedElement().getReadableTagName();
            String nameAndKind = nedType.getName() + " -- " + kindName;
            if (showProject) {
                IProject project = nedType.getProject();
                String projectName = project==null ? "built-in" : project.getName();
                if (simpleName.equals(fullyQualifiedName))
                    return nameAndKind + " (" + projectName + ")";
                else
                    return nameAndKind + " (" + fullyQualifiedName + " - " + projectName + ")";
            }
            else {
                if (simpleName.equals(fullyQualifiedName))
                    return nameAndKind;
                else
                    return nameAndKind + " (" + fullyQualifiedName + ")";
            }
        }
    }

    /**
     * Creates the dialog with some default settings. Settings can be customized
     * (overwritten) using setters methods before opening the dialog.
     */
    public NedTypeSelectionDialog(Shell parent) {
        super(parent, new NedLabelProvider());
        labelProvider = (NedLabelProvider) ReflectionUtils.getFieldValue(this, "fRenderer"); // sorry...
        setMessage("Select NED type:");
        setTitle("Select NED Type");
        setDialogBoundsSettings(UIUtils.getDialogSettings(NedResourcesPlugin.getDefault(), "SelectNEDTypeDialog"), Dialog.DIALOG_PERSISTSIZE + Dialog.DIALOG_PERSISTLOCATION);
        setShowProject(true); // because contextProject == null here
        INedResources nedResources = NedResourcesPlugin.getNedResources();
        nedResources.addNedModelChangeListener(nedChangeListener);
    }

    /**
     * Enable/disable showing the defining project's name for each type.
     * Note: This flag is affected by other setters, so make the call to this method the last one.
     */
    public void setShowProject(boolean showProject) {
        labelProvider.showProject = showProject;
    }

    public boolean getShowProject() {
        return labelProvider.showProject;
    }

    /**
     * Sets the context project. When non-null, (filtered) types visible from the given project are offered.
     */
    public void setContextProject(IProject contextProject) {
        this.contextProject = contextProject;
        setShowProject(willShowTypesFromAllProjects());
    }

    public IProject getContextProject() {
        return contextProject;
    }

    /**
     * Sets an enclosing type. If non-null, also offer the (filtered) inner types of this type.
     */
    public void setEnclosingNedType(INedTypeInfo enclosingNedType) {
        this.enclosingNedType = enclosingNedType;
        setShowProject(willShowTypesFromAllProjects());
    }

    public INedTypeInfo getEnclosingNedType() {
        return enclosingNedType;
    }

    /**
     * Sets the list of types. This will still be filtered with the filter.
     */
    public void setTypeList(INedTypeInfo[] typeList) {
        this.typeList = typeList;
        setShowProject(willShowTypesFromAllProjects());
    }

    public INedTypeInfo[] getTypeList() {
        return typeList;
    }

    public boolean willShowTypesFromAllProjects() {
        return contextProject == null && enclosingNedType == null && typeList == null;
    }

    /**
     * Sets a filter predicate. When non-null, only types matching the predicate are offered.
     */
    public void setTypeFilter(INedTypeResolver.IPredicate typeFilter) {
        this.typeFilter = typeFilter;
    }

    public INedTypeResolver.IPredicate getTypeFilter() {
        return typeFilter;
    }

    protected INedTypeInfo[] getElementsToShow() {
        IPredicate typeFilter = getTypeFilter();
        INedResources nedResources = NedResourcesPlugin.getNedResources();
        Collection<INedTypeInfo> result;

        if (willShowTypesFromAllProjects()) {
            if (typeFilter == null)
                result = nedResources.getToplevelNedTypesFromAllProjects();
            else
                result = nedResources.getToplevelNedTypesFromAllProjects(typeFilter);
        }
        else {
            result = new ArrayList<INedTypeInfo>();
            if (getTypeList() != null)
                for (INedTypeInfo type : getTypeList())
                    if (typeFilter==null || typeFilter.matches(type))
                        result.add(type);

            if (getContextProject() != null) {
                if (typeFilter == null)
                    result.addAll(nedResources.getToplevelNedTypes(getContextProject()));
                else
                    result.addAll(nedResources.getToplevelNedTypes(typeFilter, getContextProject()));
            }

            if (getEnclosingNedType() != null)
                for (INedTypeElement type : getEnclosingNedType().getInnerTypes().values())
                    if (typeFilter==null || typeFilter.matches(type.getNedTypeInfo()))
                        result.add(type.getNedTypeInfo());
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
