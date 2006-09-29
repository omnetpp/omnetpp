package org.omnetpp.ned2.model;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.displaymodel.IDisplayString.Prop;
import org.omnetpp.ned2.model.pojo.SubmoduleNode;

public class SubmoduleNodeEx extends SubmoduleNode implements INamedGraphNode {
    // srcConns contains all connections where the sourcemodule is this module
	protected List<ConnectionNodeEx> srcConns = new ArrayList<ConnectionNodeEx>();
	// destConns contains all connections where the destmodule is this module
	protected List<ConnectionNodeEx> destConns = new ArrayList<ConnectionNodeEx>();
	protected DisplayString displayString = null;
	
	public SubmoduleNodeEx() {
        init();
	}

	public SubmoduleNodeEx(NEDElement parent) {
		super(parent);
        init();
	}

    private void init() {
        setName(INamed.INITIAL_NAME);
        setType("node");
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
    	List<SubmoduleNodeEx> smls = getCompoundModule().getSubmodules();
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
			displayString = new DisplayString(this, NedElementExUtil.getDisplayString(this));
		}
		return displayString;
	}
	
	public void propertyChanged(Prop changedProp) {
		// syncronize it to the underlying model 
		NedElementExUtil.setDisplayString(this, displayString.toString());
        fireAttributeChangedToAncestors(IDisplayString.ATT_DISPLAYSTRING+"."+changedProp);
	}

	public CompoundModuleNodeEx getCompoundModule() {
		if (getParent() == null) return null;
		return (CompoundModuleNodeEx)(getParent().getParent());
	}

	public List<ConnectionNodeEx> getSrcConnections() {
		return srcConns;
	}
	
	public List<ConnectionNodeEx> getDestConnections() {
		return destConns;
	}

    public void attachSrcConnection(ConnectionNodeEx conn) {
        assert(!srcConns.contains(conn));
        srcConns.add(conn);
        getCompoundModule().addConnection(conn);
        fireAttributeChangedToAncestors(ATT_SRC_CONNECTION);
    }

    public void detachSrcConnection(ConnectionNodeEx conn) {
        assert(srcConns.contains(conn));
        srcConns.remove(conn);
        getCompoundModule().removeConnection(conn);
        fireAttributeChangedToAncestors(ATT_SRC_CONNECTION);
    }

    public void attachDestConnection(ConnectionNodeEx conn) {
        assert(!destConns.contains(conn));
        destConns.add(conn);
        getCompoundModule().addConnection(conn);
        fireAttributeChangedToAncestors(ATT_DEST_CONNECTION);
    }

    public void detachDestConnection(ConnectionNodeEx conn) {
        assert(destConns.contains(conn));
        destConns.remove(conn);
        getCompoundModule().removeConnection(conn);
        fireAttributeChangedToAncestors(ATT_DEST_CONNECTION);
    }

	@Override
    public String debugString() {
        return "srcConnSize="+srcConns.size()+" destConnSize="+destConns.size();
    }

}
