package org.omnetpp.ned2.model;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.displaymodel.IDisplayStringProvider;
import org.omnetpp.common.displaymodel.IDisplayString.Prop;
import org.omnetpp.ned2.model.pojo.ChannelInterfaceNode;

public class ChannelInterfaceNodeEx extends ChannelInterfaceNode 
		implements IDisplayStringProvider, IParentable, INamed, ITopLevelElement {

	protected DisplayString displayString = null;

	public ChannelInterfaceNodeEx() {
		super();
	}

	public ChannelInterfaceNodeEx(NEDElement parent) {
		super(parent);
	}

	public DisplayString getDisplayString() {
		if (displayString == null) {
			displayString = new DisplayString(this, NedElementExUtil.getDisplayString(this));
		}
		return displayString;
	}
	
	public void propertyChanged(Prop changedProp) {
		// syncronize it to the underlying model 
		NedElementExUtil.setDisplayString(this, displayString.toString());
        fireAttributeChangedToAncestors(IDisplayString.ATT_DISPLAYSTRING+"."+changedProp);
	}

}
