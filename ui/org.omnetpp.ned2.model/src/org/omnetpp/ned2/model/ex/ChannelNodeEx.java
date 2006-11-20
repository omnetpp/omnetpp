package org.omnetpp.ned2.model.ex;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IHasDisplayString;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.interfaces.IHasAncestors;
import org.omnetpp.ned2.model.interfaces.IHasInterfaces;
import org.omnetpp.ned2.model.interfaces.IHasName;
import org.omnetpp.ned2.model.interfaces.IHasParameters;
import org.omnetpp.ned2.model.interfaces.IHasParent;
import org.omnetpp.ned2.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned2.model.interfaces.ITopLevelElement;
import org.omnetpp.ned2.model.pojo.ChannelNode;
import org.omnetpp.ned2.model.pojo.ExtendsNode;
import org.omnetpp.ned2.model.pojo.InterfaceNameNode;

public final class ChannelNodeEx extends ChannelNode 
				implements IHasDisplayString, IHasParent, 
                           IHasName, IHasAncestors, IHasInterfaces, 
                           ITopLevelElement, IHasParameters {

	protected DisplayString displayString = null;

    protected ChannelNodeEx() {
		super();
	}

    protected ChannelNodeEx(NEDElement parent) {
		super(parent);
	}

	public DisplayString getDisplayString() {
		if (displayString == null) {
			displayString = new DisplayString(this, NEDElementUtilEx.getDisplayString(this));
		}
        // by default we do not have a fallback display string
//        displayString.setDefaults(null);
		return displayString;
	}

    public DisplayString getEffectiveDisplayString() {
        return NEDElementUtilEx.getEffectiveDisplayString(this);
    }
	
    // EXTENDS support
    public String getFirstExtends() {
        return NEDElementUtilEx.getFirstExtends(this);
    }

    public void setFirstExtends(String ext) {
        NEDElementUtilEx.setFirstExtends(this, ext);
    }
    
    public INEDTypeInfo getFirstExtendsNEDTypeInfo() {
        String extendsName = getFirstExtends(); 
        INEDTypeInfo typeInfo = getContainerNEDTypeInfo(); 
        if ( extendsName == null || "".equals(extendsName) || typeInfo == null)
            return null;

        return typeInfo.getResolver().getComponent(extendsName);
    }

    public NEDElement getFirstExtendsRef() {
        INEDTypeInfo it = getFirstExtendsNEDTypeInfo();
        return it == null ? null : it.getNEDElement();
    }

    public List<ExtendsNode> getAllExtends() {
        List<ExtendsNode> result = new ArrayList<ExtendsNode>();
        ExtendsNode extendsNode = getFirstExtendsChild();
        if (extendsNode == null)
            return result;

        for(NEDElement currChild : extendsNode)
            if (currChild instanceof ExtendsNode)
                result.add(extendsNode);
        
        return result;
    }
    // parameter quiry support
    public Map<String, NEDElement> getParamValues() {
        return getContainerNEDTypeInfo().getParamValues();
    }

    public Map<String, NEDElement> getParams() {
        return getContainerNEDTypeInfo().getParams();
    }

    // interface implementation support
    public List<InterfaceNameNode> getAllInterfaces() {
        List<InterfaceNameNode> result = new ArrayList<InterfaceNameNode>();

        for(NEDElement currChild : this)
            if (currChild instanceof InterfaceNameNode)
                result.add((InterfaceNameNode)currChild);

        return result;
    }

}
