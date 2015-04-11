/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.properties.util;

import java.util.List;

import org.eclipse.jface.viewers.ICellEditorValidator;
import org.omnetpp.common.util.StringUtils;

/**
 * Validator used for checking type,likeType and extends properties. This checks
 * whether the entered type name is present and valid.
 */
public class ExistingTypeNameValidator implements ICellEditorValidator {
    List<String> values;
    public ExistingTypeNameValidator(List<String> values) {
        this.values = values;
    }

    public String isValid(Object newText) {
        // accept empty type names too
        if (StringUtils.isEmpty((String)newText))
            return null;

        if (!values.contains(newText) )
            return "Type name is not available. Value is not stored.";

        return null;
    }
}