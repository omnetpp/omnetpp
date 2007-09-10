package org.omnetpp.scave.model2;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.omnetpp.scave.engine.ResultItem;

/**
 * Simple reflection utility to describe fields of result items
 * and to access them by name.
 *
 * @author tomi
 */
public class ResultItemFields2 {

	public static final String FIELD_FILENAME = "file";
	public static final String FIELD_RUNNAME = "run";
	public static final String FIELD_MODULENAME = "module";
	public static final String FIELD_DATANAME = "name";

	private static String[] fieldNames;

	static {
		List<String> fields = new ArrayList<String>();
		fields.addAll(Arrays.asList(new String[] {FIELD_FILENAME, FIELD_RUNNAME, FIELD_MODULENAME, FIELD_DATANAME}));
		fields.addAll(Arrays.asList(RunAttribute.getNames()));
		ResultItemFields2.fieldNames = fields.toArray(new String[fields.size()]);
	}

	public static String[] getFieldNames() {
		return fieldNames;
	}

	public static boolean isFieldName(String name) {
		if (name != null)
			for (String fieldName : fieldNames)
				if (name.equals(fieldName))
					return true;
		return false;
	}

	public static String getFieldValue(ResultItem item, String field) {
		if (item != null && field != null) {
			if (FIELD_FILENAME.equals(field))
				return item.getFileRun().getFile().getFilePath();
			else if (FIELD_RUNNAME.equals(field))
				return item.getFileRun().getRun().getRunName();
			else if (FIELD_MODULENAME.equals(field))
				return item.getModuleName();
			else if (FIELD_DATANAME.equals(field))
				return item.getName();
			else if (RunAttribute.isRunAttribute(field))
				return item.getFileRun().getRun().getAttribute(field);
		}
		return null;
	}
}
