package org.omnetpp.ned.editor.graph.misc;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.MenuItem;
import org.omnetpp.ned2.model.CompoundModuleNodeEx;
import org.omnetpp.ned2.model.IConnectable;
import org.omnetpp.ned2.model.ModelUtil;
import org.omnetpp.ned2.model.SubmoduleNodeEx;
import org.omnetpp.ned2.model.pojo.ConnectionNode;
import org.omnetpp.ned2.model.pojo.GateNode;
import org.omnetpp.resources.INEDComponent;
import org.omnetpp.resources.NEDResourcesPlugin;

/**
 * @author rhornig
 * Helper class that allows to choose a connection for a mosule pair (src , dest)
 */
public class ConnectionChooser {
    private static final String DEFAULT_INDEX = "0";

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

    /**
	 * Creates a connection object from the provided gates
	 * 
	 * @param srcMod		Source module
	 * @param srcModPP	Index IF the srcMdule is vector (typically '0' or   
	 * @param srcGate
	 * @param srcGatePP
	 * @param destMod
	 * @param destModPP
	 * @param destGate
	 * @param destGatePP
	 * @return
	 */
	protected static ConnectionNode createTemplateConnection(
						IConnectable srcMod, boolean srcModPP, GateNode srcGate, boolean srcGatePP,
						IConnectable destMod, boolean destModPP, GateNode destGate, boolean destGatePP) {
		ConnectionNode conn = new ConnectionNode();
		// set the source and dest module names.
		// if compound module, name must be empty
		// for Submodules name must be the submodulename
		if(srcMod instanceof SubmoduleNodeEx)
			conn.setSrcModule(((SubmoduleNodeEx)srcMod).getName());
		else
			conn.setSrcModule(null);

		if(destMod instanceof SubmoduleNodeEx)
			conn.setDestModule(((SubmoduleNodeEx)destMod).getName());
		else
			conn.setDestModule(null);
		
		// set the possible gates
		conn.setSrcGate(srcGate.getName());
		conn.setDestGate(destGate.getName());
		// if both side is bidirectional gate, use a bidir connection
		if(srcGate.getType() == GateNode.NED_GATETYPE_INOUT && destGate.getType() == GateNode.NED_GATETYPE_INOUT)
			conn.setArrowDirection(ConnectionNode.NED_ARROWDIR_BIDIR);
		else
			conn.setArrowDirection(ConnectionNode.NED_ARROWDIR_L2R);
		
		// check if we have a module vector and add an index to it.
		if(srcGate.getIsVector())
			if(srcGatePP)
				conn.setSrcGatePlusplus(true);
			else
				conn.setSrcGateIndex(DEFAULT_INDEX);
		
		if(destGate.getIsVector())
			if(destGatePP)
				conn.setDestGatePlusplus(true);
			else
				conn.setDestGateIndex(DEFAULT_INDEX);
		
		// but remove from the model
//		conn.removeFromParent();
		return conn;
	}
	
	/**
	 * Add the provided TemplateConnection to the menu,
	 * @param menu
	 * @param conn
	 */
	protected static void addToMenu(BlockingMenu menu, ConnectionNode conn) {
		MenuItem mi = menu.addMenuItem(SWT.PUSH);
		mi.setData(conn);
		String label = ModelUtil.generateNedSource(conn, false).trim(); 
		mi.setText(label);
	}
	
	/**
	 * This method ask the user which gates should be connected on the source and destination module
	 * @param askForSrcGate
	 * @param askForDestGate
	 */
	public static ConnectionNode askForTemplateConnection(IConnectable srcModule, IConnectable destModule) {
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
				// add the gate names to the menu item as additional widget data
				ConnectionNode conn = createTemplateConnection(
											srcModule, false, srcOut, false, destModule, false, destIn, false);
				addToMenu(menu, conn);
			}
				
		for (GateNode srcInOut : srcInOutModuleGates)
			for(GateNode destInOut : destInOutModuleGates) {
				ConnectionNode conn = createTemplateConnection(
						srcModule, false, srcInOut, false, destModule, false, destInOut, false);
				addToMenu(menu, conn);
			}

		MenuItem selection = menu.open();
		if (selection == null)
			return null;

		return (ConnectionNode)selection.getData();
	}

}
