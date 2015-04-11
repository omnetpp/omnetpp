/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.properties.util;

import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;

/**
 * Validator used for checking submodule full names.
 */
public class SubmoduleFullNameValidator extends SubmoduleNameValidator {
    CompoundModuleElementEx compoundModuleModel;

    public SubmoduleFullNameValidator(SubmoduleElementEx element) {
        super(element);
        this.compoundModuleModel = element.getCompoundModule();
    }

    @Override
    public String isValid(Object newValue) {
        String newText = (String)newValue;
        int openIndex = newText.indexOf("[");
        int closeIndex = newText.indexOf("]");
        if (openIndex != -1 || closeIndex != -1) {
            if (openIndex != -1 && closeIndex != -1 && closeIndex > openIndex + 1)
                return super.isValid(newText.substring(0, openIndex));
            else
                return "Incorrect submodule index format";
        }
        else
            return super.isValid(newValue);
    }
}