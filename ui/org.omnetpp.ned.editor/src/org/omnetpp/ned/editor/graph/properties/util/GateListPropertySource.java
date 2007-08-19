package org.omnetpp.ned.editor.graph.properties.util;

import java.util.Map;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.ned.model.ex.GateNodeEx;
import org.omnetpp.ned.model.interfaces.IHasGates;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.pojo.GateNode;

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
        Map<String, GateNode> gates = model.getGates();

        pdesc = new PropertyDescriptor[gates.size()];
        totalParamCount = inheritedParamCount = 0;
        for (GateNode gateElement : gates.values()) {
            GateNodeEx gateDefNode = (GateNodeEx)gateElement;
            String typeString = gateDefNode.getAttribute(GateNodeEx.ATT_TYPE);
            String definedIn = "";
            if (gateDefNode.getContainingNedTypeNode() != model) {
                inheritedParamCount++;
                definedIn= " (inherited from "+((IHasName)gateDefNode.getContainingNedTypeNode()).getName()+")";
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
        if (!(id instanceof GateNodeEx))
            return getEditableValue();
        Map<String, GateNode> gateSizes = model.getGateSizes();
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
