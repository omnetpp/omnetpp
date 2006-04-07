package org.omnetpp.ned.editor.graph.model;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.omnetpp.ned2.model.DisplayString;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.SubmoduleDisplayString;
import org.omnetpp.ned2.model.pojo.SubmoduleNode;

public class SubmoduleNodeEx extends SubmoduleNode implements INedModule {
    // srcConns contains all connections where the sourcemodule is this module
	protected List<ConnectionNodeEx> srcConns = new ArrayList<ConnectionNodeEx>();
	// destConns contains all connections where the destmodule is this module
	protected List<ConnectionNodeEx> destConns = new ArrayList<ConnectionNodeEx>();
	
	public SubmoduleNodeEx() {
        init();
	}

	public SubmoduleNodeEx(NEDElement parent) {
		super(parent);
        init();
	}

    private void init() {
        // TODO correctly handle the initial naming for new nodes (name most be unique)
        setName("unnamed");
        setType("node");
    }
    
	public DisplayString getDisplayString() {
        // TODO mabe we should cache the created DisplayString object for performance reasons?
		return new SubmoduleDisplayString(NedElementExUtil.getDisplayString(this)); 
	}
	
	public void setDisplayString(DisplayString dspString) {
		NedElementExUtil.setDisplayString(this, dspString.toString());
	}

	protected CompoundModuleNodeEx getCompoundModule() {
		return (CompoundModuleNodeEx)(getParent().getParent());
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
        DisplayString dps = getDisplayString();

        return new Dimension(dps.getAsIntDef(DisplayString.Prop.WIDTH, -1),
                             dps.getAsIntDef(DisplayString.Prop.HEIGHT, -1));
    }

    public void setSize(Dimension size) {
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

    @Override
    public String debugString() {
        return "srcConnSize="+srcConns.size()+" destConnSize="+destConns.size();
    }

}
