package org.omnetpp.scave.model2;

import static org.omnetpp.scave.model2.ResultItemFields2.FIELD_DATANAME;
import static org.omnetpp.scave.model2.ResultItemFields2.FIELD_FILENAME;
import static org.omnetpp.scave.model2.ResultItemFields2.FIELD_MODULENAME;
import static org.omnetpp.scave.model2.ResultItemFields2.FIELD_RUNNAME;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.omnetpp.scave.engine.ResultItem;

/**
 * Formatting tool for generating legend labels and
 * identifiers for result items.
 * <p>
 * Format string may contain to fields of the result item
 * by "{fieldname}" syntax. Use "\{" to quote a '{'.
 * Accepted field names are defined by {@link FilterUtil.getFieldNames()}.
 * Use {index} to refer to the index of the item in the collection.
 *  
 * @author tomi
 */
public class ResultItemFormatter {
	
	private static final String fieldSpecifierRE = "(?<!\\\\)\\{([a-zA-Z]+)\\}";
	private static final Pattern fsPattern = Pattern.compile(fieldSpecifierRE);
	
	private static final Map<String,IResultItemFormatter> formatters;
	
	static {
		formatters = new HashMap<String,IResultItemFormatter>();
		for (String field : ResultItemFields2.getFieldNames()) {
			IResultItemFormatter formatter = null;
			if (field.equals(FIELD_FILENAME))
				formatter = new FileNameFormatter();
			else if (field.equals(FIELD_RUNNAME))
				formatter = new RunNameFormatter();
			else if (field.equals(FIELD_MODULENAME))
				formatter = new ModuleNameFormatter();
			else if (field.equals(FIELD_DATANAME))
				formatter = new DataNameFormatter();
			else if (RunAttribute.isRunAttribute(field))
				formatter = new RunAttributeFormatter(field);
			if (formatter != null)
				formatters.put(field, formatter);
		}
	}
	
	public static String[] formatResultItems(String format, ResultItem[] items) {
		return formatResultItems(format, Arrays.asList(items));
	}

	public static String[] formatResultItems(String format, Collection<? extends ResultItem> items) {
		Object[] formatObjects = parseFormatString(format);
		String[] names = new String[items.size()];
		int i = 0;
		for (ResultItem item : items)
			names[i++] = formatResultItem(formatObjects, item);
		return names;
	}
	
	public static String formatResultItem(String format, ResultItem item) {
		return formatResultItem(parseFormatString(format), item);
	}
	
	private static String formatResultItem(Object[] format, ResultItem item) {
		StringBuffer sb = new StringBuffer();
		for (Object formatObj : format) {
			if (formatObj instanceof String)
				sb.append((String)formatObj);
			else if (formatObj instanceof IResultItemFormatter)
				sb.append(((IResultItemFormatter)formatObj).format(item));
		}
		return sb.toString();
	}
	
	private static Object[] parseFormatString(String format) {
		List<Object> formatObjs = new ArrayList<Object>();
		Matcher matcher = fsPattern.matcher(format);
		int start = 0, len = format.length();
		while (start < len) {
		    if (matcher.find(start)) {
		    	// add previous characters as fixed string
				if (matcher.start() != start)
				    formatObjs.add(format.substring(start, matcher.start()));

				String fieldName = matcher.group(1);
				IResultItemFormatter formatter = getFormatter(fieldName);
				if (formatter != null)
					formatObjs.add(formatter);
				else
					formatObjs.add(format.substring(matcher.start(), matcher.end()));
				start = matcher.end();
		    }
		    else {
				// No more valid format specifiers.
				// The rest of the string is fixed text
				formatObjs.add(format.substring(start));
				break;
		    }
		}
		return formatObjs.toArray();
	}
	
	private static IResultItemFormatter getFormatter(String field) {
		if ("index".equals(field))
			return new IndexFormatter();
		else
			return formatters.get(field);
	}
	
	interface IResultItemFormatter
	{
		String format(ResultItem item);
	}
	
	static class FileNameFormatter implements IResultItemFormatter
	{
		public String format(ResultItem item) {
			return item.getFileRun().getFile().getFilePath();
		}
	}

	static class RunNameFormatter implements IResultItemFormatter
	{
		public String format(ResultItem item) {
			return item.getFileRun().getRun().getRunName();
		}
	}
	
	static class ModuleNameFormatter implements IResultItemFormatter
	{
		public String format(ResultItem item) {
			return item.getModuleName();
		}
	}
	
	static class DataNameFormatter implements IResultItemFormatter
	{
		public String format(ResultItem item) {
			return item.getName();
		}
	}
	
	static class RunAttributeFormatter implements IResultItemFormatter
	{
		private String attrName;
		
		public RunAttributeFormatter(String attrName) {
			this.attrName = attrName;
		}
		
		public String format(ResultItem item) {
			return item.getFileRun().getRun().getAttribute(attrName);
		}
	}
	
	static class IndexFormatter implements IResultItemFormatter
	{
		int index;
		
		public String format(ResultItem item) {
			return String.valueOf(index++);
		}
	}
}
