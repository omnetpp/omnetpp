package org.omnetpp.ned.editor.graph.properties.util;

import java.util.Map;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.ex.ParamNodeEx;
import org.omnetpp.ned2.model.interfaces.IHasName;
import org.omnetpp.ned2.model.interfaces.IHasParameters;

/**
 * @author rhornig
 * Property source to display all parameters for a given component
 */
public class ParameterListPropertySource extends NotifiedPropertySource {
    public final static String CATEGORY = "parameters";
    public final static String DESCRIPTION = "List of parameters (direct and inherited)";
    protected IHasParameters model;
    protected PropertyDescriptor[] pdesc;
    protected int totalParamCount;
    protected int inheritedParamCount;
    
    public ParameterListPropertySource(IHasParameters model) {
        super((NEDElement)model);
        this.model = model;
    }

    @Override
    public IPropertyDescriptor[] getPropertyDescriptors() {
        Map<String, NEDElement> params = model.getParams();
        
        pdesc = new PropertyDescriptor[params.size()];
        totalParamCount = inheritedParamCount = 0;
        for(NEDElement paramElement : params.values()) {
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
        Map<String, NEDElement> paramValues = model.getParamValues();
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
