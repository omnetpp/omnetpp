/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model2;

import static org.omnetpp.scave.engine.ResultItemField.MODULE;
import static org.omnetpp.scave.engine.ResultItemField.NAME;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.Callable;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.charting.properties.ChartVisualProperties;
import org.omnetpp.scave.charttemplates.ChartTemplate;
import org.omnetpp.scave.charttemplates.ChartTemplateRegistry;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.ResultItemField;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Chart.ChartType;
import org.omnetpp.scave.model.Chart.DialogPage;
import org.omnetpp.scave.model.Charts;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave.model.ModelChangeEvent;
import org.omnetpp.scave.model.ModelObject;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.commands.AddChartPropertyCommand;
import org.omnetpp.scave.model.commands.AddInputFileCommand;
import org.omnetpp.scave.model.commands.CommandStack;
import org.omnetpp.scave.model.commands.CompoundCommand;
import org.omnetpp.scave.model.commands.ICommand;
import org.omnetpp.scave.model.commands.MoveChartCommand;
import org.omnetpp.scave.model.commands.SetChartPropertyCommand;
import org.omnetpp.scave.model.commands.SetInputFileCommand;

/**
 * A collection of static methods to manipulate model objects
 * @author andras
 */
public class ScaveModelUtil {

    public static InputFile createInput(String name) {
        InputFile input = new InputFile(name);
        return input;
    }

    public static Chart createChart(String name, String title, ResultType type) {
        Chart chart;
        // XXX STATISTICS type? MATPLOTLIB chart?
        if (type==ResultType.SCALAR_LITERAL)
            chart = new Chart(ChartType.BAR, name);
        else if (type==ResultType.VECTOR_LITERAL)
            chart = new Chart(ChartType.LINE, name);
        else if (type==ResultType.HISTOGRAM_LITERAL)
            chart = new Chart(ChartType.HISTOGRAM, name);
        else
            throw new IllegalArgumentException();

        Property property = new Property(ChartVisualProperties.PROP_GRAPH_TITLE, title);
        chart.addProperty(property);

        return chart;
    }

    public static Chart createChartFromTemplate(String templateId) {
        return createChartFromTemplate(ChartTemplateRegistry.getTemplateByID(templateId));
    }

    public static Chart createChartFromTemplate(ChartTemplate template) {
        Chart chart = new Chart(template.getChartType());
        chart.setTemplateID(template.getID());

        chart.setScript(template.getPythonScript());

        List<DialogPage> templatePages = template.getDialogPages();
        List<Chart.DialogPage> pages = new ArrayList<Chart.DialogPage>(templatePages.size());

        for (int i = 0; i < templatePages.size(); ++i)
            pages.add(new DialogPage(templatePages.get(i)));

        chart.setDialogPages(pages);

        chart.setName(template.getName());
        chart.setTemporary(false);
        chart.setIconPath(template.getIconPath());
        return chart;
    }

    public static List<String> getIDListAsFilters(IDList ids, String[] runidFields, ResultFileManager manager) {
        Assert.isNotNull(runidFields);
        String[] filterFields = getFilterFieldsFor(runidFields);
        List<String> filters = new ArrayList<String>(ids.size());
        for (int i = 0; i < ids.size(); ++i) {
            long id = ids.get(i);
            String filter = new FilterUtil(manager.getItem(id), filterFields).getFilterPattern(); //TODO include: getTypeOf(item)
            filters.add(filter);
        }
        return filters;
    }

    public static String getIDListAsFilterExpression(IDList ids, String[] runidFields, ResultFileManager manager) {
        Assert.isNotNull(runidFields);
        String[] filterFields = getFilterFieldsFor(runidFields);

        StringBuilder sb = new StringBuilder();

        RunList runs = manager.getUniqueRuns(ids);

        if (runs.size() < ids.size() / 2) {

            boolean first = true;

            for (Run r :runs.toArray()) {
                if (!first)
                    sb.append("\n OR \n");
                sb.append("( run(\"" + r.getRunName() + "\") AND (\n");

                IDList idsInRun = manager.filterIDList(ids, r, null, null);

                for (int i = 0; i < idsInRun.size(); ++i) {
                    long id = ids.get(i);
                    ResultItem item = manager.getItem(id);
                    String filter = new FilterUtil(item, filterFields).getFilterPattern();
                    sb.append(filter + " \n");
                }
                sb.append(") )");

                first = false;
            }
        }
        else {

            for (int i = 0; i < ids.size(); ++i) {
                long id = ids.get(i);
                ResultItem item = manager.getItem(id);
                String filter = new FilterUtil(item, filterFields).getFilterPattern();
                sb.append(filter + " \n");
            }
        }
        return sb.toString();
    }


    public static void addInputFiles(CommandStack commandStack, Analysis analysis, List<String> list) {
        List<ICommand> addCommands = new ArrayList<ICommand>();

        for (String item : list) {
            InputFile inputFile = new InputFile(item);
            addCommands.add(new AddInputFileCommand(analysis, inputFile));
        }
        ICommand command = new CompoundCommand("Add input files", addCommands);
        commandStack.execute(command);
    }

    public static void setInputFile(CommandStack commandStack, InputFile inputFile, String value) {
        ICommand command = new SetInputFileCommand(inputFile, value);
        commandStack.execute(command);
    }

    public static String[] getFilterFieldsFor(String[] runidFields) {
        Assert.isNotNull(runidFields);
        int runidFieldCount = runidFields.length;
        String[] filterFields = new String[runidFieldCount+2];
        System.arraycopy(runidFields, 0, filterFields, 0, runidFieldCount);
        filterFields[runidFieldCount] = MODULE;
        filterFields[runidFieldCount + 1] = NAME;
        return filterFields;
    }

    public static void moveElements(CommandStack commandStack, Charts charts, Object[] elements, int index) {
        // the elements[] array contains the elements in no particular order. We need to keep
        // the order they are in the model (Charts object). Plus, we need reverse order due to
        // the order we insert them back.
        List<AnalysisItem> itemsToMove = new ArrayList<>();
        for (AnalysisItem item : charts.getCharts())
            if (ArrayUtils.contains(elements, item))
                itemsToMove.add(0, item); // reverse order

        // do a series of "Move" commands
        CompoundCommand command = new CompoundCommand("Move analysis items");
        for (AnalysisItem item : itemsToMove)
            command.append(new MoveChartCommand(charts, item, index));
        commandStack.execute(command);
    }

    /**
     * Finds an enclosing object having type {@code type}.
     * If the {@code object} itself has the type, it is returned.
     */
    @SuppressWarnings("unchecked")
    public static <T extends ModelObject> T findEnclosingOrSelf(ModelObject object, Class<T> type) {
        while (object != null && !type.isInstance(object))
            object = object.getParent();
        return (T)object;
    }

    /**
     * Collect charts from the given collection.
     */
    public static List<Chart> collectCharts(Collection<?> items) {
        List<Chart> charts = new ArrayList<>();
        for (Object item : items)
            if (item instanceof Chart)
                charts.add((Chart)item);
        return charts;
    }

    public static boolean isInputsChange(ModelChangeEvent event) {
        ModelObject subject = event.getSubject();
        return subject instanceof Inputs || subject instanceof InputFile;
    }

    public static Property getChartProperty(Chart chart, String propertyName) {
        for (Object object : chart.getProperties()) {
            Property property = (Property)object;
            if (property.getName().equals(propertyName))
                return property;
        }
        return null;
    }

    public static void setChartProperty(CommandStack commandStack, Chart chart, String propertyName, String propertyValue) {
        Property property = getChartProperty(chart, propertyName);
        ICommand command;
        if (property == null) {
            property = new Property(propertyName, propertyValue);
            command = new AddChartPropertyCommand(chart, property);
        }
        else {
            command = new SetChartPropertyCommand(property, propertyValue);
        }
        commandStack.execute(command);
    }

    public static IDList getAllIDs(ResultFileManager manager, ResultType type) {
        if (type == null)
            return manager.getAllItems(true, true);

        switch (type) {
        case SCALAR_LITERAL: return manager.getAllScalars(true, true);
        case VECTOR_LITERAL: return manager.getAllVectors();
        case STATISTICS_LITERAL: return manager.getAllStatistics();
        case HISTOGRAM_LITERAL: return manager.getAllHistograms();
        }
        Assert.isTrue(false, "Unknown dataset type: " + type);
        return null;
    }

    public static ResultItem[] getResultItems(IDList idlist, ResultFileManager manager) {
        int size = idlist.size();
        ResultItem[] items = new ResultItem[size];
        for (int i = 0; i < size; ++i)
            items[i] = manager.getItem(idlist.get(i));
        return items;
    }

    public static IDList filterIDList(IDList idlist, Filter filter, ResultFileManager manager) {
        Assert.isTrue(filter.getFilterPattern()!=null);
        return manager.filterIDList(idlist, filter.getFilterPattern());
    }

    /**
     * Returns an ordered array of distinct values of the {@code field} attribute
     * of the result items found in {@code idlist}.
     */
    public static String[] getFieldValues(IDList idlist, ResultItemField field, ResultFileManager manager) {
        Set<String> values = new HashSet<>();
        for (int i = 0; i < idlist.size(); ++i) {
            long id = idlist.get(i);
            ResultItem item = manager.getItem(id);
            String value = field.getFieldValue(item);
            if (!StringUtils.isEmpty(value))
                values.add(value);
        }
        String[] result = values.toArray(new String[values.size()]);
        Arrays.sort(result);
        return result;
    }


    public static List<String> getResultItemFields(IDList idlist, ResultFileManager manager) {
        List<String> fields = new ArrayList<>();
        fields.add(ResultItemField.FILE);
        fields.add(ResultItemField.RUN);
        fields.add(ResultItemField.MODULE);
        fields.add(ResultItemField.NAME);
        fields.addAll(getRunAttributeNames(idlist, manager));
        return fields;
    }

    public static List<String> getRunAttributeNames(IDList idlist, ResultFileManager manager) {
        StringVector runAttributes = manager.getUniqueRunAttributeNames(manager.getUniqueRuns(idlist)).keys();
        return Arrays.asList(runAttributes.toArray());
    }


    public static void dumpIDList(String header, final IDList idlist, final ResultFileManager manager) {
        Debug.print(header);
        if (idlist.size() == 0)
            Debug.println("Empty");
        else {
            Debug.println();
            ResultFileManager.callWithReadLock(manager, new Callable<Object>() {
                @Override
                public Object call() {
                    for (int i = 0; i < idlist.size(); ++i) {
                        ResultItem r = manager.getItem(idlist.get(i));
                        Debug.println(
                            String.format("File: %s Run: %s Module: %s Name: %s",
                                r.getFileRun().getFile().getFilePath(),
                                r.getFileRun().getRun().getRunName(),
                                r.getModuleName(),
                                r.getName()));
                    }
                    return null;
                }
            });
        }
    }

    /**
     *
     */
    public static int asInternalResultType(ResultType type) {
        switch (type) {
        case SCALAR_LITERAL: return ResultFileManager.SCALAR;
        case VECTOR_LITERAL: return ResultFileManager.VECTOR;
        case STATISTICS_LITERAL: return ResultFileManager.STATISTICS;
        case HISTOGRAM_LITERAL: return ResultFileManager.HISTOGRAM;
        default: Assert.isTrue(false, "Unknown ResultType:"+type); return 0;
        }
    }

    public static ResultType asResultType(int internalResultType) {
        if (internalResultType == ResultFileManager.SCALAR)
            return ResultType.SCALAR_LITERAL;
        else if (internalResultType == ResultFileManager.VECTOR)
            return ResultType.VECTOR_LITERAL;
        else if (internalResultType == ResultFileManager.STATISTICS)
            return ResultType.STATISTICS_LITERAL;
        else if (internalResultType == ResultFileManager.HISTOGRAM)
            return ResultType.HISTOGRAM_LITERAL;
        else {
            Assert.isTrue(false, "Unknown internal ResultType:"+internalResultType);
            return null;
        }
    }

}
