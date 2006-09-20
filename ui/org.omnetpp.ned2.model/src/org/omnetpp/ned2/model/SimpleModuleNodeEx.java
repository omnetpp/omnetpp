package org.omnetpp.ned2.model;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IDisplayStringProvider;
import org.omnetpp.common.displaymodel.IDisplayString.Prop;
import org.omnetpp.ned2.model.pojo.SimpleModuleNode;

public class SimpleModuleNodeEx extends SimpleModuleNode 
				implements IDisplayStringProvider, IElement, INamed  {
	protected DisplayString displayString = null;
	
	public SimpleModuleNodeEx() {
        init();
	}

	public SimpleModuleNodeEx(NEDElement parent) {
		super(parent);
        init();
	}

    private void init() {
        // TODO correctly handle the initial naming for new nodes (name must be unique)
        setName("unnamed");
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
//        fireAttributeChangedToAncestors(IDisplayString.ATT_DISPLAYSTRING+"."+changedProp);
	}


}
