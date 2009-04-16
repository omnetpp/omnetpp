/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model2;

import static org.omnetpp.scave.model2.FilterField.FILE;
import static org.omnetpp.scave.model2.FilterField.MODULE;
import static org.omnetpp.scave.model2.FilterField.NAME;
import static org.omnetpp.scave.model2.FilterField.RUN;

import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.Callable;

import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model2.FilterField.Kind;

/**
 * Given an IDList, produces hints for different filter fields such as
 * module name, statistic name, run attributes etc.
 *
 * @author tomi
 */
public class FilterHints {
	
	private static final String[] EMPTY = new String[0];
	
	private Map<FilterField,String[]> hints = new HashMap<FilterField, String[]>();

	public FilterHints() {
	}

	public FilterHints(final ResultFileManager manager, final ResultType type) {
		ResultFileManager.callWithReadLock(manager, new Callable<Object>() {
			public Object call() throws Exception {
				init(manager, ScaveModelUtil.getAllIDs(manager, type));
				return null;
			}
		});
	}
	
	public FilterHints(final ResultFileManager manager, final IDList idlist) {
		ResultFileManager.callWithReadLock(manager, new Callable<Object>() {
			public Object call() throws Exception {
				init(manager, idlist);
				return null;
			}
		});
	}
	
	private void init(ResultFileManager manager, IDList idlist) {
		manager.checkReadLock();
		ResultFileList fileList = manager.getUniqueFiles(idlist);
		RunList runList = manager.getUniqueRuns(idlist);

		setHints(FILE, manager.getFilePathFilterHints(fileList).toArray());
		setHints(RUN, manager.getRunNameFilterHints(runList).toArray());
		setHints(MODULE, manager.getModuleFilterHints(idlist).toArray());
		setHints(NAME, manager.getNameFilterHints(idlist).toArray());
		for (String attrName : manager.getUniqueAttributeNames(idlist).keys().toArray())
			setHints(Kind.ItemField, attrName, manager.getResultItemAttributeFilterHints(idlist, attrName).toArray());
		for (String attrName : manager.getUniqueRunAttributeNames(runList).keys().toArray())
			setHints(Kind.RunAttribute, attrName, manager.getRunAttributeFilterHints(runList, attrName).toArray());
		for (String paramName : manager.getUniqueModuleParamNames(runList).keys().toArray())
			setHints(Kind.ModuleParam, paramName, manager.getModuleParamFilterHints(runList, paramName).toArray());
	}
	
	public FilterField[] getFields() {
		Set<FilterField> keys = hints.keySet();
		FilterField[] fields = keys.toArray(new FilterField[keys.size()]);
		Arrays.sort(fields);
		return fields;
	}
	
	public String[] getHints(FilterField field) {
		if (hints.containsKey(field))
			return hints.get(field);
		else
			return EMPTY;
	}

	private void setHints(FilterField field, String[] value) {
		this.hints.put(field, value);
	}

	private void setHints(Kind kind, String name, String[] value) {
		this.hints.put(new FilterField(kind, name), value);
	}
}
