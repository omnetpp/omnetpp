package org.omnetpp.ned.editor.graph.properties.util;

import java.util.Map;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.ParamNodeEx;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.IHasParameters;

/**
 * Property source to display all parameters for a given component
 * @author rhornig
 */
public class ParameterListPropertySource extends NotifiedPropertySource {
    public final static String CATEGORY = "parameters";
    public final static String DESCRIPTION = "List of parameters (direct and inherited)";
    protected IHasParameters model;
    protected PropertyDescriptor[] pdesc;
    protected int totalParamCount;
    protected int inheritedParamCount;

    public ParameterListPropertySource(IHasParameters model) {
        super((INEDElement)model);
        this.model = model;
    }

    @Override
    public IPropertyDescriptor[] getPropertyDescriptors() {
        Map<String, INEDElement> params = model.getParams();

        pdesc = new PropertyDescriptor[params.size()];
        totalParamCount = inheritedParamCount = 0;
        for(INEDElement paramElement : params.values()) {
            ParamNodeEx paramDefNode = (ParamNodeEx)paramElement;
            String typeString = (paramDefNode.getIsVolatile() ? "volatile " : "") + paramDefNode.getAttribute(ParamNodeEx.ATT_TYPE);
            String definedIn = "";
            if (paramDefNode.getContainingTopLevelElement() != model) {
                inheritedParamCount++;
                definedIn= " (inherited from "+((IHasName)paramDefNode.getContainingTopLevelElement()).getName()+")";
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
        // yust a little summary - show the number of submodules
        String summary = "";
        // if the property descriptor is not yet build, build it now
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
        Map<String, INEDElement> paramValues = model.getParamValues();
        ParamNodeEx paramDefNode = (ParamNodeEx)id;
        ParamNodeEx paramValueNode = ((ParamNodeEx)paramValues.get(paramDefNode.getName()));
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

}
