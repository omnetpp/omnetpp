package org.omnetpp.ned.editor.graph.misc;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.MenuItem;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.NEDTreeUtil;
import org.omnetpp.ned2.model.ex.CompoundModuleNodeEx;
import org.omnetpp.ned2.model.ex.NEDElementFactoryEx;
import org.omnetpp.ned2.model.ex.SubmoduleNodeEx;
import org.omnetpp.ned2.model.interfaces.IHasConnections;
import org.omnetpp.ned2.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned2.model.pojo.ConnectionNode;
import org.omnetpp.ned2.model.pojo.GateNode;
import org.omnetpp.ned2.model.pojo.NEDElementTags;

/**
 * @author rhornig
 * Helper class that allows to choose a connection for a mosule pair (src , dest)
 */
public class ConnectionChooser {
    private static final String DEFAULT_INDEX = "0";

	/**
	 * @param module The queried module
	 * @param gateType The type of the gate we are lookin for
	 * @param nameFilter name filter, or NULL if no filtering is needed
	 * @return All gates from the module matching the type and name
	 */
	protected static List<GateNode> getModuleGates(IHasConnections module, int gateType, String nameFilter) {
		List<GateNode> result = new ArrayList<GateNode>();
		
        INEDTypeInfo comp = null;

        if (module instanceof CompoundModuleNodeEx) {
            comp = ((CompoundModuleNodeEx)module).getContainerNEDTypeInfo();

            // if we connect a compound module swap the gate type (in<>out) submodule.out -> out
            if (gateType == GateNode.NED_GATETYPE_INPUT)
                gateType = GateNode.NED_GATETYPE_OUTPUT;
            else if (gateType == GateNode.NED_GATETYPE_OUTPUT)
                gateType = GateNode.NED_GATETYPE_INPUT;
            else 
                gateType = GateNode.NED_GATETYPE_INOUT;
		}

        if (module instanceof SubmoduleNodeEx) {
            comp = ((SubmoduleNodeEx)module).getTypeNEDTypeInfo();
		}
		
		if (comp == null)
			return result;
		
		for(NEDElement elem: comp.getGates().values()) {
			GateNode currGate = (GateNode)elem;
			if (currGate.getType() == gateType)
				if (nameFilter == null || nameFilter.equals(currGate.getName()))
                        result.add(currGate);
		}

		return result;
	}

    /**
	 * Creates a template connection object from the provided gates and modules. 
	 * If the module is a vector, it uses module[0] syntax
	 * If the gate is a vector uses either gate[0] or gate++ syntax depending on the srcGatePP, destGatePP parameters.
	 * If the gatePP is set to <code>true</code> but none of the gates are vector, it returns <code>null</code>     
	 * 
	 * @param srcMod
	 * @param srcGate
	 * @param destMod
	 * @param destGate
	 * @param gatePP 	if set to <code>true</code> creates gatename++ (only for vectore gates)
	 * @return The template connection or <code>null</code> if connection cannot be created
	 */
	protected static ConnectionNode createTemplateConnection(
						IHasConnections srcMod, GateNode srcGate,
						IHasConnections destMod, GateNode destGate, boolean gatePP) {

		// check if at least one of the gates are vector if gatePP (gate++) syntax requested
		if (gatePP && !srcGate.getIsVector() && !destGate.getIsVector())
			return null;
		
		ConnectionNode conn = (ConnectionNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NEDElementTags.NED_CONNECTION);
		// set the source and dest module names.
		// if compound module, name must be empty
		// for Submodules name must be the submodulename
		if(srcMod instanceof SubmoduleNodeEx) {
			SubmoduleNodeEx smodNode = (SubmoduleNodeEx)srcMod;
			conn.setSrcModule(smodNode.getName());
			if (smodNode.getVectorSize()!= null && !"".equals(smodNode.getVectorSize()))
					conn.setSrcModuleIndex(DEFAULT_INDEX);
		} else
			conn.setSrcModule(null);

		if(destMod instanceof SubmoduleNodeEx) {
			SubmoduleNodeEx dmodNode = (SubmoduleNodeEx)destMod;
			conn.setDestModule(dmodNode.getName());
			if (dmodNode.getVectorSize()!= null && !"".equals(dmodNode.getVectorSize()))
					conn.setDestModuleIndex(DEFAULT_INDEX);
		} else
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
			if(gatePP)
				conn.setSrcGatePlusplus(true);
			else
				conn.setSrcGateIndex(DEFAULT_INDEX);
		
		if(destGate.getIsVector())
			if(gatePP)
				conn.setDestGatePlusplus(true);
			else
				conn.setDestGateIndex(DEFAULT_INDEX);
		
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
		String label = NEDTreeUtil.generateNedSource(conn, false).trim(); 
		mi.setText(label);
	}
	
	
	// TODO implement popupmenu if only one of the srcModule or destModule is present 
	// only one side of the connection should be selected
	// TODO if on,ly one selection is possible do not offer a menu, but select that connection immediately
	// TODO show which gates are already connected (do not offer those gates)
	/**
	 * This method ask the user which gates should be connected on the source and destination module
	 * @param srcModule the source module we are connecting to, should not be NULL
	 * @param srcGate which dest module gate should be offered. if NULL, all module gates wil be enumerated 
	 * @param destModule the destination module we are connecting to, should not be NULL
	 * @param destGate which dest module gate should be offered. if NULL, all module gates wil be enumerated
	 * @return
	 */
	public static ConnectionNode open(IHasConnections srcModule, String srcGate, IHasConnections destModule, String destGate) {
        Assert.isNotNull(srcModule);
        Assert.isNotNull(destModule);
		
        List<GateNode> srcOutModuleGates = getModuleGates(srcModule, GateNode.NED_GATETYPE_OUTPUT, srcGate);
        List<GateNode> srcInOutModuleGates = getModuleGates(srcModule, GateNode.NED_GATETYPE_INOUT, srcGate);
        List<GateNode> destInModuleGates = getModuleGates(destModule, GateNode.NED_GATETYPE_INPUT, destGate);
        List<GateNode> destInOutModuleGates = getModuleGates(destModule, GateNode.NED_GATETYPE_INOUT, destGate);
        
		BlockingMenu menu = new BlockingMenu(Display.getCurrent().getActiveShell(), SWT.NONE);
		
		for (GateNode srcOut : srcOutModuleGates)
			for(GateNode destIn : destInModuleGates) {
				// add the gate names to the menu item as additional widget data
				ConnectionNode conn = 
						createTemplateConnection(srcModule, srcOut, destModule, destIn, false);
				if (conn != null) addToMenu(menu, conn);

				// try to add a connection with gate++ syntax too
				conn = createTemplateConnection(srcModule, srcOut, destModule, destIn, true);
				if (conn != null) addToMenu(menu, conn);
			}
				
		for (GateNode srcInOut : srcInOutModuleGates)
			for(GateNode destInOut : destInOutModuleGates) {
				ConnectionNode conn = 
						createTemplateConnection(srcModule, srcInOut, destModule, destInOut, false);
				if (conn != null) addToMenu(menu, conn);

				// try to add a connection with gate++ syntax too
				conn = createTemplateConnection(srcModule, srcInOut, destModule, destInOut, true);
				if (conn != null) addToMenu(menu, conn);
			}

		MenuItem selection = menu.open();
		if (selection == null)
			return null;

		return (ConnectionNode)selection.getData();
	}

}
