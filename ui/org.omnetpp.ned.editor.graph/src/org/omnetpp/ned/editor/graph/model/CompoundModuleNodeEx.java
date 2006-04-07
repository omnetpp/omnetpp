package org.omnetpp.ned.editor.graph.model;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.omnetpp.ned.editor.graph.properties.SubmoduleDisplayPropertySource;
import org.omnetpp.ned2.model.DisplayString;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.DisplayString.Prop;
import org.omnetpp.ned2.model.pojo.CompoundModuleNode;
import org.omnetpp.ned2.model.pojo.ConnectionsNode;
import org.omnetpp.ned2.model.pojo.SubmoduleNode;
import org.omnetpp.ned2.model.pojo.SubmodulesNode;

public class CompoundModuleNodeEx extends CompoundModuleNode 
								  implements INedContainer, INedModule {
    
	// srcConns contains all connections where the sourcemodule is this module
	protected List<ConnectionNodeEx> srcConns = new ArrayList<ConnectionNodeEx>();
	// destConns contains all connections where the destmodule is this module
	protected List<ConnectionNodeEx> destConns = new ArrayList<ConnectionNodeEx>();

	public CompoundModuleNodeEx() {
	}

	public CompoundModuleNodeEx(NEDElement parent) {
		super(parent);
	}

	public DisplayString getDisplayString() {
        // TODO mabe we should cache the created DisplayString object for performance reasons?
        // TODO change this to CompoundmoduleDisplayString
		return new DisplayString(NedElementExUtil.getDisplayString(this)); 
	}
	
	public void setDisplayString(DisplayString dspString) {
		NedElementExUtil.setDisplayString(this, dspString.toString());
	}

	public List<SubmoduleNodeEx> getSubmodules() {
		List<SubmoduleNodeEx> result = new ArrayList<SubmoduleNodeEx>();
		SubmodulesNode submodulesNode = getFirstSubmodulesChild();
		if (submodulesNode == null)
			return result;
		for(NEDElement currChild : submodulesNode) 
			if (currChild instanceof SubmoduleNodeEx) 
				result.add((SubmoduleNodeEx)currChild);
				
		return result;
	}

	public SubmoduleNodeEx getSubmoduleByName(String submoduleName) {
		SubmodulesNode submodulesNode = getFirstSubmodulesChild();
		if (submoduleName == null)
			return null;
		return (SubmoduleNodeEx)submodulesNode
					.getFirstChildWithAttribute(NED_SUBMODULE, SubmoduleNode.ATT_NAME, submoduleName);
	}

	public List<ConnectionNodeEx> getConnections() {
		List<ConnectionNodeEx> result = new ArrayList<ConnectionNodeEx>();
		ConnectionsNode connectionsNode = getFirstConnectionsChild();
		if (connectionsNode == null)
			return result;
		for(NEDElement currChild : connectionsNode) 
			if (currChild instanceof ConnectionNodeEx) 
				result.add((ConnectionNodeEx)currChild);
				
		return result;
	}

    public List<? extends INedModule> getModelChildren() {
        return getSubmodules();
    }

	public List<ConnectionNodeEx> getSrcConnections() {
		return srcConns;
	}
	
	public List<ConnectionNodeEx> getDestConnections() {
		return destConns;
	}
	
    public void addSrcConnection(ConnectionNodeEx conn) {
        assert(!srcConns.contains(conn));
        srcConns.add(conn);
        fireAttributeChangedToAncestors(ATT_SRC_CONNECTION);
    }

    public void removeSrcConnection(ConnectionNodeEx conn) {
        assert(srcConns.contains(conn));
        srcConns.remove(conn);
        fireAttributeChangedToAncestors(ATT_SRC_CONNECTION);
    }

    public void addDestConnection(ConnectionNodeEx conn) {
        assert(!destConns.contains(conn));
        destConns.add(conn);
        fireAttributeChangedToAncestors(ATT_DEST_CONNECTION);
    }

    public void removeDestConnection(ConnectionNodeEx conn) {
        assert(destConns.contains(conn));
        destConns.remove(conn);
        fireAttributeChangedToAncestors(ATT_DEST_CONNECTION);
    }

	public void addModelChild(INedModule child) {
        SubmodulesNode snode = getFirstSubmodulesChild();
        if (snode == null) 
            snode = (SubmodulesNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NEDElementFactoryEx.NED_SUBMODULES, this);

        snode.appendChild((NEDElement)child);
	}

	public void removeModelChild(INedModule child) {		
        SubmodulesNode snode = getFirstSubmodulesChild();
        if (snode == null) 
            snode = (SubmodulesNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NEDElementFactoryEx.NED_SUBMODULES, this);
		
        snode.removeChild((NEDElement)child);
	}

	public void insertModelChild(int index, INedModule child) {
		// check wheter Submodules node exists and create one if doesn't
		SubmodulesNode snode = getFirstSubmodulesChild();
		if (snode == null) 
			snode = (SubmodulesNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NEDElementFactoryEx.NED_SUBMODULES, this);
		
		NEDElement insertBefore = snode.getFirstChild();
		for(int i=0; (i<index) && (insertBefore!=null); ++i) 
			insertBefore = insertBefore.getNextSibling();
		
		snode.insertChildBefore(insertBefore, (NEDElement)child);
	}

	public void insertModelChild(INedModule insertBefore, INedModule child) {
		// check wheter Submodules node exists and create one if doesn't
		SubmodulesNode snode = getFirstSubmodulesChild();
		if (snode == null) 
			snode = (SubmodulesNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NEDElementFactoryEx.NED_SUBMODULES, this);
		
		snode.insertChildBefore((NEDElement)insertBefore, (NEDElement)child);
	}

    public Point getLocation() {
        DisplayString dps = getDisplayString();
        Integer x = dps.getAsInteger(DisplayString.Prop.X);
        Integer y = dps.getAsInteger(DisplayString.Prop.Y);
        // if it's unspecified in any direction we should return a NULL constraint
        if (x == null || y == null)
            return null;
        
        return new Point (x,y);
    }

    public void setLocation(Point location) {
        DisplayString dps = getDisplayString();
        
        // if location is not specified, remove the constraint from the display string
        if (location == null) {
            dps.set(DisplayString.Prop.X, null);
            dps.set(DisplayString.Prop.Y, null);
        } else { 
            dps.set(DisplayString.Prop.X, location.x);
            dps.set(DisplayString.Prop.Y, location.y);
        }

        setDisplayString(dps);
    }

    public Dimension getSize() {
        // FIXME get the propertysource from the model if it is already registered
        DisplayString dps = getDisplayString();

        return new Dimension(dps.getAsIntDef(DisplayString.Prop.WIDTH, -1),
                             dps.getAsIntDef(DisplayString.Prop.HEIGHT, -1));
    }

    public void setSize(Dimension size) {
        // FIXME get the propertysource from the model if it is already registered
        DisplayString dps = getDisplayString();
        
        // if the size is unspecified, remove the size constraint from the model
        if (size == null || size.width < 0 ) 
            dps.set(DisplayString.Prop.WIDTH, null);
        else
            dps.set(DisplayString.Prop.WIDTH, size.width);

        // if the size is unspecified, remove the size constraint from the model
        if (size == null || size.height < 0) 
            dps.set(DisplayString.Prop.HEIGHT, null);
        else
            dps.set(DisplayString.Prop.HEIGHT, size.height);
        
        setDisplayString(dps);
    }

}
