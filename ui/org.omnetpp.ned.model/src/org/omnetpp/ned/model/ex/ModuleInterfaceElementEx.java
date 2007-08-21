package org.omnetpp.ned.model.ex;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IHasGates;
import org.omnetpp.ned.model.interfaces.IInterfaceTypeNode;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeNode;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.pojo.ExtendsElement;
import org.omnetpp.ned.model.pojo.GateElement;
import org.omnetpp.ned.model.pojo.ModuleInterfaceElement;
import org.omnetpp.ned.model.pojo.ParamElement;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class ModuleInterfaceElementEx extends ModuleInterfaceElement implements IInterfaceTypeNode, IHasGates {

	private INEDTypeInfo typeInfo;
	protected DisplayString displayString = null;

    protected ModuleInterfaceElementEx() {
		super();
	}

    protected ModuleInterfaceElementEx(INEDElement parent) {
		super(parent);
	}

    public void setNEDTypeInfo(INEDTypeInfo typeInfo) {
    	this.typeInfo = typeInfo;
    }

    public INEDTypeInfo getNEDTypeInfo() {
    	return typeInfo;
    }

    @Override
    public void fireModelChanged(NEDModelEvent event) {
    	// invalidate cached display string because NED tree may have changed outside the DisplayString class
    	if (!NEDElementUtilEx.isDisplayStringUpToDate(displayString, this))
    		displayString = null;
    	super.fireModelChanged(event);
    }

    public DisplayString getDisplayString() {
    	if (displayString == null)
    		displayString = new DisplayString(this, NEDElementUtilEx.getDisplayStringLiteral(this));
    	displayString.setFallbackDisplayString(NEDElementUtilEx.displayStringOf(getFirstExtendsRef()));
    	return displayString;
    }

    // "extends" support
    public String getFirstExtends() {
        return NEDElementUtilEx.getFirstExtends(this);
    }

    public void setFirstExtends(String ext) {
        NEDElementUtilEx.setFirstExtends(this, ext);
    }

    public INEDTypeInfo getFirstExtendsNEDTypeInfo() {
        return resolveTypeName(getFirstExtends());
    }

    public INedTypeNode getFirstExtendsRef() {
        INEDTypeInfo it = getFirstExtendsNEDTypeInfo();
        return it == null ? null : it.getNEDElement();
    }

    public List<ExtendsElement> getAllExtends() {
        List<ExtendsElement> result = new ArrayList<ExtendsElement>();
        ExtendsElement extendsElement = getFirstExtendsChild();
        if (extendsElement == null)
            return result;

        for (INEDElement currChild : extendsElement)
            if (currChild instanceof ExtendsElement)
                result.add(extendsElement);

        return result;
    }

    // parameter query support
    public Map<String, ParamElement> getParamAssignments() {
        return getNEDTypeInfo().getParamAssignments();
    }

    public Map<String, ParamElement> getParamDeclarations() {
        return getNEDTypeInfo().getParamDeclarations();
    }

    // gate support
    public Map<String, GateElement> getGateSizes() {
        return getNEDTypeInfo().getGateSizes();
    }

    public Map<String, GateElement> getGateDeclarations() {
        return getNEDTypeInfo().getGateDeclarations();
    }

}
