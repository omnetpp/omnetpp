package org.omnetpp.ned.editor.graph.model.commands;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.gef.commands.Command;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.omnetpp.ned.editor.graph.misc.BlockingMenu;
import org.omnetpp.ned2.model.CompoundModuleNodeEx;
import org.omnetpp.ned2.model.ConnectionNodeEx;
import org.omnetpp.ned2.model.IConnectable;
import org.omnetpp.ned2.model.IConnectionContainer;
import org.omnetpp.ned2.model.IElement;
import org.omnetpp.ned2.model.INamedGraphNode;
import org.omnetpp.ned2.model.ModelUtil;
import org.omnetpp.ned2.model.SubmoduleNodeEx;
import org.omnetpp.ned2.model.pojo.ConnectionNode;
import org.omnetpp.ned2.model.pojo.GateNode;
import org.omnetpp.resources.INEDComponent;
import org.omnetpp.resources.NEDResourcesPlugin;

/**
 * (Re)assigns a Connection to srcModule/destModule sub/compound module gates and also adds it to the
 * model (to the compound module's connectios section) (or removes it if the new source or destination is NULL)
 * @author rhornig
 */
public class ConnectionCommand extends Command {

    protected IConnectable oldSrcModule;
    protected String oldSrcGate;
    protected IConnectable oldDestModule;
    protected String oldDestGate;
    protected IConnectable srcModule;
    protected String srcGate;
    protected IConnectable destModule;
    protected String destGate;
    protected int oldDirection;
    protected int direction = ConnectionNode.NED_ARROWDIR_L2R;
    protected ConnectionNodeEx connNode;
    protected ConnectionNodeEx connNodeNextSibling = null;
    protected IConnectionContainer parent = null;

    @Override
    public String getLabel() {
        if (connNode != null && srcModule == null && destModule == null)
            return "Delete connection";
        
        if (connNode != null && srcModule != null && destModule != null)
            return "Create connection";

        return "Move connection";
    }

    public ConnectionCommand (ConnectionNodeEx conn) {
        connNode = conn;
        oldSrcModule = connNode.getSrcModuleRef();
        oldDestModule = connNode.getDestModuleRef();
        oldSrcGate = connNode.getSrcGate();
        oldDestGate = connNode.getDestGate();
        oldDirection = connNode.getArrowDirection();
    }
    /**
     * Handles which module can be connected to which
     */
    @Override
    public boolean canExecute() {
    	// allow deletion (both module is null)
    	if(srcModule == null && destModule == null)
    		return true;
    	// we can connect two submodules module ONLY if they are siblings (ie they have the same parent)
    	IConnectable sMod = srcModule != null ? srcModule : connNode.getSrcModuleRef();
    	IConnectable dMod = destModule != null ? destModule : connNode.getDestModuleRef();

    	if(sMod instanceof SubmoduleNodeEx && dMod instanceof SubmoduleNodeEx &&
    			((IElement)sMod).getParent() == ((IElement)dMod).getParent()) 
    		return true;
    	
    	// if one module is bubmodule and the other is compound, the compound module MUST contain the submodule
    	if (sMod instanceof CompoundModuleNodeEx && dMod instanceof SubmoduleNodeEx &&
    			((SubmoduleNodeEx)dMod).getCompoundModule() == sMod)
    		return true;
    	
    	if (dMod instanceof CompoundModuleNodeEx && sMod instanceof SubmoduleNodeEx &&
    			((SubmoduleNodeEx)sMod).getCompoundModule() == dMod)
    		return true;
    	
    	// do not allow command inany other cases
        return false;
    }

    @Override
    public void execute() {
   		// do the changes
    	redo();

    	// after we connected the modules, let's ask the user which gates should be connected
    	if(srcModule != null || destModule != null) {
    		// ask the user for ser and dest gate names
    		ConnectionNodeEx tempConn = askForTemplateConnection(srcModule, destModule);
    		// if both gates are specified by the user do the model change
    		if (tempConn != null) {
        		srcGate = tempConn.getSrcGate();
        		destGate = tempConn.getDestGate();
        		direction = tempConn.getArrowDirection();
        		redo();
    		}
    		else //otherwise revert the connection change (user cancel)
    			undo();
    	}
    }

    @Override
    public void redo() {
        if (srcModule != null && oldSrcModule != srcModule) 
            connNode.setSrcModuleRef(srcModule);
        
        if (srcGate != null && oldSrcGate != srcGate)
            connNode.setSrcGate(srcGate);
        
        if (destModule != null && oldDestModule != destModule) 
            connNode.setDestModuleRef(destModule);
        
        if (destGate != null && oldDestGate != destGate)
            connNode.setDestGate(destGate);
        
        connNode.setArrowDirection(direction);
        
        // if both src and dest module should be detached then remove it 
        // from the model totally (ie delete it)
        if (srcModule == null && destModule == null) {
            // just store the NEXT sibling so we can put it back during undo to the right place
            connNodeNextSibling = (ConnectionNodeEx)connNode.getNextConnectionNodeSibling();
            // store the parent too so we now where to put it back during undo
            parent = (IConnectionContainer)connNode.getParent().getParent();
            // now detach from both src and dest modules
            connNode.setSrcModuleRef(null);
            connNode.setDestModuleRef(null);
            // and remove from the parent too
           	connNode.removeFromParent();
        }
    }

    @Override
    public void undo() {
        // if it was removed from the model, put it back
        if (connNode.getParent() == null && parent != null)
            parent.insertConnection(connNodeNextSibling, connNode);
        
        // attach to the original modules and gates
        connNode.setSrcModuleRef(oldSrcModule);
        connNode.setSrcGate(oldSrcGate);
        connNode.setDestModuleRef(oldDestModule);
        connNode.setDestGate(oldDestGate);
        connNode.setArrowDirection(oldDirection);
    }

    public void setSrcModule(IConnectable newSrcModule) {
        srcModule = newSrcModule;
    }

    public void setSrcGate(String newSrcGate) {
        srcGate = newSrcGate;
    }

    public void setDestModule(IConnectable newDestModule) {
        destModule = newDestModule;
    }

    public void setDestGate(String newDestGate) {
        destGate = newDestGate;
    }

	public String getDestGate() {
		return destGate;
	}

	public IConnectable getDestModule() {
		return destModule;
	}

	public String getSrcGate() {
		return srcGate;
	}

	public IConnectable getSrcModule() {
		return srcModule;
	}
	
	/**
	 * This method ask the user which gates should be connected on the source and destination module
	 * @param askForSrcGate
	 * @param askForDestGate
	 */
	protected ConnectionNodeEx askForTemplateConnection(IConnectable srcModule, IConnectable destModule) {
		// do not ask anything 
		if (srcModule == null && destModule == null)
			return null;
		
		List<GateNode> srcOutModuleGates = getModuleGates(srcModule, GateNode.NED_GATETYPE_OUTPUT);
		List<GateNode> srcInOutModuleGates = getModuleGates(srcModule, GateNode.NED_GATETYPE_INOUT);
		List<GateNode> destInModuleGates = getModuleGates(destModule, GateNode.NED_GATETYPE_INPUT);
		List<GateNode> destInOutModuleGates = getModuleGates(srcModule, GateNode.NED_GATETYPE_INOUT);

		BlockingMenu menu = new BlockingMenu(Display.getCurrent().getActiveShell(), SWT.NONE);
		
		for (GateNode srcOut : srcOutModuleGates)
			for(GateNode destIn : destInModuleGates) {
				MenuItem mi = menu.addMenuItem(SWT.PUSH);
				// add the gate names to the menu item as additional widget data
				ConnectionNodeEx conn = new ConnectionNodeEx();
				// attach it to the src and dest modules
				conn.setSrcModuleRef(srcModule);
				conn.setDestModuleRef(destModule);
				// set the possible gates
				conn.setSrcGate(srcOut.getName());
				conn.setDestGate(destIn.getName());
				conn.setArrowDirection(ConnectionNode.NED_ARROWDIR_L2R);
				// but remove from the model
				conn.removeFromParent();
				mi.setData(conn);
				mi.setText(ModelUtil.generateNedSource(conn, false).trim());
			}
				
		for (GateNode srcInOut : srcInOutModuleGates)
			for(GateNode destInOut : destInOutModuleGates) {
				MenuItem mi = menu.addMenuItem(SWT.PUSH);
				// add the gate names to the menu item as additional widget data
				ConnectionNodeEx conn = new ConnectionNodeEx();
				// attach it to the src and dest modules
				conn.setSrcModuleRef(srcModule);
				conn.setDestModuleRef(destModule);
				// set the possible gates
				conn.setSrcGate(srcInOut.getName());
				conn.setDestGate(destInOut.getName());
				conn.setArrowDirection(ConnectionNode.NED_ARROWDIR_BIDIR);
				// but remove from the model
				conn.removeFromParent();
				mi.setData(conn);
				mi.setText(ModelUtil.generateNedSource(conn, false).trim());
			}

		MenuItem selection = menu.open();
		if (selection == null)
			return null;

		return (ConnectionNodeEx)selection.getData();
	}

	protected static List<GateNode> getModuleGates(IConnectable module, int gateType) {
		List<GateNode> result = new ArrayList<GateNode>();
		
		// the type is the compound module's name
		// the gate type depends 
		String moduleType = "";
		if (module instanceof CompoundModuleNodeEx) {
			moduleType = ((CompoundModuleNodeEx)module).getName();
			// if we connect a compound module swap the gate type (in<>out) submodule.out -> out
			gateType = (gateType == GateNode.NED_GATETYPE_INPUT) ? GateNode.NED_GATETYPE_OUTPUT : 
				(gateType == GateNode.NED_GATETYPE_OUTPUT ? GateNode.NED_GATETYPE_INPUT : GateNode.NED_GATETYPE_INOUT);
		}
		if (module instanceof SubmoduleNodeEx) {
			moduleType = ((SubmoduleNodeEx)module).getType();
		}
		
		INEDComponent comp = NEDResourcesPlugin.getNEDResources().getComponent(moduleType);
		if (comp == null)
			return result;
		
		for(String s : comp.getGateNames()) {
			GateNode currGate = (GateNode)comp.getMember(s);
			if (currGate.getType() == gateType)
				result.add(currGate);
		}

		return result;
	}
}
