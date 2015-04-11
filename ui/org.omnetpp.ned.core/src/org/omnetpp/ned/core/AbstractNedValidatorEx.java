/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.core;

import org.omnetpp.ned.model.NedElementConstants;
import org.omnetpp.ned.model.ex.ChannelInterfaceElementEx;
import org.omnetpp.ned.model.ex.ChannelElementEx;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.ModuleInterfaceElementEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.ex.SimpleModuleElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.pojo.AbstractNedValidator;
import org.omnetpp.ned.model.pojo.ChannelInterfaceElement;
import org.omnetpp.ned.model.pojo.ChannelElement;
import org.omnetpp.ned.model.pojo.CompoundModuleElement;
import org.omnetpp.ned.model.pojo.ConnectionElement;
import org.omnetpp.ned.model.pojo.ModuleInterfaceElement;
import org.omnetpp.ned.model.pojo.NedFileElement;
import org.omnetpp.ned.model.pojo.SimpleModuleElement;
import org.omnetpp.ned.model.pojo.SubmoduleElement;

/**
 * Validation using the extended ("Ex") classes
 *
 * @author andras
 */
public abstract class AbstractNedValidatorEx extends AbstractNedValidator implements NedElementConstants {
    protected abstract void validateElement(NedFileElementEx node);
    protected abstract void validateElement(SimpleModuleElementEx node);
    protected abstract void validateElement(ModuleInterfaceElementEx node);
    protected abstract void validateElement(CompoundModuleElementEx node);
    protected abstract void validateElement(ChannelInterfaceElementEx node);
    protected abstract void validateElement(ChannelElementEx node);
    protected abstract void validateElement(SubmoduleElementEx node);
    protected abstract void validateElement(ConnectionElementEx node);

    @Override
    final protected void validateElement(NedFileElement node) {
        validateElement((NedFileElementEx)node);
    }

    @Override
    final protected void validateElement(ModuleInterfaceElement node) {
        validateElement((ModuleInterfaceElementEx)node);
    }

    @Override
    final protected void validateElement(SimpleModuleElement node) {
        validateElement((SimpleModuleElementEx)node);
    }

    @Override
    final protected void validateElement(CompoundModuleElement node) {
        validateElement((CompoundModuleElementEx)node);
    }

    @Override
    final protected void validateElement(ChannelInterfaceElement node) {
        validateElement((ChannelInterfaceElementEx)node);
    }

    @Override
    final protected void validateElement(ChannelElement node) {
        validateElement((ChannelElementEx)node);
    }

    @Override
    final protected void validateElement(SubmoduleElement node) {
        validateElement((SubmoduleElementEx)node);
    }

    @Override
    final protected void validateElement(ConnectionElement node) {
        validateElement((ConnectionElementEx)node);
    }

}
