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

//        if (chooseSrc && chooseDest) {
            // gather unidirectional connections
            for (GateElement srcOut : srcOutModuleGates)
                for (GateElement destIn : destInModuleGates)
                    accumlateConnection(srcMod, srcOut, chooseSrc, destMod, destIn, chooseDest, conn, unusedList, usedList);

            // gather bidirectional connections
            for (GateElement srcInOut : srcInOutModuleGates)
                for (GateElement destInOut : destInOutModuleGates)
                    accumlateConnection(srcMod, srcInOut, chooseSrc, destMod, destInOut, chooseDest, conn, unusedList, usedList);
//        }
//        else if (chooseSrc) {
//            // gather unidirectional connections
//            for (GateElement srcOut : srcOutModuleGates)
//                accumlateConnection(srcMod, srcOut, destMod, null, conn, unusedList, usedList);
//
//            // gather bidirectional connections
//            for (GateElement srcInOut : srcInOutModuleGates)
//                accumlateConnection(srcMod, srcInOut, destMod, null, conn, unusedList, usedList);
//        }
//        else if (chooseDest) {
//            // gather unidirectional connections
//            for (GateElement destIn : destInModuleGates)
//                accumlateConnection(srcMod, null, destMod, destIn, conn, unusedList, usedList);
//
//            // gather bidirectional connections
//            for (GateElement destInOut : destInOutModuleGates)
//                accumlateConnection(srcMod, null, destMod, destInOut, conn, unusedList, usedList);
//        }

        BlockingMenu menu = new BlockingMenu(Display.getCurrent().getActiveShell(), SWT.NONE);

        // add the enabled items
        for (ConnectionElement unusedConn : unusedList) {
            createMenuItem(menu, unusedConn);
        }

        // add the used disabled items
        for (ConnectionElement usedConn : usedList) {
            MenuItem mi = createMenuItem(menu, usedConn);
            mi.setEnabled(false);
        }

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
        String label = NEDTreeUtil.generateNedSource(conn, false).trim();
        mi.setText(label);
        return mi;
    }

    /**
     * Creates a connection template form the parameters and adds is to either the used
     * or unused list. Used connection items should be displayed in disabled state.
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
    private static void accumlateConnection(IConnectableElement srcModule, GateElement srcGate, boolean chooseSrc, 
                                            IConnectableElement destModule, GateElement destGate, boolean chooseDest,
                                            ConnectionElementEx connection, 
                                            List<ConnectionElement> unusedList, List<ConnectionElement> usedList) {
        // add the gate names to the menu item as additional widget data
        ConnectionElement templateConn =  createTemplateConnection(srcModule, srcGate, destModule, destGate);
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
//            if (isConnectionUnused(conn, conn, srcGate, destGate))
                unusedList.add(templateConn);
//            else
//                usedList.add(conn);
        }
    }
    /**
	 * Creates a template connection object from the provided gates and modules.
	 * If the module is a vector, it uses module[0] syntax
	 * If the gate is a vector uses either gate[0] or gate++ syntax depending whether the gate size was specified.
	 * If the specified gates are incompatible (eg: labels do not match) it returns <code>null</code>
	 */
	private static ConnectionElement createTemplateConnection(
						IConnectableElement srcMod, GateElement srcGate,
						IConnectableElement destMod, GateElement destGate) {

		ConnectionElement conn = (ConnectionElement)NEDElementFactoryEx.getInstance().createElement(NEDElementTags.NED_CONNECTION);
		// set the source and dest module names.
		// if compound module, name must be empty
		// for Submodules name must be the submodule name
		if (srcMod instanceof SubmoduleElementEx) {
			SubmoduleElementEx smodNode = (SubmoduleElementEx)srcMod;
			conn.setSrcModule(smodNode.getName());
			if (StringUtils.isNotBlank(smodNode.getVectorSize()))
					conn.setSrcModuleIndex(DEFAULT_INDEX);
		} else
			conn.setSrcModule(null);

		if (destMod instanceof SubmoduleElementEx) {
			SubmoduleElementEx dmodNode = (SubmoduleElementEx)destMod;
			conn.setDestModule(dmodNode.getName());
			if (StringUtils.isNotBlank(dmodNode.getVectorSize()))
					conn.setDestModuleIndex(DEFAULT_INDEX);
		} else
			conn.setDestModule(null);

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

    /**
     * Returns whether the connection is unused, that is, the gates we want to connect
     * are unconnected currently
     */
//    private static boolean isConnectionUnused(ConnectionCommand connCommand, ConnectionElement conn, GateElement srcGate, GateElement destGate) {
//        // note that vector gates or any gate on a submodule vector should be treated always unconnected
//        // because the user can connect the connection to different instances/indexes of the gate/submodule
//        boolean isSrcSideAVector = srcGate.getIsVector() ||
//            (srcGate.getParent().getParent() instanceof SubmoduleElementEx &&
//                    !"".equals(((SubmoduleElementEx)srcGate.getParent().getParent()).getVectorSize()));
//        boolean isDestSideAVector = destGate.getIsVector() ||
//        (destGate.getParent().getParent() instanceof SubmoduleElementEx &&
//                !"".equals(((SubmoduleElementEx)destGate.getParent().getParent()).getVectorSize()));
//
//        // if we are inserting a new connection
//        CompoundModuleElementEx compModule = connCommand.getCompoundModuleParent();
//        if (connCommand instanceof CreateConnectionCommand) {
//            // if there are any connections with the same source or dest gate name, we should return invalid
//            if (!compModule.getConnections(conn.getSrcModule(), conn.getSrcGate(), null, null).isEmpty()
//                    && !isSrcSideAVector)
//                return false;
//            if (!compModule.getConnections(null, null, conn.getDestModule(), conn.getDestGate()).isEmpty()
//                    && !isDestSideAVector)
//                return false;
//        }
//        else if (connCommand instanceof ReconnectCommand){
//            // if we are moving the source side connection we should filter only for that side
//            if (!isSrcSideAVector && connCommand.getSrcGate() == null &&
//                    !compModule.getConnections(conn.getSrcModule(), conn.getSrcGate(), null, null).isEmpty())
//                return false;
//            // if we are moving the dest side connection we should filter only for that side
//            if (!isDestSideAVector && connCommand.getDestGate() == null &&
//                    !compModule.getConnections(null, null, conn.getDestModule(), conn.getDestGate()).isEmpty())
//                return false;
//        }
//        // the connection can be attached to the gate
//        return true;
//    }

}
