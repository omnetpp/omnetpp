package org.omnetpp.ned.core.ui.misc;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.dialogs.ElementListSelectionDialog;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.ned.core.INedResources;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.model.NedTreeUtil;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;

/**
 * NED type selection dialog.
 * 
 * @author Andras
 */
public class NedTypeSelectionDialog extends ElementListSelectionDialog {

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

}
