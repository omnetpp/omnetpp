package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.core.runtime.Assert;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.pojo.ConnectionElement;

/**
 * (Re)assigns a Connection to srcModule/destModule sub/compound module gates and also adds it to the
 * model (to the compound module's connections section) (or removes it if both the new source and destination is NULL)
 *
 * @author rhornig
 */
// TODO handling of subgates $i and $o is missing
//FIXME import the channel type used: see CreateSubmoduleCommand and NEDElementUtilEx.addImportFor()
public class ReconnectCommand extends ConnectionCommand {
    /**
     * Create, delete, or modify  a connection element
     * @param conn Connection Model
     * @param compoundModuleElement Connection's container (compound module) 
     */
    public ReconnectCommand(ConnectionElementEx conn, CompoundModuleElementEx compoundModuleElement) {
        super(conn, compoundModuleElement);
        setLabel("Reconnect");
    }

    @Override
    public void redo() {
        Assert.isTrue(getSrcModule() != null || getDestModule() != null, "Modules cannot be empty");
        Assert.isTrue(StringUtils.isNotBlank(getSrcGate()) && StringUtils.isNotBlank(getDestGate()));

        copyConn(newConn, modelConn);
        modelConn.setSrcModuleRef(srcModule);
        modelConn.setDestModuleRef(destModule);
    }

    @Override
    public void undo() {
        copyConn(originalConn, modelConn);
        modelConn.setSrcModuleRef(originalSrcModule);
        modelConn.setDestModuleRef(originalDestModule);
    }

	/**
	 * Utility method to copy base connection properties from one
	 * connectionNode to the other (except the module name)
	 * @param from
	 * @param to
	 */
	public static void copyConn(ConnectionElement from, ConnectionElement to) {
	    to.setSrcModule(from.getSrcModule());
		to.setSrcModuleIndex(from.getSrcModuleIndex());
        to.setSrcGate(from.getSrcGate());
        to.setSrcGateIndex(from.getSrcGateIndex());
        to.setSrcGatePlusplus(from.getSrcGatePlusplus());
        to.setSrcGateSubg(from.getSrcGateSubg());

        to.setDestModule(from.getDestModule());
        to.setDestModuleIndex(from.getDestModuleIndex());
        to.setDestGate(from.getDestGate());
        to.setDestGateIndex(from.getDestGateIndex());
        to.setDestGatePlusplus(from.getDestGatePlusplus());
        to.setDestGateSubg(from.getDestGateSubg());

        to.setArrowDirection(from.getArrowDirection());
	}

}
