/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.properties.util;

import java.util.Arrays;

import org.eclipse.jface.viewers.ICellEditorValidator;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.NedElementConstants;
import org.omnetpp.ned.model.ex.NedElementUtilEx;
import org.omnetpp.ned.model.interfaces.IHasName;

/**
 * Generic name validator used for checking whether the name is not empty and contains only allowed characters
 * and not a reserved keyword.
 * @author rhornig
 */
public class NedNameValidator implements ICellEditorValidator {
    protected IHasName element;

    public NedNameValidator(IHasName element) {
        this.element = element;
    }

    public String isValid(Object value) {
        if (StringUtils.isBlank((String)value))
            return "Invalid identifier. Names must not be empty";

        if (!NedElementUtilEx.isValidIdentifier((String)value))
            return "Invalid identifier. Names must begin with a letter or underscore, and may contain letters," +
            " digits or underscore.";

        if (Arrays.asList(NedElementConstants.RESERVED_NED_KEYWORDS).contains(value))
            return "Name is a reserved keyword.";
        return null;
    }

}
