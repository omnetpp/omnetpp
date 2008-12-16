/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.SCRAP;

import java.util.HashMap;
import java.util.Map;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.model.SetOperation;
import org.omnetpp.scave.model2.FilterSyntax;
import org.omnetpp.scave.model2.FilterSyntax.INodeVisitor;
import org.omnetpp.scave.model2.FilterSyntax.Node;
import org.omnetpp.scave.model2.FilterSyntax.Token;
import org.omnetpp.scave.model2.FilterSyntax.TokenType;

/**
 * Filter parameters for datasets.
 */
//FIXME this class has to be stripped down to only contain the filter string, and eventually eliminated
public class Filter {

	public static final String FIELD_FILENAME = "file";
	public static final String FIELD_RUNNAME = "run";
	public static final String FIELD_EXPERIMENT = "experiment";
	public static final String FIELD_MEASUREMENT = "measurement";
	public static final String FIELD_REPLICATION = "replication";
	public static final String FIELD_MODULENAME = "module";
	public static final String FIELD_DATANAME = "name";

	private static final String[] fieldNames = new String[] {
		FIELD_FILENAME,
		FIELD_RUNNAME,
		FIELD_EXPERIMENT,
		FIELD_MEASUREMENT,
		FIELD_REPLICATION,
		FIELD_MODULENAME,
		FIELD_DATANAME,
	};

	// full match expression
	private String filterPattern;
	// separate fields connected with AND operator
	private Map<String, String> fields = new HashMap<String, String>();

	public Filter() {
	}

	public Filter(String filterText) {
		this.filterPattern = filterText;
		parseFields(filterText);
	}

	public Filter(String fileName, String runName,
			String experimentName, String measurementName, String replicationName,
			String moduleName, String dataName) {
		setField(FIELD_FILENAME, fileName);
		setField(FIELD_RUNNAME, runName);
		setField(FIELD_EXPERIMENT, experimentName);
		setField(FIELD_MEASUREMENT, measurementName);
		setField(FIELD_REPLICATION, replicationName);
		setField(FIELD_MODULENAME, moduleName);
		setField(FIELD_DATANAME, dataName);
	}

	public Filter(SetOperation op) {
		this(op.getFilterPattern()==null ? "" : op.getFilterPattern());
	}

	public static String[] getFieldNames() {
		return fieldNames;
	}

	public static String getDefaultField() {
		return FIELD_DATANAME;
	}

	public static boolean isFieldName(String name) {
		if (name != null)
			for (String fieldName : fieldNames)
				if (name.equals(fieldName))
					return true;
		return false;
	}

	public String getFilterPattern() {
		return filterPattern != null ? filterPattern : buildPattern();
	}

	public boolean setFilterPattern(String text) {
		filterPattern = text;
		return parseFields(text);
	}

	public String getField(String name) {
		String value = fields.get(name);
		return value != null ? value : "";
	}

	public void setField(String name, String value) {
		fields.put(name, value);
	}

	private String buildPattern() {
		StringBuffer sb = new StringBuffer();
		for (String name : fieldNames) {
			if (fields.containsKey(name))
				addField(sb, name, getField(name));
		}
		if (sb.length() == 0) sb.append("*");
		return sb.toString();
	}

	private void addField(StringBuffer sb, String attrName, String attrPattern) {
		if (attrPattern != null && attrPattern.length() > 0) {
			if (sb.length() > 0)
				sb.append(" AND ");
			sb.append(quote(attrName)).append("(").append(quote(attrPattern)).append(")");
		}
	}

	private String quote(String str) {
		if (StringUtils.containsNone(str, " \t\n()"))
			return str;
		else
		{
			StringBuffer sb = new StringBuffer(str.length()+2);
			sb.append('"').append(str).append('"');
			return sb.toString();
		}
	}

	private boolean parseFields(String matchExpr) { // XXX test code
		try {
			Node node = FilterSyntax.parseFilter(matchExpr);
			FilterNodeVisitor visitor = new FilterNodeVisitor();
			node.accept(visitor);
			if (visitor.simple) {
				for (Map.Entry<String,String> entry : visitor.fields.entrySet())
					if (isFieldName(entry.getKey()))
						setField(entry.getKey(), entry.getValue());
				return true;
			}
			else
				return false;
		} catch (Exception e) {
			e.printStackTrace();  //FIXME log it or something. Just "print" is not OK!
			return false;
		}

	}

	private static class FilterNodeVisitor implements INodeVisitor {
		public boolean simple = true;
		public Map<String,String> fields = new HashMap<String, String>();

		public boolean visit(Node node) {
			switch (node.type) {
			case Node.UNARY_OPERATOR_EXPR: simple = false; return false;
			case Node.BINARY_OPERATOR_EXPR:
				if (node.getOperator().getType() == TokenType.AND)
					return true;
				else {
					simple = false;
					return false;
				}
			case Node.PATTERN:
				if (!fields.containsKey(Filter.getDefaultField())) {
					if (!node.getPattern().isEmpty())
						fields.put(Filter.getDefaultField(), node.getPatternString());
				}
				else
					simple = false;
				return false;
			case Node.FIELDPATTERN:
				if (!fields.containsKey(node.getFieldName())) {
					if (!node.getPattern().isEmpty())
						fields.put(node.getFieldName(), node.getPatternString());
				}
				else
					simple = false;
				return false;
			case Node.PARENTHESISED_EXPR:
				return true;
			default: /*TODO*/ return false;
			}
		}

		public void visit(Token token) {}
	}

	public String toString() {
		StringBuffer sb = new StringBuffer();
		for (String name : fieldNames) {
			if (fields.containsKey(name)) {
				sb.append(name).append("=").append(getField(name)).append(" ");
			}
		}
		if (sb.length() > 0)
			sb.deleteCharAt(sb.length() - 1);
		else
			sb.append("<all>");
		return sb.toString();
	}
}
