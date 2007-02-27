package org.omnetpp.scave.model2;

import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.Common;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.model2.FilterSyntax.INodeVisitor;
import org.omnetpp.scave.model2.FilterSyntax.Node;
import org.omnetpp.scave.model2.FilterSyntax.Token;
import org.omnetpp.scave.model2.FilterSyntax.TokenType;

/**
 * Parsing and assembling filter patterns. Filter patterns containing OR, NOT or
 * parentheses cannot be represented, only ones that contain terms connected with AND.
 * Whether a pattern is in a representable form can be determined by calling isANDPattern().
 */
public class FilterUtil {

	// Note: these field names must be kept consistent with resultfilemanager.cc
	// XXX and with RunAttribute
	// XXX or: obtain them from resultfilemanager.cc via swig.. 
	public static final String FIELD_FILENAME = "file";
	public static final String FIELD_RUNNAME = "run";
	public static final String FIELD_EXPERIMENT = "experiment";
	public static final String FIELD_MEASUREMENT = "measurement";
	public static final String FIELD_REPLICATION = "replication";
	public static final String FIELD_MODULENAME = "module";
	public static final String FIELD_DATANAME = "name";
	public static final String FIELD_RUNNUMBER = "runNumber";
	public static final String FIELD_NETWORKNAME = "networkName";
	public static final String FIELD_DATETIME = "dateTime";

	private static final String[] fieldNames = new String[] {
		FIELD_FILENAME,
		FIELD_RUNNAME,
		FIELD_RUNNUMBER,
		FIELD_EXPERIMENT,
		FIELD_MEASUREMENT,
		FIELD_REPLICATION,
		FIELD_MODULENAME,
		FIELD_DATANAME,
		FIELD_RUNNUMBER,
		FIELD_NETWORKNAME,
		FIELD_DATETIME,
	};

	// separate fields connected with AND operator. The full string is NOT STORED.
	private Map<String, String> fields = new LinkedHashMap<String, String>();

	// if true, the original string contained ORs/NOTs/parens, which means it cannot be
	// reconstructed from fields[].
	private boolean lossy = false;

	public FilterUtil() {
	}

	public FilterUtil(String filterPattern) {
		this(filterPattern, false);
	}

	public FilterUtil(String filterPattern, boolean lossyAllowed) {
		if (!isANDPattern(filterPattern)) {
			if (!lossyAllowed) 
				throw new IllegalArgumentException("Not an AND pattern: "+filterPattern);
			lossy = true;
		}
		parseFields(filterPattern);
	}

	public FilterUtil(String runName, String moduleName, String dataName) {
		setField(FIELD_RUNNAME, runName);
		setField(FIELD_MODULENAME, moduleName);
		setField(FIELD_DATANAME, dataName);
	}

	public FilterUtil(String fileName, String runName,
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

	public FilterUtil(ResultItem item, String[] runidFields) {
		ResultFile file = item.getFileRun().getFile();
		Run run = item.getFileRun().getRun();
		if (runidFields==null) {
			if (run.getRunName().length()>0) {
				// default: identify run with runName
				setField(FIELD_RUNNAME, run.getRunName());
			}
			else {
				// fallback for old files that don't have runName yet
				setField(FIELD_FILENAME, file.getFilePath());
				String runNumber = run.getAttribute(RunAttribute.RUNNUMBER);
				if (runNumber != null)
					setField(FIELD_RUNNUMBER, runNumber);
			}
		}
		else {
			// explicit selection of run identification fields
			for (String field : runidFields) {
				if (field == FIELD_FILENAME)
					setField(field, file.getFilePath());
				else if (field == FIELD_RUNNAME)
					setField(field, run.getRunName());
				else if (field == FIELD_EXPERIMENT)
					setField(field, run.getAttribute(RunAttribute.EXPERIMENT));
				else if (field == FIELD_MEASUREMENT)
					setField(field, run.getAttribute(RunAttribute.MEASUREMENT));
				else if (field == FIELD_REPLICATION)
					setField(field, run.getAttribute(RunAttribute.REPLICATION));
			}
		}
		setField(FIELD_MODULENAME, item.getModuleName());
		setField(FIELD_DATANAME, item.getName());
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
		if (lossy)
			throw new IllegalStateException("Original pattern cannot be reconstructed because it contained OR/NOT/parens");
		return buildPattern();
	}

	public boolean isLossy() {
		return lossy;
	}
	
	public String getField(String name) {
		String value = fields.get(name);
		return value != null ? value : "";
	}

	public void setField(String name, String value) {
		fields.put(name, value);
	}

	public boolean containsOnly(String[] fieldNames) {
		Set<String> allowedNames = new HashSet<String>(Arrays.asList(fieldNames));
		for (String name : fields.keySet())
			if (!allowedNames.contains(name))
				return false;
		return true;
	}
	
	private String buildPattern() {
		StringBuffer sb = new StringBuffer();
		for (String name : fields.keySet())
			appendField(sb, name, getField(name));
		if (sb.length() == 0) sb.append("*");
		return sb.toString();
	}

	private void appendField(StringBuffer sb, String attrName, String attrPattern) {
		if (attrPattern != null && attrPattern.length() > 0) {
			if (sb.length() > 0)
				sb.append(" AND ");
			sb.append(quoteStringIfNeeded(attrName)).append("(").append(quoteStringIfNeeded(attrPattern)).append(")");
		}
	}
	
	public static boolean needsQuotes(String pattern) {
		return Common.needsQuotes(pattern) || StringUtils.indexOfAny(pattern, " \t\n()") >= 0;
	}
	
	public static String quoteStringIfNeeded(String str) {
		return needsQuotes(str) ? Common.quoteString(str) : str;
	}

	public static boolean isValidPattern(String pattern) {
		try {
			ResultFileManager.checkPattern(pattern);
		} catch (Exception e) {
			return false; // bogus
		}
		return true;
	}

	public static boolean isANDPattern(String pattern) {
		if (!isValidPattern(pattern))
			return false; // a bogus pattern is not an "AND" pattern
		Node node = FilterSyntax.parseFilter(pattern);
		FilterNodeVisitor visitor = new FilterNodeVisitor();
		node.accept(visitor);
		return visitor.isANDPattern;
	}
	
	private void parseFields(String pattern) {
		// Note: here we allow non-AND patterns which cannot be represented, but 
		// at least the fields can be extracted.
		Node node = FilterSyntax.parseFilter(pattern);
		FilterNodeVisitor visitor = new FilterNodeVisitor();
		node.accept(visitor);
		for (Map.Entry<String,String> entry : visitor.fields.entrySet())
			setField(entry.getKey(), entry.getValue());
	}

	/**
	 * Collects fields, and determines whether this is an "AND" pattern
	 */
	private static class FilterNodeVisitor implements INodeVisitor {
		public boolean isANDPattern = true;
		public Map<String,String> fields = new HashMap<String, String>();

		public boolean visit(Node node) {
			switch (node.type) {
			case Node.UNARY_OPERATOR_EXPR: 
				isANDPattern = false;
				break;
			case Node.BINARY_OPERATOR_EXPR:
				if (node.getOperator().getType() != TokenType.AND)
					isANDPattern = false;
				break;
			case Node.PATTERN:
				if (!node.getPattern().isEmpty())
					fields.put(FilterUtil.getDefaultField(), node.getPatternString());
				break;
			case Node.FIELDPATTERN:
				if (!node.getPattern().isEmpty())
					fields.put(node.getFieldName(), node.getPatternString());
				break;
			case Node.PARENTHESISED_EXPR:
			case Node.ROOT:
				break;
			default: Assert.isTrue(false);  
			}
			return true;
		}

		public void visit(Token token) {}
	}

	public String toString() {
		return getFilterPattern();
	}

}
