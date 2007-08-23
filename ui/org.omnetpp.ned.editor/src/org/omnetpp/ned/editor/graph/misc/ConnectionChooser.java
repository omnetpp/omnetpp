package org.omnetpp.ned.editor.graph.misc;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.MenuItem;

import org.omnetpp.ned.editor.graph.commands.ConnectionCommand;
import org.omnetpp.ned.model.NEDTreeUtil;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
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
     * destination module.
     *
     * @param srcModule the source module we are connecting to, should not be NULL
     * @param srcGate which dest module gate should be offered. if NULL, all module gates will be enumerated
     * @param destModule the destination module we are connecting to, should not be NULL
     * @param destGate which dest module gate should be offered. if NULL, all module gates will be enumerated
     * @return TODO what does it return?
     */
    public static ConnectionElement open(ConnectionCommand connCommand) {
        Assert.isNotNull(connCommand.getSrcModule());
        Assert.isNotNull(connCommand.getDestModule());

        List<GateElementEx> srcOutModuleGates = getModuleGates(connCommand.getSrcModule(), GateElement.NED_GATETYPE_OUTPUT, connCommand.getSrcGate());
        List<GateElementEx> srcInOutModuleGates = getModuleGates(connCommand.getSrcModule(), GateElement.NED_GATETYPE_INOUT, connCommand.getSrcGate());
        List<GateElementEx> destInModuleGates = getModuleGates(connCommand.getDestModule(), GateElement.NED_GATETYPE_INPUT, connCommand.getDestGate());
        List<GateElementEx> destInOutModuleGates = getModuleGates(connCommand.getDestModule(), GateElement.NED_GATETYPE_INOUT, connCommand.getDestGate());

        List<ConnectionElement> unusedList = new ArrayList<ConnectionElement>();
        List<ConnectionElement> usedList = new ArrayList<ConnectionElement>();

        // gather unidirectional connections
        for (GateElement srcOut : srcOutModuleGates)
            for (GateElement destIn : destInModuleGates)
                accumlateConnection(connCommand, srcOut, destIn, unusedList, usedList);

        // gather bidirectional connections
        for (GateElement srcInOut : srcInOutModuleGates)
            for (GateElement destInOut : destInOutModuleGates)
                accumlateConnection(connCommand, srcInOut, destInOut, unusedList, usedList);

        BlockingMenu menu = new BlockingMenu(Display.getCurrent().getActiveShell(), SWT.NONE);

        // add the enabled items
        for (ConnectionElement conn : unusedList) {
            createMenuItem(connCommand, menu, conn);
        }

        // add the used disabled items
        for (ConnectionElement conn : usedList) {
            MenuItem mi = createMenuItem(connCommand, menu, conn);
            mi.setEnabled(false);
        }

        MenuItem selection = menu.open();
        if (selection == null)
            return null;

        return (ConnectionElement)selection.getData();
    }

    /**
     * Creates a new menu item from the provided connection template, and adds it to the provided menu.
     * For convenience it returns the created item
     */
    private static MenuItem createMenuItem(ConnectionCommand connCommand, BlockingMenu menu, ConnectionElement conn) {
        MenuItem mi = menu.addMenuItem(SWT.PUSH);
        // store the connection template in the widget's extra data
        mi.setData(conn);
        String label = NEDTreeUtil.generateNedSource(conn, false).trim();
        mi.setText(label);
        return mi;
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
     * Creates a connection template form the parameters and adds is to either the used
     * or unused list. Used connection items should be displayed in disabled state.
     * @param connCommand The command used to specify which module and gates are to be used
     * @param srcGate The source gate used to create the connections
     * @param destGate The source gate used to create the connections
     * @param unusedList Connections that can be chosen
     * @param usedList Connections that are already connected
     */
    private static void accumlateConnection(ConnectionCommand connCommand, GateElement srcGate, GateElement destGate,
                            List<ConnectionElement> unusedList, List<ConnectionElement> usedList) {
        // add the gate names to the menu item as additional widget data
        ConnectionElement conn =  createTemplateConnection(connCommand.getSrcModule(), srcGate,
                                                           connCommand.getDestModule(), destGate);
        if (conn != null) {
            if (isConnectionUnused(connCommand, conn, srcGate, destGate))
                unusedList.add(conn);
            else
                usedList.add(conn);
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
			if (smodNode.getVectorSize()!= null && !"".equals(smodNode.getVectorSize()))
					conn.setSrcModuleIndex(DEFAULT_INDEX);
		} else
			conn.setSrcModule(null);

		if (destMod instanceof SubmoduleElementEx) {
			SubmoduleElementEx dmodNode = (SubmoduleElementEx)destMod;
			conn.setDestModule(dmodNode.getName());
			if (dmodNode.getVectorSize()!= null && !"".equals(dmodNode.getVectorSize()))
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
     * Returns whether the connection is unused, that is, the gates we want to connect
     * are unconnected currently
     */
    private static boolean isConnectionUnused(ConnectionCommand connCommand, ConnectionElement conn, GateElement srcGate, GateElement destGate) {
        CompoundModuleElementEx compModule = connCommand.getParentEditPart().getCompoundModuleModel();
        // note that vector gates or any gate on a submodule vector should be treated always unconnected
        // because the user can connect the connection to different instances/indexes of the gate/submodule
        boolean isSrcSideAVector = srcGate.getIsVector() ||
            (srcGate.getParent().getParent() instanceof SubmoduleElementEx &&
                    !"".equals(((SubmoduleElementEx)srcGate.getParent().getParent()).getVectorSize()));
        boolean isDestSideAVector = destGate.getIsVector() ||
        (destGate.getParent().getParent() instanceof SubmoduleElementEx &&
                !"".equals(((SubmoduleElementEx)destGate.getParent().getParent()).getVectorSize()));

        // if we are inserting a new connection
        if (connCommand.isCreating()) {
            // if there are any connections with the same source or dest gate name, we should return invalid
            if (!compModule.getConnections(conn.getSrcModule(), conn.getSrcGate(), null, null).isEmpty()
                    && !isSrcSideAVector)
                return false;
            if (!compModule.getConnections(null, null, conn.getDestModule(), conn.getDestGate()).isEmpty()
                    && !isDestSideAVector)
                return false;
        }
        // if we are moving the source side connection we should filter only for that side
        if (!isSrcSideAVector && connCommand.isSrcMoving() &&
                !compModule.getConnections(conn.getSrcModule(), conn.getSrcGate(), null, null).isEmpty())
            return false;
        // if we are moving the dest side connection we should filter only for that side
        if (!isDestSideAVector && connCommand.isDestMoving() &&
                !compModule.getConnections(null, null, conn.getDestModule(), conn.getDestGate()).isEmpty())
            return false;
        // the connection can be attached to the gate
        return true;
    }

}
