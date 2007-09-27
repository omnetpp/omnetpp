package org.omnetpp.ned.editor.graph.properties.util;

import java.util.Map;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;

import org.omnetpp.ned.model.ex.GateElementEx;
import org.omnetpp.ned.model.interfaces.IHasGates;

/**
 * Property source to display all submodules for a given compound module
 *
 * @author rhornig
 */
public class GateListPropertySource extends NedBasePropertySource {
    public final static String CATEGORY = "gates";
    public final static String DESCRIPTION = "List of gates (direct and inherited)";
    protected PropertyDescriptor[] pdesc;
    protected int totalParamCount;
    protected int inheritedParamCount;

    public GateListPropertySource(IHasGates model) {
        super(model);
    }

    @Override
    public IPropertyDescriptor[] getPropertyDescriptors() {
        Map<String, GateElementEx> gates = getHasGatesModel().getGateDeclarations();

        pdesc = new PropertyDescriptor[gates.size()];
        totalParamCount = inheritedParamCount = 0;
        for (GateElementEx gateDecl : gates.values()) {
            String typeString = gateDecl.getAttribute(GateElementEx.ATT_TYPE);
            String definedIn = "";
            if (gateDecl.getEnclosingTypeElement() != getHasGatesModel()) {
                inheritedParamCount++;
                definedIn= " (inherited from " + gateDecl.getEnclosingTypeElement().getName() + ")";
            }

            String label = typeString+" "+gateDecl.getName() + (gateDecl.getIsVector() ? "[]" : "");
            pdesc[totalParamCount] = new PropertyDescriptor(gateDecl, label);
            pdesc[totalParamCount].setCategory(CATEGORY);
            pdesc[totalParamCount].setDescription("Gate "+gateDecl.getNameWithIndex()+" of type "+typeString+definedIn+" - (read only)");
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
        Map<String, GateElementEx> gateSizes = getHasGatesModel().getGateSizes();
        GateElementEx gateDeclNode = (GateElementEx)id;
        GateElementEx gateSizeNode = gateSizes.get(gateDeclNode.getName());
        String valueString = gateSizeNode== null ? "" : gateSizeNode.getVectorSize();
        return valueString;
    }

    public IHasGates getHasGatesModel() {
        return (IHasGates)getModel();
    }

}
