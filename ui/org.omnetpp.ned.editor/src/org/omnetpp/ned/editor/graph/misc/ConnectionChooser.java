package org.omnetpp.ned.editor.graph.misc;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.MenuItem;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.NEDTreeUtil;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.GateElementEx;
import org.omnetpp.ned.model.ex.NEDElementFactoryEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IConnectableElement;
import org.omnetpp.ned.model.pojo.ConnectionElement;
import org.omnetpp.ned.model.pojo.GateElement;
import org.omnetpp.ned.model.pojo.NEDElementTags;

/**
 * Helper class that allows to choose a connection for a module pair (src, dest) via a Popup menu
 *
 * @author rhornig
 */
public class ConnectionChooser {
    private static final String DEFAULT_INDEX = "0";

    /**
     * This method asks the user which gates should be connected on the source and
     * destination module during connection creation. Returns a filled in connection element where
     * values can be copied from or NULL if the user has cancelled the selection popup menu.
     */
    public static ConnectionElementEx open(CompoundModuleElementEx compound, ConnectionElementEx conn, boolean chooseSrc, boolean chooseDest) {
        Assert.isNotNull(compound);
        Assert.isNotNull(conn);
        
        IConnectableElement srcMod = StringUtils.isNotEmpty(conn.getSrcModule()) ? 
                compound.getSubmoduleByName(conn.getSrcModule()) : compound;
        IConnectableElement destMod = StringUtils.isNotEmpty(conn.getDestModule()) ? 
                        compound.getSubmoduleByName(conn.getDestModule()) : compound;
        
        Assert.isNotNull(srcMod);
        Assert.isNotNull(destMod);

        // we either get all gates with a given type, or we filter with the name and have only a single one returned
        List<GateElementEx> srcOutModuleGates = getModuleGates(srcMod, GateElement.NED_GATETYPE_OUTPUT, chooseSrc ? null : conn.getSrcGate());
        List<GateElementEx> srcInOutModuleGates = getModuleGates(srcMod, GateElement.NED_GATETYPE_INOUT, chooseSrc ? null : conn.getSrcGate());
        List<GateElementEx> destInModuleGates = getModuleGates(destMod, GateElement.NED_GATETYPE_INPUT, chooseDest ? null : conn.getDestGate());
        List<GateElementEx> destInOutModuleGates = getModuleGates(destMod, GateElement.NED_GATETYPE_INOUT, chooseDest ? null : conn.getDestGate());

        List<ConnectionElement> unusedList = new ArrayList<ConnectionElement>();
        List<ConnectionElement> usedList = new ArrayList<ConnectionElement>();

        // gather unidirectional connections
        for (GateElement srcOut : srcOutModuleGates)
            for (GateElement destIn : destInModuleGates)
                accumulateConnection(compound, conn, srcMod, srcOut, chooseSrc, destMod, destIn, chooseDest, unusedList, usedList);

        // gather bidirectional connections
        for (GateElement srcInOut : srcInOutModuleGates)
            for (GateElement destInOut : destInOutModuleGates)
                accumulateConnection(compound, conn, srcMod, srcInOut, chooseSrc, destMod, destInOut, chooseDest, unusedList, usedList);

        BlockingMenu menu = new BlockingMenu(Display.getCurrent().getActiveShell(), SWT.NONE);

        // add the enabled items
        for (ConnectionElement unusedConn : unusedList) {
            createMenuItem(menu, unusedConn);
        }

        List<String> noGates = new ArrayList<String>();
        if (srcMod.getGateDeclarations().size() == 0)   
            noGates.add(srcMod.getName());
        if (destMod.getGateDeclarations().size() == 0)   
            noGates.add(destMod.getName());
        if (noGates.size() > 0) {
            MenuItem mi = menu.addMenuItem(SWT.PUSH);
            mi.setText(StringUtils.join(noGates, " and ")+ (noGates.size() > 1 ? " have no gates" : " has no gates"));
            mi.setEnabled(false);
        }
        
        // add the used disabled items
        for (ConnectionElement usedConn : usedList) {
            MenuItem mi = createMenuItem(menu, usedConn);
            mi.setEnabled(false);
        }

        // Note: the following code was an attempt to move the mouse over the menu right away,
        // but it does not work: it causes the first menu item to get accepted automatically
        //Point loc = Display.getCurrent().getCursorLocation();
        //Display.getCurrent().setCursorLocation(loc.x+5, loc.y+5);
        
        MenuItem selection = menu.open();
        if (selection == null)
            return null;

        return (ConnectionElementEx)selection.getData();
    }

    /**
     * Creates a new menu item from the provided connection template, and adds it to the provided menu.
     * For convenience it returns the created item
     */
    private static MenuItem createMenuItem(BlockingMenu menu, ConnectionElement conn) {
        MenuItem mi = menu.addMenuItem(SWT.PUSH);
        // store the connection template in the widget's extra data
        mi.setData(conn);
        String label = StringUtils.removeEnd(NEDTreeUtil.generateNedSource(conn, false).trim(), ";");
        mi.setText(label);
        return mi;
    }

    /**
     * Creates a connection template form the parameters and adds is to either the used
     * or unused list. Used connection items should be displayed in disabled state.
     * @param compound The compound module in which the current connection is/will be present
     * @param srcModule The source module used to create the connections
     * @param srcGate The source gate used to create the connections
     * @param chooseSrc Whether we want to choose from source gates (if not, the dest part will be copied from "connection") 
     * @param destModule The destination module used to create the connections
     * @param destGate The destination gate used to create the connections
     * @param chooseSrc Whether we want to choose from dest gates (if not, the dest part will be copied from "connection")  
     * @param connection the connection which is being modified (if one side should be unchanged ie one of srcGate or destGate is NULL)
     * @param unusedList Connections that can be chosen
     * @param usedList Connections that are already connected
     */
    private static void accumulateConnection(CompoundModuleElementEx compound, ConnectionElementEx connection,
                                            IConnectableElement srcModule, GateElement srcGate, boolean chooseSrc, 
                                            IConnectableElement destModule, GateElement destGate, boolean chooseDest,
                                            List<ConnectionElement> unusedList, List<ConnectionElement> usedList) {
        // add the gate names to the menu item as additional widget data
        ConnectionElementEx templateConn =  createTemplateConnection(srcModule, srcGate, destModule, destGate);
        // we dont want to change src so we should use the current one from the model (connection)
        if (!chooseSrc) {
            templateConn.setSrcModule(connection.getSrcModule());
            templateConn.setSrcModuleIndex(connection.getSrcModuleIndex());
            templateConn.setSrcGate(connection.getSrcGate());
            templateConn.setSrcGateIndex(connection.getSrcGateIndex());
            templateConn.setSrcGatePlusplus(connection.getSrcGatePlusplus());
            templateConn.setSrcGateSubg(connection.getSrcGateSubg());
        }
        if (!chooseDest) {
            templateConn.setDestModule(connection.getDestModule());
            templateConn.setDestModuleIndex(connection.getDestModuleIndex());
            templateConn.setDestGate(connection.getDestGate());
            templateConn.setDestGateIndex(connection.getDestGateIndex());
            templateConn.setDestGatePlusplus(connection.getDestGatePlusplus());
            templateConn.setDestGateSubg(connection.getDestGateSubg());
        }
        
        if (templateConn != null) {
            if (isConnectionUnused(compound, templateConn, srcModule, srcGate, chooseSrc, destModule, destGate, chooseDest))
                unusedList.add(templateConn);
            else
                usedList.add(templateConn);
        }
    }
    /**
	 * Creates a template connection object from the provided gates and modules.
	 * If the module is a vector, it uses module[0] syntax
	 * If the gate is a vector uses either gate[0] or gate++ syntax depending whether the gate size was specified.
	 * If the specified gates are incompatible (eg: labels do not match) it returns <code>null</code>
	 */
	private static ConnectionElementEx createTemplateConnection(
						IConnectableElement srcMod, GateElement srcGate,
						IConnectableElement destMod, GateElement destGate) {

		ConnectionElementEx conn = (ConnectionElementEx)NEDElementFactoryEx.getInstance().createElement(NEDElementTags.NED_CONNECTION);
		// set the source and dest module names.
		// if compound module, name must be empty
		// for Submodules name must be the submodule name
		if (srcMod instanceof SubmoduleElementEx) {
			SubmoduleElementEx smodNode = (SubmoduleElementEx)srcMod;
			conn.setSrcModule(smodNode.getName());
			if (StringUtils.isNotBlank(smodNode.getVectorSize()))
					conn.setSrcModuleIndex(DEFAULT_INDEX);
		} else
			conn.setSrcModule("");

		if (destMod instanceof SubmoduleElementEx) {
			SubmoduleElementEx dmodNode = (SubmoduleElementEx)destMod;
			conn.setDestModule(dmodNode.getName());
			if (StringUtils.isNotBlank(dmodNode.getVectorSize()))
					conn.setDestModuleIndex(DEFAULT_INDEX);
		} else
			conn.setDestModule("");

		// set the possible gates
		conn.setSrcGate(srcGate.getName());
		conn.setDestGate(destGate.getName());
		// if both side is bidirectional gate, use a bidirectional connection
		if (srcGate.getType() == GateElement.NED_GATETYPE_INOUT && destGate.getType() == GateElement.NED_GATETYPE_INOUT)
			conn.setArrowDirection(ConnectionElement.NED_ARROWDIR_BIDIR);
		else
			conn.setArrowDirection(ConnectionElement.NED_ARROWDIR_L2R);

		// check if we have a module vector and add an index to it.
		if (srcGate.getIsVector())
			if (srcMod.getGateSizes().containsKey(srcGate.getName()))
			    conn.setSrcGateIndex(DEFAULT_INDEX);
			else
			    conn.setSrcGatePlusplus(true);

		if (destGate.getIsVector())
			if (destMod.getGateSizes().containsKey(destGate.getName()))
			    conn.setDestGateIndex(DEFAULT_INDEX);
			else
			    conn.setDestGatePlusplus(true);

		return conn;
	}

    /**
     * Returns gates with the given type (in, out, inout) of the given compound module
     * or submodule. If nameFilter is present (not null), the gate with that name is returned.
     */
    private static List<GateElementEx> getModuleGates(IConnectableElement module, int gateType, String nameFilter) {
        List<GateElementEx> result = new ArrayList<GateElementEx>();

        if (module instanceof CompoundModuleElementEx) {
            // if we connect a compound module, swap the gate type (in<>out) submodule.out -> out
            if (gateType == GateElement.NED_GATETYPE_INPUT)
                gateType = GateElement.NED_GATETYPE_OUTPUT;
            else if (gateType == GateElement.NED_GATETYPE_OUTPUT)
                gateType = GateElement.NED_GATETYPE_INPUT;
        }

        for (GateElementEx gate: module.getGateDeclarations().values())
            if (gate.getType() == gateType)
                if (nameFilter == null || nameFilter.equals(gate.getName()))
                    result.add(gate);
        return result;
    }

    private static boolean isConnectionUnused(CompoundModuleElementEx compound, ConnectionElementEx conn,
                                              IConnectableElement srcMod, GateElement srcGate, boolean chooseSrc,
                                              IConnectableElement destMod, GateElement destGate, boolean chooseDest) {
        // note that vector gates or any gate on a submodule vector should be treated always unconnected
        // because the user can connect the connection to different instances/indexes of the gate/submodule
        boolean isSrcSideAVector = srcGate.getIsVector() ||
                (srcMod instanceof SubmoduleElementEx &&
                StringUtils.isNotEmpty(((SubmoduleElementEx)srcMod).getVectorSize()));
        boolean isDestSideAVector = destGate.getIsVector() ||
                (destMod instanceof SubmoduleElementEx &&
                StringUtils.isNotEmpty(((SubmoduleElementEx)destMod).getVectorSize()));

        boolean isSrcFree = true;
        if (chooseSrc) 
            isSrcFree = isSrcSideAVector || 
            (compound.getConnections(conn.getSrcModule(), conn.getSrcGate(), null, null).isEmpty() &&
             compound.getConnections(null, null, conn.getSrcModule(), conn.getSrcGate()).isEmpty());
        
        boolean isDestFree = true;
        if (chooseDest) 
            isDestFree = isDestSideAVector || 
            (compound.getConnections(conn.getDestModule(), conn.getDestGate(), null, null).isEmpty() &&
             compound.getConnections(null, null, conn.getDestModule(), conn.getDestGate()).isEmpty());
        
        return isSrcFree && isDestFree;
    }
    

}
