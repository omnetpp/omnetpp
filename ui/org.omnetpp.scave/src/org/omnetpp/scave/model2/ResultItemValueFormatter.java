/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model2;

import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.scave.engine.EnumType;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.ResultItem.DataType;

public class ResultItemValueFormatter {
    private DataType type;
    private EnumType enumType;

    public ResultItemValueFormatter() {
    }

    public ResultItemValueFormatter(ResultItem resultItem) {
        setResultItem(resultItem);
    }

    public void setResultItem(ResultItem resultItem) {
        if (resultItem != null) {
            type = resultItem.getDataType();
            enumType = resultItem.getEnum();
        }
        else {
            type = null;
            enumType = null;
        }
    }

    public void setResultItem(ResultItemRef resultItemRef) {
        setResultItem(resultItemRef.resolve());
    }

    public String format(double value) {
        if (type == DataType.TYPE_DOUBLE) {
            return String.valueOf(value);
        }
        else if (type == DataType.TYPE_INT) {
            return String.valueOf((int)value);
        }
        else if (type == DataType.TYPE_ENUM && enumType != null) {
            String name = enumType.nameOf((int)value);
            return name != null ? name : "?";
        }
        else
            return "?";
    }

    public String format(BigDecimal value) {
        if (type == DataType.TYPE_DOUBLE) {
            return String.valueOf(value);
        }
        else if (type == DataType.TYPE_INT) {
            return String.valueOf(value);
        }
        else if (type == DataType.TYPE_ENUM && enumType != null) {
            String name = enumType.nameOf((int)value.doubleValue());
            return name != null ? name : "?";
        }
        else
            return "?";
    }
}
