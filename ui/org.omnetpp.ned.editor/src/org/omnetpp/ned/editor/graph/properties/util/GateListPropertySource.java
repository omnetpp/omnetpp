package org.omnetpp.ned.editor.graph.properties.util;

import java.util.Map;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.ned.model.ex.GateElementEx;
import org.omnetpp.ned.model.interfaces.IHasGates;
import org.omnetpp.ned.model.pojo.GateElement;

/**
 * Property source to display all submodules for a given compound module
 *
 * @author rhornig
 */
public class GateListPropertySource extends NotifiedPropertySource {
    public final static String CATEGORY = "gates";
    public final static String DESCRIPTION = "List of gates (direct and inherited)";
    protected IHasGates model;
    protected PropertyDescriptor[] pdesc;
    protected int totalParamCount;
    protected int inheritedParamCount;

    public GateListPropertySource(IHasGates model) {
        super(model);
        this.model = model;
    }

    @Override
    public IPropertyDescriptor[] getPropertyDescriptors() {
        Map<String, GateElement> gates = model.getGateDeclarations();

        pdesc = new PropertyDescriptor[gates.size()];
        totalParamCount = inheritedParamCount = 0;
        for (GateElement gateElement : gates.values()) {
            GateElementEx gateDefNode = (GateElementEx)gateElement;
            String typeString = gateDefNode.getAttribute(GateElementEx.ATT_TYPE);
            String definedIn = "";
            if (gateDefNode.getEnclosingTypeNode() != model) {
                inheritedParamCount++;
                definedIn= " (inherited from " + gateDefNode.getEnclosingTypeNode().getName() + ")";
            }
            pdesc[totalParamCount] = new PropertyDescriptor(gateDefNode, typeString );
            pdesc[totalParamCount].setCategory(CATEGORY);
            pdesc[totalParamCount].setDescription("Gate "+gateDefNode.getNameWithIndex()+" of type "+typeString+definedIn+" - (read only)");
            totalParamCount++;
        }

        return pdesc;
    }

    @Override
    public Object getEditableValue() {
        // just a little summary - show the number of submodules
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
        if (!(id instanceof GateElementEx))
            return getEditableValue();
        Map<String, GateElement> gateSizes = model.getGateSizes();
        GateElementEx gateDefNode = (GateElementEx)id;
        GateElementEx gateSizeNode = ((GateElementEx)gateSizes.get(gateDefNode.getName()));
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
