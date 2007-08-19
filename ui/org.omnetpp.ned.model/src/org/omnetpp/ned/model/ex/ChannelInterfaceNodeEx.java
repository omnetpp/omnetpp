package org.omnetpp.ned.model.ex;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IInterfaceTypeNode;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeNode;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.pojo.ChannelInterfaceNode;
import org.omnetpp.ned.model.pojo.ExtendsNode;
import org.omnetpp.ned.model.pojo.ParamNode;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class ChannelInterfaceNodeEx extends ChannelInterfaceNode implements IInterfaceTypeNode {

	private INEDTypeInfo typeInfo;
	protected DisplayString displayString = null;

	protected ChannelInterfaceNodeEx() {
		super();
	}

    protected ChannelInterfaceNodeEx(INEDElement parent) {
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

    // EXTENDS SUPPORT
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

    public List<ExtendsNode> getAllExtends() {
        List<ExtendsNode> result = new ArrayList<ExtendsNode>();

        for (INEDElement currChild : this)
            if (currChild instanceof ExtendsNode)
                result.add((ExtendsNode)currChild);

        return result;
    }

    // parameter list support
    public Map<String, ParamNode> getParamValues() {
        return getNEDTypeInfo().getParamAssignments();
    }

    public Map<String, ParamNode> getParams() {
        return getNEDTypeInfo().getParamDeclarations();
    }

}
