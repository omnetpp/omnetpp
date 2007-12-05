package org.omnetpp.scave.model2;

import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.scave.engine.EnumType;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.ResultItem.Type;

public class ResultItemValueFormatter {
	private Type type;
	private EnumType enumType;
	
	public ResultItemValueFormatter() {
	}
	
	public ResultItemValueFormatter(ResultItem resultItem) {
		setResultItem(resultItem);
	}
	
	public void setResultItem(ResultItem resultItem) {
		if (resultItem != null) {
			type = resultItem.getType();
			enumType = resultItem.getEnum();
		}
		else {
			type = null;
			enumType = null;
		}
	}
	
	public String format(double value) {
		if (type == Type.TYPE_DOUBLE) {
			return String.valueOf(value);
		}
		else if (type == Type.TYPE_INT) {
			return String.valueOf((int)value);
		}
		else if (type == Type.TYPE_ENUM && enumType != null) {
			String name = enumType.nameOf((int)value);
			return name != null ? name : "?";
		}
		else
			return "?";
	}
	
	public String format(BigDecimal value) {
		if (type == Type.TYPE_DOUBLE) {
			return String.valueOf(value);
		}
		else if (type == Type.TYPE_INT) {
			return String.valueOf(value);
		}
		else if (type == Type.TYPE_ENUM && enumType != null) {
			String name = enumType.nameOf((int)value.doubleValue());
			return name != null ? name : "?";
		}
		else
			return "?";
	}
}
