/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.model.INedElement;

/**
 * Sets an attribute of a NED element. Useful as a building block for
 * higher level compound commands.
 *
 * @author andras
 */
public class SetAttributeCommand extends Command {
    private final INedElement element;
    private final String attribute;
    private final String value;
    private String oldValue;

    public SetAttributeCommand(INedElement element, String attribute, String value) {
        this.element = element;
        this.attribute = attribute;
        this.value = value;
        setLabel("Modify");
    }

    @Override
    public void execute() {
        oldValue = element.getAttribute(attribute);
        redo();
    }

    @Override
    public void redo() {
        element.setAttribute(attribute, value);
    }

    @Override
    public void undo() {
        element.setAttribute(attribute, oldValue);
    }

}
