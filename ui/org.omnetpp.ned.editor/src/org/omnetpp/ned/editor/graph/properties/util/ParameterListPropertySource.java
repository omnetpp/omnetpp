package org.omnetpp.ned.editor.graph.properties.util;

import java.util.Map;

import org.eclipse.jface.viewers.DialogCellEditor;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;

import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.interfaces.IHasParameters;

/**
 * Property source to display all parameters for a given component
 *
 * @author rhornig
 */
public class ParameterListPropertySource extends NotifiedPropertySource
                            implements IDialogCellEditorProvider {
    // inner class to activate a dialog cell editor
    class CellEditor extends DialogCellEditor {
        @Override
        protected Object openDialogBox(Control cellEditorWindow) {
            // FIXME enable when the ParametersDialog is correctly implemented

//            ParametersDialog dialog =
//                new ParametersDialog(Display.getDefault().getActiveShell(), model);
//
//            // if the dialog is cancelled, the command is not executable
//            dialog.open();
//            // execute the command in the active editors command stack
//            // this is a little hack because we cannot access the command stack from the UndoablePropertySheetEntry
//            IEditorPart activeEditor = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage().getActiveEditor();
//            if (activeEditor instanceof MultiPageNedEditor) {
//                MultiPageNedEditor mpNedEditor = (MultiPageNedEditor) activeEditor;
//                mpNedEditor.getGraphEditor().getEditDomain().getCommandStack().execute(dialog.getResultCommand());
//            }
            return null;
        }
    }

    public final static String CATEGORY = "parameters";
    public final static String DESCRIPTION = "List of parameters (direct and inherited)";
    protected IHasParameters model;
    protected PropertyDescriptor[] pdesc;
    protected int totalParamCount;
    protected int inheritedParamCount;

    public ParameterListPropertySource(IHasParameters model) {
        super(model);
        this.model = model;
    }

    @Override
    public IPropertyDescriptor[] getPropertyDescriptors() {
        Map<String, ParamElementEx> params = model.getParamDeclarations();

        pdesc = new PropertyDescriptor[params.size()];
        totalParamCount = inheritedParamCount = 0;
        for (ParamElementEx paramDecl : params.values()) {
            String typeString = (paramDecl.getIsVolatile() ? "volatile " : "") + paramDecl.getAttribute(ParamElementEx.ATT_TYPE);
            String definedIn = "";
            if (paramDecl.getEnclosingTypeElement() != model) {
                inheritedParamCount++;
                definedIn= " (inherited from " + paramDecl.getEnclosingTypeElement().getName() + ")";
            }
            pdesc[totalParamCount] = new PropertyDescriptor(paramDecl, typeString +" "+paramDecl.getName());
            pdesc[totalParamCount].setCategory(CATEGORY);
            pdesc[totalParamCount].setDescription("Parameter "+paramDecl.getName()+" with type "+typeString+definedIn+" - (read only)");
            totalParamCount++;
        }

        return pdesc;
    }

    @Override
    public Object getEditableValue() {
        // just a little summary - show the number of submodules
        String summary = "";
        // if the property descriptor is not yet built, build it now
        if (pdesc == null)
            getPropertyDescriptors();

        if (pdesc != null )
            summary ="total: "+totalParamCount+" (inherited: "+inheritedParamCount+")";
        return summary;
    }

    @Override
    public Object getPropertyValue(Object id) {
        if (!(id instanceof ParamElementEx))
            return getEditableValue();
        Map<String, ParamElementEx> paramValues = model.getParamAssignments();
        ParamElementEx paramDefNode = (ParamElementEx)id;
        ParamElementEx paramValueNode = paramValues.get(paramDefNode.getName());
        String valueString = paramValueNode== null ? "" : paramValueNode.getValue();
        return valueString;
    }

    @Override
    public boolean isPropertyResettable(Object id) {
        return false;
    }

    @Override
    public boolean isPropertySet(Object id) {
        return false;
    }

    @Override
    public void resetPropertyValue(Object id) {
    }

    @Override
    public void setPropertyValue(Object id, Object value) {
    }

    public DialogCellEditor getCellEditor() {
        return new CellEditor();
    }

}
