package org.omnetpp.ned.editor.graph.properties.util;

import java.util.Map;

import org.eclipse.jface.viewers.DialogCellEditor;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.ned.editor.MultiPageNedEditor;
import org.omnetpp.ned.editor.graph.misc.ParametersDialog;
import org.omnetpp.ned.model.ex.ParamNodeEx;
import org.omnetpp.ned.model.interfaces.IHasParameters;
import org.omnetpp.ned.model.pojo.ParamNode;

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
            ParametersDialog dialog =
                new ParametersDialog(Display.getDefault().getActiveShell(), model);

            // if the dialog is cancelled, the command is not executable
            dialog.open();
            // execute the command in the active editors command stack
            // this is a little hack because we cannot access the command stack from the UndoablePropertySheetEntry
            IEditorPart activeEditor = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage().getActiveEditor();
            if (activeEditor instanceof MultiPageNedEditor) {
                MultiPageNedEditor mpNedEditor = (MultiPageNedEditor) activeEditor;
                mpNedEditor.getGraphEditor().getEditDomain().getCommandStack().execute(dialog.getResultCommand());
            }
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
        Map<String, ParamNode> params = model.getParams();

        pdesc = new PropertyDescriptor[params.size()];
        totalParamCount = inheritedParamCount = 0;
        for (ParamNode paramElement : params.values()) {
            ParamNodeEx paramDefNode = (ParamNodeEx)paramElement;
            String typeString = (paramDefNode.getIsVolatile() ? "volatile " : "") + paramDefNode.getAttribute(ParamNodeEx.ATT_TYPE);
            String definedIn = "";
            if (paramDefNode.getEnclosingTypeNode() != model) {
                inheritedParamCount++;
                definedIn= " (inherited from " + paramDefNode.getEnclosingTypeNode().getName() + ")";
            }
            pdesc[totalParamCount] = new PropertyDescriptor(paramDefNode, typeString +" "+paramDefNode.getName());
            pdesc[totalParamCount].setCategory(CATEGORY);
            pdesc[totalParamCount].setDescription("Parameter "+paramDefNode.getName()+" with type "+typeString+definedIn+" - (read only)");
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
        if (!(id instanceof ParamNodeEx))
            return getEditableValue();
        Map<String, ParamNode> paramValues = model.getParamValues();
        ParamNodeEx paramDefNode = (ParamNodeEx)id;
        ParamNodeEx paramValueNode = (ParamNodeEx)paramValues.get(paramDefNode.getName());
        String valueString = paramValueNode== null ? "" :paramValueNode.getValue();
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
