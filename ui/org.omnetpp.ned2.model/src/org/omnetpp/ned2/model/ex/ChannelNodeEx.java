package org.omnetpp.ned2.model.ex;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.displaymodel.IDisplayStringProvider;
import org.omnetpp.common.displaymodel.IDisplayString.Prop;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.interfaces.IDerived;
import org.omnetpp.ned2.model.interfaces.IImplementor;
import org.omnetpp.ned2.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned2.model.interfaces.INamed;
import org.omnetpp.ned2.model.interfaces.IParametrized;
import org.omnetpp.ned2.model.interfaces.IParentable;
import org.omnetpp.ned2.model.interfaces.ITopLevelElement;
import org.omnetpp.ned2.model.pojo.ChannelNode;
import org.omnetpp.ned2.model.pojo.ExtendsNode;
import org.omnetpp.ned2.model.pojo.InterfaceNameNode;

public final class ChannelNodeEx extends ChannelNode 
				implements IDisplayStringProvider, IParentable, 
                           INamed, IDerived, IImplementor, 
                           ITopLevelElement, IParametrized {

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
		return displayString;
	}

    public DisplayString getEffectiveDisplayString() {
        return NEDElementUtilEx.getEffectiveDisplayString(this);
    }
	
	public void propertyChanged(Prop changedProp) {
		// syncronize it to the underlying model 
        String newDspString = displayString.toString(); 
        NEDElementUtilEx.setDisplayString(this, newDspString);
        fireAttributeChanged(IDisplayString.ATT_DISPLAYSTRING+"."+changedProp, newDspString, null);
	}

    // EXTENDS support
    public String getFirstExtends() {
        ExtendsNode extendsNode = getFirstExtendsChild();
        if(extendsNode == null)
            return null;

        return extendsNode.getName();
    }

    public void setFirstExtends(String ext) {
        ExtendsNode extendsNode = getFirstExtendsChild();
            if (extendsNode == null) {
                extendsNode = (ExtendsNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NED_EXTENDS);
                appendChild(extendsNode);
            }
            extendsNode.setName(ext);
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
