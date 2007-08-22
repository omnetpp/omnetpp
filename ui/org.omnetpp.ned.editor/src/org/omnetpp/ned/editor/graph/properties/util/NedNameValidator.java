package org.omnetpp.ned.editor.graph.properties.util;

import java.util.Arrays;

import org.eclipse.jface.viewers.ICellEditorValidator;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.NEDElementConstants;
import org.omnetpp.ned.model.ex.NEDElementUtilEx;
import org.omnetpp.ned.model.interfaces.IHasName;

/**
 * Generic name validator used for checking whether the name is not empty and contains only allowed characters
 * and not a reserved keyword.
 * @author rhornig
 */
public class NedNameValidator implements ICellEditorValidator {
    String originalName;

    public NedNameValidator(IHasName element) {
        originalName = element.getName();
    }

    public String isValid(Object value) {
        if (StringUtils.isBlank((String)value))
            return "Invalid identifier. Names must not be empty";

        if (!NEDElementUtilEx.isValidIdentifier((String)value))
            return "Invalid identifier. Names must begin with a letter or underscore, and may contain letters," +
            " digits or underscore.";

        if (Arrays.asList(NEDElementConstants.RESERVED_NED_KEYWORDS).contains(value))
            return "Name is a reserved keyword.";
        return null;
    }

}
