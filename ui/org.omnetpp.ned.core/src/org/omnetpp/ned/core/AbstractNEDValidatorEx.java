package org.omnetpp.ned.core;

import org.omnetpp.ned.model.NEDElementConstants;
import org.omnetpp.ned.model.ex.ChannelInterfaceNodeEx;
import org.omnetpp.ned.model.ex.ChannelNodeEx;
import org.omnetpp.ned.model.ex.CompoundModuleNodeEx;
import org.omnetpp.ned.model.ex.ConnectionNodeEx;
import org.omnetpp.ned.model.ex.ModuleInterfaceNodeEx;
import org.omnetpp.ned.model.ex.NedFileNodeEx;
import org.omnetpp.ned.model.ex.SimpleModuleNodeEx;
import org.omnetpp.ned.model.ex.SubmoduleNodeEx;
import org.omnetpp.ned.model.pojo.AbstractNEDValidator;
import org.omnetpp.ned.model.pojo.ChannelInterfaceNode;
import org.omnetpp.ned.model.pojo.ChannelNode;
import org.omnetpp.ned.model.pojo.CompoundModuleNode;
import org.omnetpp.ned.model.pojo.ConnectionNode;
import org.omnetpp.ned.model.pojo.ModuleInterfaceNode;
import org.omnetpp.ned.model.pojo.NedFileNode;
import org.omnetpp.ned.model.pojo.SimpleModuleNode;
import org.omnetpp.ned.model.pojo.SubmoduleNode;

/**
 * Validation using the extended ("Ex") classes
 *
 * @author andras
 */
public abstract class AbstractNEDValidatorEx extends AbstractNEDValidator implements NEDElementConstants {
    protected abstract void validateElement(NedFileNodeEx node);
    protected abstract void validateElement(SimpleModuleNodeEx node);
    protected abstract void validateElement(ModuleInterfaceNodeEx node);
    protected abstract void validateElement(CompoundModuleNodeEx node);
    protected abstract void validateElement(ChannelInterfaceNodeEx node);
    protected abstract void validateElement(ChannelNodeEx node);
    protected abstract void validateElement(SubmoduleNodeEx node);
    protected abstract void validateElement(ConnectionNodeEx node);

	@Override
    final protected void validateElement(NedFileNode node) {
		validateElement((NedFileNodeEx)node);
	}

	@Override
	final protected void validateElement(ModuleInterfaceNode node) {
		validateElement((ModuleInterfaceNodeEx)node);
	}

	@Override
	final protected void validateElement(SimpleModuleNode node) {
		validateElement((SimpleModuleNodeEx)node);
	}

	@Override
	final protected void validateElement(CompoundModuleNode node) {
		validateElement((CompoundModuleNodeEx)node);
	}

	@Override
	final protected void validateElement(ChannelInterfaceNode node) {
		validateElement((ChannelInterfaceNodeEx)node);
	}

	@Override
	final protected void validateElement(ChannelNode node) {
		validateElement((ChannelNodeEx)node);
	}

	@Override
	final protected void validateElement(SubmoduleNode node) {
		validateElement((SubmoduleNodeEx)node);
	}
	
	@Override
	final protected void validateElement(ConnectionNode node) {
		validateElement((ConnectionNode)node);
	}

}
