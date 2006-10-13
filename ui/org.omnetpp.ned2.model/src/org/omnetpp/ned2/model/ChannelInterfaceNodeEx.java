package org.omnetpp.ned2.model;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.displaymodel.IDisplayStringProvider;
import org.omnetpp.common.displaymodel.IDisplayString.Prop;
import org.omnetpp.ned2.model.pojo.ChannelInterfaceNode;
import org.omnetpp.ned2.model.pojo.ExtendsNode;

public class ChannelInterfaceNodeEx extends ChannelInterfaceNode 
		implements IDisplayStringProvider, IDerived, IParentable, INamed, ITopLevelElement {

	protected DisplayString displayString = null;

	ChannelInterfaceNodeEx() {
		super();
	}

	ChannelInterfaceNodeEx(NEDElement parent) {
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

    // EXTENDS SUPPORT 
    public String getExtends() {
        ExtendsNode extendsNode = getFirstExtendsChild();
        if(extendsNode == null)
            return null;

        return extendsNode.getName();
    }

    public void setExtends(String ext) {
        ExtendsNode extendsNode = getFirstExtendsChild();
            if (extendsNode == null) {
                extendsNode = (ExtendsNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NED_EXTENDS);
                appendChild(extendsNode);
            }
            extendsNode.setName(ext);
    }

    public ITypeInfo getExtendsTypeInfo() {
        String extendsName = getExtends(); 
        if ( extendsName == null || "".equals(extendsName))
            return null;

        return getContainerTypeInfo().getResolver().getComponent(extendsName);
    }

    public NEDElement getExtendsRef() {
        ITypeInfo it = getExtendsTypeInfo();
        return it == null ? null : it.getNEDElement();
    }

    // notification support
    public void propertyChanged(Prop changedProp) {
		// syncronize it to the underlying model 
		NEDElementUtilEx.setDisplayString(this, displayString.toString());
        fireAttributeChangedToAncestors(IDisplayString.ATT_DISPLAYSTRING+"."+changedProp);
	}

}
