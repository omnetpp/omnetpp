package org.omnetpp.ned2.model;

import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.displaymodel.IDisplayString.Prop;
import org.omnetpp.ned2.model.pojo.SubmoduleNode;

public class SubmoduleNodeEx extends SubmoduleNode
                            implements INamedGraphNode, IIndexable, IStringTyped {

    protected DisplayString displayString = null;

	SubmoduleNodeEx() {
        init();
	}

	SubmoduleNodeEx(NEDElement parent) {
		super(parent);
        init();
	}

    private void init() {
        setName(INamed.INITIAL_NAME);
        setType("node");
    }

    public String getNameWithIndex() {
        String result = getName();
        if (getVectorSize() != null && !"".equals(getVectorSize()))
            result += "["+getVectorSize()+"]";
        return result;
    }

    /**
     * @return The name of component but stripped any digits from the right ie: name123 would be name
     */
    public String getNameBase() {
    	String nameBase = getName();
    	int i=nameBase.length()-1;
    	while(i>=0 && Character.isDigit(nameBase.charAt(i))) --i;
    	// strip away the digits at the end
    	return nameBase.substring(0,i+1);
    }


    /**
     * Makes the current modulename unique, concatenating unique numbers at the end if necessary.
     * This method is effective ONLY if the submodule is already placed into the compound module.
     * ie. getCompoundModule should not return null;
     */
    public void makeNameUnique() {
    	List<SubmoduleNodeEx> smls = getCompoundModule().getOwnSubmodules();
    	Set<String> nameSet = new HashSet<String>(smls.size());
    	// create a set from the sibling submodules
    	for(SubmoduleNodeEx sm : smls)
    		if (sm != this)
    			nameSet.add(sm.getName().toLowerCase());

    	// if there is no sibling with the same name we don't have to change the name
    	if (!nameSet.contains(getName().toLowerCase()))
    		return;

    	// there is an other module with the same name, so find a new name
    	String baseName = getNameBase();
    	int i = 1;
    	while(nameSet.contains(new String(baseName+i).toLowerCase())) i++;
    	// we found a unique name
    	setName(baseName+i);
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
		NEDElementUtilEx.setDisplayString(this, displayString.toString());
        fireAttributeChangedToAncestors(IDisplayString.ATT_DISPLAYSTRING+"."+changedProp);
	}

	/**
	 * @return The compound module containing the definition of this connection
	 */
	public CompoundModuleNodeEx getCompoundModule() {
        NEDElement parent = getParent(); 
        while (parent != null && !(parent instanceof CompoundModuleNodeEx)) 
            parent = parent.getParent();
        return (CompoundModuleNodeEx)parent;
	}
    
	// connection related methods
    
	/**
	 * @return All source connections that connect to this node and defined 
     * in the parent compound module connections defined in derived modules 
     * are NOT included here
	 */
	public List<ConnectionNodeEx> getSrcConnections() {
		return getCompoundModule().getSrcConnectionsFor(this);
	}

    /**
     * @return All connections that connect to this node and defined in the 
     * parent compound module connections defined in derived modules are 
     * NOT included here
     */
	public List<ConnectionNodeEx> getDestConnections() {
		return getCompoundModule().getDestConnectionsFor(this);
	}

	@Override
    public String debugString() {
        return "submodule: "+getName();
    }

    // type support
    public INEDTypeInfo getTypeNEDTypeInfo() {
        String typeName = getType(); 
        if ( typeName == null || "".equals(typeName))
            return null;

        return getContainerNEDTypeInfo().getResolver().getComponent(typeName);
    }

    public NEDElement getTypeRef() {
        INEDTypeInfo it = getTypeNEDTypeInfo();
        return it == null ? null : it.getNEDElement();
    }

}
