package org.omnetpp.scave.model2;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.runtime.IProgressMonitor;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.editors.datatable.PanelType;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.InterruptedFlag;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engine.Scave;

/**
 * Given a set of selected results (an IDList of selected items, out of an IDList
 * of "all" items), generates a filter expression that results in the first IDList,
 * when used to filter the second one. Tries to keep the expression short and reasonable.
 */
public class ResultSelectionFilterGenerator {

    protected static boolean debug = false; // TODO initialize from OMNETPP_DEBUG envvar, like elsewhere

    static class AttributeValueCounts {
        int targetCount;
        int nonTargetCount;
    }

    static class ColumnValueCounts {
        Map<String, AttributeValueCounts> valueCounts;
    }

    /**
     * Given a set of selected results (an IDList of selected items, out of an IDList
     * of "all" items), generates a filter expression that results in the "target" IDList,
     * when used to filter the second one. Tries to keep the expression short and reasonable.
     */
    public static String getFilter(IDList target, IDList all, ResultFileManager manager, IProgressMonitor monitor, InterruptedFlag interrupted) throws InterruptedException {
        return ResultFileManager.callWithReadLock(manager, () -> {
            if (monitor != null)
                monitor.beginTask("Generating filter", 1);

            if (!target.isSubsetOf(all))
                throw new IllegalArgumentException("IDs to be selected must be a subset of all IDs");

            return doGetFilter(target, all, manager, monitor, interrupted);
        });
    }

    protected static String doGetFilter(IDList target, IDList all, ResultFileManager manager, IProgressMonitor monitor, InterruptedFlag interrupted) throws InterruptedException {

        checkMonitor(monitor);

        String approxFilter = getApproximateFilter(target, all, manager, interrupted);

        checkMonitor(monitor);

        if (debug) {
            Debug.println("getFilter, trying to reduce " + all.size() + " IDs to " + target.size());
            Debug.println("approx filter is: " + approxFilter);
        }


        IDList approxMatching = Debug.timed("approxfilter", 1, () -> manager.filterIDList(all, approxFilter, -1, interrupted));

        checkMonitor(monitor);

        if (debug)
            Debug.println("approx matching count: " + approxMatching.size());

        IDList approxNotMatching = all.subtract(approxMatching);

        IDList toBeExcluded = approxMatching.subtract(target);
        IDList toBeIncluded = target.subtract(approxMatching);

        String includeFilter = null;
        if (!toBeIncluded.isEmpty()) {
            if (debug)
                Debug.println("computing include filter...");
            includeFilter = doGetFilter(toBeIncluded, approxNotMatching,  manager, monitor, interrupted);
            if (debug)
                Debug.println("include filter: " + includeFilter);
        }

        String excludeFilter = null;
        if (!toBeExcluded.isEmpty()) {
            if (debug)
                Debug.println("computing exclude filter...");
            excludeFilter = doGetFilter(toBeExcluded, approxMatching, manager, monitor, interrupted);
            if (debug)
                Debug.println("exclude filter: " + excludeFilter);
        }

        boolean haveInclude = includeFilter != null && !includeFilter.equals("(\n    *\n)");
        boolean haveExclude = excludeFilter != null && !excludeFilter.equals("(\n    *\n)");

        if (approxFilter.equals("*")) {
            String result = "";

            if (haveInclude) {
                if (haveExclude)
                    result += " (\n" + StringUtils.indentLines(includeFilter, "    ") + "\n)";
                else
                    result += includeFilter;
            }

            if (haveExclude) {
                if (haveInclude)
                    result += "\n AND \n";

                result += "NOT (\n" + StringUtils.indentLines(excludeFilter, "    ") + "\n)";
            }

            return result.isEmpty() ? "*" : result;
        }
        else {
            if (!haveInclude && !haveExclude)
                return approxFilter;
            else {
                String result = "(\n" + StringUtils.indentLines(approxFilter, "    ") + "\n)";

                if (haveInclude)
                    result += " OR (\n" + StringUtils.indentLines(includeFilter, "    ") + "\n)";

                if (haveExclude)
                    result += " AND NOT (\n" + StringUtils.indentLines(excludeFilter, "    ") + "\n)";

                return result;
            }
        }
    }

    // TODO reasonable name
    protected static ColumnValueCounts computeColumnData(IDList target, IDList all, String attrName, ResultFileManager manager, InterruptedFlag interrupted) throws InterruptedException {

        IDList nonTarget = all.subtract(target);

        Map<String, AttributeValueCounts> valueCounts = new HashMap<String, AttributeValueCounts>();

        for (int i = 0; i < target.size(); ++i) {
            String value = manager.getItemProperty(target.get(i), attrName);
            if (!valueCounts.containsKey(value))
                valueCounts.put(value, new AttributeValueCounts());

            valueCounts.get(value).targetCount += 1;

            if ((i & 0xffff) == 0)
                checkInterruptedFlag(interrupted);
        }

        for (int i = 0; i < nonTarget.size(); ++i) {
            String value = manager.getItemProperty(nonTarget.get(i), attrName);
            if (!valueCounts.containsKey(value))
                valueCounts.put(value, new AttributeValueCounts());

            valueCounts.get(value).nonTargetCount += 1;

            if ((i & 0xffff) == 0)
                checkInterruptedFlag(interrupted);
        }

        if (debug)
            Debug.println("counters for " + attrName);
        for (String v : valueCounts.keySet()) {
            AttributeValueCounts c = valueCounts.get(v);
            if (debug)
                Debug.println("   " + v + " : target = " + c.targetCount + " , nonTarget = " + c.nonTargetCount);
        }

        ColumnValueCounts result = new ColumnValueCounts();
        result.valueCounts = valueCounts;
        return result;
    }

    protected static String getApproximateFilter(IDList target, IDList all, ResultFileManager manager, InterruptedFlag interrupted) throws InterruptedException {

        if (target.size() >= all.size() * 0.9)
            return "*";

        String[] fields = new String[] {"runattr:experiment", "runattr:measurement", "runattr:replication", "module", "name"};

        Map<String, ColumnValueCounts> columnData = new HashMap<String, ColumnValueCounts>();

        for (String f : fields) {
            columnData.put(f, computeColumnData(target, all, f, manager, interrupted));
            checkInterruptedFlag(interrupted);
        }

        String bestAttr = null;
        float bestScore = 0;

        for (String f : fields) {
            ColumnValueCounts cd = columnData.get(f);

            float columnScore = 0;
            int numVals = 0;

            for (String v : cd.valueCounts.keySet()) {
                AttributeValueCounts ad = cd.valueCounts.get(v);

                float tc = (float)ad.targetCount / (ad.nonTargetCount + ad.targetCount);
                float ntc = (float)ad.nonTargetCount / (ad.nonTargetCount + ad.targetCount);

                columnScore += Math.max(tc, ntc);
                numVals += 1;
            }

            columnScore /= numVals;

            if (debug)
                Debug.println("column score for " + f + " is: " + columnScore);

            if (columnScore > bestScore) {
                bestAttr = f;
                bestScore = columnScore;
            }
        }

        if (debug)
            Debug.println("best attr: " + bestAttr);

        Set<String> selectedValues = new HashSet<String>();

        ColumnValueCounts bcd = columnData.get(bestAttr);
        for (String k : bcd.valueCounts.keySet()) {
            AttributeValueCounts vcd = bcd.valueCounts.get(k);

            if (vcd.targetCount > vcd.nonTargetCount)
                selectedValues.add(k);
        }

        if (bestScore >= 0.8) {

            String newTerm = "";
            if (!selectedValues.isEmpty()) {
                if (selectedValues.size() == 1)
                    newTerm = bestAttr + " =~ " + AndFilter.quoteStringIfNeeded(selectedValues.iterator().next());
                else {
                    newTerm += "(";
                    String[] vals = selectedValues.toArray(new String[]{});
                    boolean first = true;
                    for (String v : vals) {
                        if (!first)
                            newTerm += " OR ";
                        newTerm += bestAttr + " =~ " + AndFilter.quoteStringIfNeeded(v);
                        first = false;
                    }
                    newTerm += ")";
                }

                return newTerm;
            }
        }

        return doMakeQuickFilter(target, manager);
    }

/*
    public static String getIDListAsFilterExpression(IDList allIDs, IDList ids, String[] runidFields, String viewFilter, ResultFileManager manager, IProgressMonitor monitor, InterruptedFlag interrupted) throws InterruptedException {
        return ResultFileManager.callWithReadLock(manager,
                () -> doGetIDListAsFilterExpression(allIDs, ids, runidFields, viewFilter, manager, monitor, interrupted)
        );
    }


    protected static String doGetIDListAsFilterExpression(IDList allIDs, IDList ids, String[] runidFields, String viewFilter, ResultFileManager manager, IProgressMonitor monitor, InterruptedFlag interrupted) throws InterruptedException {
        IDList allItemsOfType = allIDs;
        IDList itemsMatchingViewFilter = manager.filterIDList(allItemsOfType, viewFilter);

        boolean allSelected = ids.equals(itemsMatchingViewFilter);

        if (debug) {
            Debug.println("allitems: " + itemsMatchingViewFilter.toString());
            Debug.println("selected: " + ids.toString());
            Debug.println("selected: " + ids.size() + " matching: " + itemsMatchingViewFilter.size());
            Debug.println("allselected: " + allSelected);
        }

        if (allSelected) {
            if (debug)
                Debug.println("returning the view filter: " + viewFilter);
            return StringUtils.defaultIfEmpty(viewFilter, "*");
        }

        if (!ids.isSubsetOf(allIDs))
            throw new IllegalArgumentException("IDs to be selected must be a subset of all IDs");

        if (viewFilter.equals("*"))
            return getFilter(ids, itemsMatchingViewFilter, manager, monitor, interrupted);
        else
            return viewFilter + " AND (" + getFilter(ids, itemsMatchingViewFilter, manager, monitor, interrupted) + ")";

//TODO integrate or delete:
//
//        String refinedFilter = refineFilterForSelection(itemsMatchingViewFilter, ids, manager);
//        if (debug
//            Debug.println("refined filter: " + refinedFilter);
//
//        if (refinedFilter != null)
//            return refinedFilter;
//
//
//        IDList invisibleSelected = ids.dup();
//        invisibleSelected.subtract(itemsMatchingViewFilter);
//        Assert.isTrue(invisibleSelected.isEmpty());
//
//        IDList unselected = itemsMatchingViewFilter.dup();
//        unselected.subtract(ids);
//        if (debug)
//            Debug.println("number of unselected: " + unselected.size());
//
//
//        Assert.isNotNull(runidFields);
//        String[] filterFields = getFilterFieldsFor(runidFields);
//
//        String result;
//        if (ids.size() > 10 && unselected.size() < (ids.size() / 3)) {
//            result = "(" + viewFilter + ")\nAND NOT (\n" + StringUtils.indentLines(getIDListAsDumbFilter(unselected, manager, filterFields), "    ") + ")";
//        }
//        else {
//            RunList runs = manager.getUniqueRuns(ids);
//            if (runs.size() < ids.size() / 2)
//                result = getIDListAsRunGroupedFilter(ids, manager, filterFields);
//            else
//                result = getIDListAsDumbFilter(ids, manager, filterFields);
//        }
//
//        // debug check:
//        if (debug)
//            Debug.println("filter expression: " + result);
//        Assert.isTrue(manager.filterIDList(allItemsOfType, result).equals(ids), "Filter created from IDList does not reproduce the given IDs");
//
//        return result;
    }

    //TODO currently unused
    private static String getIDListAsRunGroupedFilter(IDList ids, ResultFileManager manager, String[] filterFields) {

        RunList runs = manager.getUniqueRuns(ids);
        StringBuilder sb = new StringBuilder();
        boolean first = true;

        for (Run r :runs.toArray()) {
            if (!first)
                sb.append("\n OR \n");
            sb.append("( run =~ " + AndFilter.quoteStringIfNeeded(r.getRunName()) + " AND (\n");

            IDList idsInRun = manager.filterIDList(ids, r, null, null);

            for (int i = 0; i < idsInRun.size(); ++i) {
                long id = ids.get(i);
                ResultItem item = manager.getItem(id);
                String filter = new AndFilter(item, filterFields).getFilterPattern();
                sb.append("    " + filter + " \n");
            }
            sb.append(") )");

            first = false;
        }
        return sb.toString();
    }
*/

    /**
     * Generate a filter string that produces the selected elements on a panel in the
     * Browse Data page.
     *
     * IMPORTANT: This method MUST complete under ~1 sec because it is used in such places.
     * Therefore it does not, and MUST NOT, call filterIDList(), or do anything else that
     * might blow up runtime.
     */
    public static String makeFilterForIDListSelection(IDListSelection idListSelection) {
        FilteredDataPanel dataPanel = idListSelection.getSource();
        IDList idList = idListSelection.getIDList();
        if (!dataPanel.getDataControl().getSelectedIDs().equals(idList))
            throw new RuntimeException("Selection is obsolete");

        ResultFileManager manager = idListSelection.getResultFileManager();
        IDList shown = dataPanel.getDataControl().getIDList();
        IDList selection = dataPanel.getDataControl().getSelectedIDs();

        if (selection.size() < shown.size() / 2) {
            // user has manually selected a few items
            return makeQuickFilter(selection, manager);
        }
        else {
            // user hit Ctrl+A, then manually unselected a few items
            PanelType type = dataPanel.getType();

            String filter = type.isSingleType()
                ? (type != PanelType.HISTOGRAMS
                    ? "type =~ " + ResultItem.itemTypeToString(type.getItemTypes())
                    : "(type =~ " + Scave.STATISTICS + " OR type=~ " +  Scave.HISTOGRAM + ")")
                : "*";

            boolean fieldsShownAsScalars = dataPanel.getShowFieldsAsScalars();
            if (type == PanelType.SCALARS && !fieldsShownAsScalars)
                filter += " AND isfield =~ false";

            String panelFilter = dataPanel.getFilter();
            if (!panelFilter.equals("*")) {
                if (dataPanel.isInAdvancedFilterMode() && panelFilter.contains("OR")) {
                    // to avoid messing up operator precedences after concatenation
                    panelFilter = "(" + panelFilter + ")";
                }
                filter += " AND " + panelFilter;
            }

            if (selection.size() < shown.size()) {
                IDList unselected = shown.subtract(selection);
                filter += "\nAND NOT (" + makeQuickFilter(unselected, manager) + ")";
            }
            return filter;
        }
    }

    /**
     * Creates a filter that identifies the given result items, with their type, run,
     * module and result name. Runtime only depends on (and is linear with) the number
     * of IDs passed. Simple optimization is done by factoring out the fields that
     * is the same in all items.
     */
    public static String makeQuickFilter(IDList ids, ResultFileManager manager) {
        return Debug.timed("makeQuickFilter() for IDList of len=" + ids.size(), 10,
                () -> ResultFileManager.callWithReadLock(manager, () -> doMakeQuickFilter(ids, manager)));
    }

    private static String doMakeQuickFilter(IDList ids, ResultFileManager manager) {
        // identify which fields can be factored out
        int types = ids.getItemTypes();
        RunList runs = manager.getUniqueRuns(ids);
        boolean sameType = types==ResultFileManager.PARAMETER || types==ResultFileManager.SCALAR || types==ResultFileManager.VECTOR || types==ResultFileManager.STATISTICS || types==ResultFileManager.HISTOGRAM;
        boolean sameExperiment = manager.getUniqueRunAttributeValues(runs, Scave.EXPERIMENT).size() == 1;
        boolean sameMeasurement = manager.getUniqueRunAttributeValues(runs, Scave.MEASUREMENT).size() == 1;
        boolean sameReplication = manager.getUniqueRunAttributeValues(runs, Scave.REPLICATION).size() == 1;
        boolean sameModule = manager.getUniqueModuleNames(ids).size() == 1;
        boolean sameName = manager.getUniqueResultNames(ids).size() == 1;

        List<String> factorFields_ = new ArrayList<>();
        List<String> itemFields_ = new ArrayList<>();
        (sameType ? factorFields_ : itemFields_).add(Scave.TYPE);
        (sameExperiment  ? factorFields_ : itemFields_).add(Scave.RUNATTR_PREFIX+Scave.EXPERIMENT);
        (sameMeasurement ? factorFields_ : itemFields_).add(Scave.RUNATTR_PREFIX+Scave.MEASUREMENT);
        (sameReplication ? factorFields_ : itemFields_).add(Scave.RUNATTR_PREFIX+Scave.REPLICATION);
        (sameModule ? factorFields_ : itemFields_).add(Scave.MODULE);
        (sameName   ? factorFields_ : itemFields_).add(Scave.NAME);
        String[] factorFields = factorFields_.toArray(new String[0]);
        String[] itemFields = itemFields_.toArray(new String[0]);

        StringBuilder sb = new StringBuilder();
        String itemIndent = "";

        // generate factored-out part
        if (factorFields.length != 0) {
            ResultItem item0 = manager.getItem(ids.get(0));
            String factorsFilter = new AndFilter(item0, factorFields).getFilterPattern();
            sb.append(factorsFilter);
            itemIndent = "  ";
        }

        if (factorFields.length != 0 && itemFields.length != 0)
            sb.append(" AND (\n");

        // generate per-item filters
        if (itemFields.length != 0) {
            for (int i = 0; i < ids.size(); ++i) {
                long id = ids.get(i);
                ResultItem item = manager.getItem(id);
                String filter = new AndFilter(item, itemFields).getFilterPattern();
                if (i > 0)
                    sb.append(" OR\n");
                sb.append(itemIndent);
                sb.append("  (");
                sb.append(filter);
                sb.append(")");
            }
        }

        if (factorFields.length != 0 && itemFields.length != 0)
            sb.append("\n)");

        return sb.toString();
    }

    private static void checkMonitor(IProgressMonitor monitor) throws InterruptedException {
        if (monitor != null) {
            monitor.worked(1);
            if (monitor.isCanceled())
                throw new InterruptedException();
        }
    }

    private static void checkInterruptedFlag(InterruptedFlag interrupted) throws InterruptedException {
        if (interrupted != null && interrupted.getFlag())
            throw new InterruptedException();
    }


}
