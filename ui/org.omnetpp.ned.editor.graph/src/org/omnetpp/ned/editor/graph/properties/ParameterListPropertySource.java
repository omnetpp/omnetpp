package org.omnetpp.ned.editor.graph.properties;

import java.util.Map;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.ned2.model.INamed;
import org.omnetpp.ned2.model.IParametrized;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.ParamNodeEx;

/**
 * @author rhornig
 * Property source to display all submodules for a given compound module
 */
public class ParameterListPropertySource extends NotifiedPropertySource {
    protected IParametrized model;
    protected PropertyDescriptor[] pdesc;
    protected int totalParamCount;
    protected int inheritedParamCount;
    
    public ParameterListPropertySource(IParametrized model) {
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
                definedIn= " (inherited from "+((INamed)paramDefNode.getContainingTopLevelElement()).getName()+")";
            }
            pdesc[totalParamCount] = new PropertyDescriptor(paramDefNode, typeString +" "+paramDefNode.getName());
            pdesc[totalParamCount].setCategory("Parameters");
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
