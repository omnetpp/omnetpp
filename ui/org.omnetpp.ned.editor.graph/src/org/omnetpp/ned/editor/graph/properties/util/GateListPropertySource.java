package org.omnetpp.ned.editor.graph.properties.util;

import java.util.Map;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.GateNodeEx;
import org.omnetpp.ned2.model.interfaces.IGateContainer;
import org.omnetpp.ned2.model.interfaces.INamed;

/**
 * @author rhornig
 * Property source to display all submodules for a given compound module
 */
public class GateListPropertySource extends NotifiedPropertySource {
    public final static String CATEGORY = "gates";
    public final static String DESCRIPTION = "List of gates (direct and inherited)";
    protected IGateContainer model;
    protected PropertyDescriptor[] pdesc;
    protected int totalParamCount;
    protected int inheritedParamCount;
    
    public GateListPropertySource(IGateContainer model) {
        super((NEDElement)model);
        this.model = model;
    }

    @Override
    public IPropertyDescriptor[] getPropertyDescriptors() {
        Map<String, NEDElement> gates = model.getGates();
        
        pdesc = new PropertyDescriptor[gates.size()];
        totalParamCount = inheritedParamCount = 0;
        for(NEDElement gateElement : gates.values()) {
            GateNodeEx gateDefNode = (GateNodeEx)gateElement;
            String typeString = gateDefNode.getAttribute(GateNodeEx.ATT_TYPE);
            String definedIn = "";
            if (gateDefNode.getContainingTopLevelElement() != model) {
                inheritedParamCount++;
                definedIn= " (inherited from "+((INamed)gateDefNode.getContainingTopLevelElement()).getName()+")";
            }
            pdesc[totalParamCount] = new PropertyDescriptor(gateDefNode, typeString );
            pdesc[totalParamCount].setCategory("Gates");
            pdesc[totalParamCount].setDescription("Gate "+gateDefNode.getNameWithIndex()+" of type "+typeString+definedIn+" - (read only)");
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
        if (!(id instanceof GateNodeEx))
            return getEditableValue();
        Map<String, NEDElement> gateSizes = model.getGateSizes();
        GateNodeEx gateDefNode = (GateNodeEx)id;
        GateNodeEx gateSizeNode = ((GateNodeEx)gateSizes.get(gateDefNode.getName()));
        String valueString = gateSizeNode== null ? "" :gateSizeNode.getNameWithIndex();
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
