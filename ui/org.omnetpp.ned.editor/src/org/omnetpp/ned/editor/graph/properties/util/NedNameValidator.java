/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.properties.util;

import java.util.Arrays;

import org.eclipse.jface.viewers.ICellEditorValidator;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.NedElementConstants;
import org.omnetpp.ned.model.ex.NedElementUtilEx;

/**
 * Generic name validator used for checking whether the name is not empty and contains only allowed characters
 * and not a reserved keyword.
 *
 * @author rhornig, andras (cleanup)
 */
public class NedNameValidator implements ICellEditorValidator {
    public String isValid(Object value) {
        if (StringUtils.isBlank((String)value))
            return "Invalid identifier: Name must not be empty";

        if (Arrays.asList(NedElementConstants.RESERVED_NED_KEYWORDS).contains(value)) // must check this first, because NedElementUtilEx.isValidIdentifier() also checks this
            return "Invalid identifier: '" + value + "' is a reserved keyword.";

        if (!NedElementUtilEx.isValidIdentifier((String)value))
            return "Invalid identifier: Name must begin with a letter or underscore, and may contain letters," +
            " digits or underscore";

        return null;
    }

}
