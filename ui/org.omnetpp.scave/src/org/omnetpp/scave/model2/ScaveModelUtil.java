/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model2;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.Callable;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.charttemplates.ChartTemplateRegistry;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.ResultItemField;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartTemplate;
import org.omnetpp.scave.model.Chart.DialogPage;
import org.omnetpp.scave.model.Charts;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave.model.ModelChangeEvent;
import org.omnetpp.scave.model.ModelObject;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.commands.AddChartCommand;
import org.omnetpp.scave.model.commands.AddInputFileCommand;
import org.omnetpp.scave.model.commands.CommandStack;
import org.omnetpp.scave.model.commands.CompoundCommand;
import org.omnetpp.scave.model.commands.ICommand;
import org.omnetpp.scave.model.commands.MoveChartCommand;
import org.omnetpp.scave.model.commands.SetChartIsTemporaryCommand;
import org.omnetpp.scave.model.commands.SetChartNameCommand;

/**
 * A collection of static methods to manipulate model objects
 * @author andras
 */
public class ScaveModelUtil {

    public static boolean debug = Debug.isChannelEnabled("scavemodelutil");

    public static InputFile createInput(String name) {
        InputFile input = new InputFile(name);
        return input;
    }


    public static Chart createChartFromTemplate(ChartTemplateRegistry chartTemplateRegistry, String templateId) {
        return createChartFromTemplate(chartTemplateRegistry.getTemplateByID(templateId));
    }

    public static Chart createChartFromTemplate(ChartTemplate template) {
        Chart chart = new Chart(template.getChartType());
        chart.setTemplateID(template.getId());

        chart.setScript(template.getPythonScript());

        List<DialogPage> templatePages = template.getDialogPages();
        List<Chart.DialogPage> pages = new ArrayList<Chart.DialogPage>(templatePages.size());

        for (int i = 0; i < templatePages.size(); ++i)
            pages.add(new DialogPage(templatePages.get(i)));

        chart.setDialogPages(pages);

        chart.setName(template.getName());
        chart.setTemporary(false);
        chart.setIconPath(template.getIconPath());

        List<Property> properties = new ArrayList<>();
        for (String name : template.getPropertyNames())
            properties.add(new Property(name, template.getPropertyDefault(name)));
        chart.setProperties(properties);

        return chart;
    }

    public static List<String> getTemplateSupportedResultTypesAsString(ChartTemplate template) {
        List<String> names = new ArrayList<>();
        int resultTypes = template.getSupportedResultTypes();
        if ((resultTypes & ResultFileManager.PARAMETER) != 0)
            names.add("parameters");
        if ((resultTypes & ResultFileManager.SCALAR) != 0)
            names.add("scalars");
        if ((resultTypes & ResultFileManager.VECTOR) != 0)
            names.add("vectors");
        if ((resultTypes & ResultFileManager.STATISTICS) != 0)
            names.add("statistics");
        if ((resultTypes & ResultFileManager.HISTOGRAM) != 0)
            names.add("histograms");
        return names;
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

    public static void saveChart(CommandStack commandStack, Chart chart, String name, Analysis analysis) {
        CompoundCommand command = new CompoundCommand("Save chart");

        command.append(new SetChartIsTemporaryCommand(chart, false));
        command.append(new SetChartNameCommand(chart, name));
        command.append(new AddChartCommand(analysis, chart));

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

    public static List<Chart> getChartsFromSelection(IStructuredSelection selection) {
        List<Chart> result = new ArrayList<Chart>();
        for (Object obj : selection.toArray())
            if (obj instanceof Chart)
                result.add((Chart)obj);
        return result;
    }

    public static Chart getChartFromSingleSelection(ISelection selection) {
        if (!(selection instanceof IStructuredSelection))
            return null;

        IStructuredSelection structuredSelection = (IStructuredSelection)selection;

        if (structuredSelection.size() == 1 && (structuredSelection.getFirstElement() instanceof Chart))
            return (Chart)structuredSelection.getFirstElement();

        return null;
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

    public static IDList getAllIDs(ResultFileManager manager, ResultType type) {
        if (type == null)
            return manager.getAllItems(true, true);

        switch (type) {
        case SCALAR: return manager.getAllScalars(true, true);
        case VECTOR: return manager.getAllVectors();
        case STATISTICS: return manager.getAllStatistics();
        case HISTOGRAM: return manager.getAllHistograms();
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


    public static void saveChartAsTemplate(Chart chart, File templatesDir, String templateId) throws IOException {
        if (!templatesDir.exists())
            templatesDir.mkdirs();

        String scriptFileName = templateId + ".py";
        String propertiesFileName = templateId + ".properties";

        writeNewTextFile(templatesDir, scriptFileName, chart.getScript());
        writeNewTextFile(templatesDir, propertiesFileName, makeTemplatePropertiesContent(chart, templateId));

        for (DialogPage dp : chart.getDialogPages()) {
            String xswtFileName = templateId + "_" + dp.id + ".xswt";
            writeNewTextFile(templatesDir, xswtFileName, dp.xswtForm);
        }
    }

    private static void writeNewTextFile(File dir, String filename, String content) throws IOException {
        File file = new File(dir.getAbsolutePath() + File.separator + filename);
        if (file.exists())
            throw new RuntimeException(file.getAbsolutePath() + " already exists");
        FileUtils.writeTextFile(file, content, null);
    }

    private static String makeTemplatePropertiesContent(Chart chart, String templateId) throws IOException {
        String scriptFileName = templateId + ".py";
        StringBuffer buf = new StringBuffer();
        buf.append("id = " + templateId + "\n");
        buf.append("name = " + chart.getName() + "\n");
        buf.append("type = " + chart.getType().toString() + "\n");
        buf.append("scriptFile = " + scriptFileName + "\n");
        buf.append("icon = " + chart.getIconPath() + "\n");
        // omitting toolbarOrder and resultTypes
        buf.append("\n");

        buf.append("propertyNames = ");
        boolean first=true;
        for (Property p : chart.getProperties()) {
            if (!first)
                buf.append(",\\\n    ");
            buf.append(p.getName());
            if (p.getValue() != null) {
                String value = p.getName().equals("filter") ? "" : p.getValue();
                buf.append(":" + value); // TODO: what if the value contains a comma? should quote...
            }
            first = false;
        }

        buf.append("\n\n");

        int i = 0;
        for (DialogPage dp : chart.getDialogPages()) {
            String xswtFileName = templateId + "_" + dp.id + ".xswt";
            buf.append("\n");
            buf.append("dialogPage." + i + ".id = " + dp.id + "\n");
            buf.append("dialogPage." + i + ".label = " + dp.label + "\n");
            buf.append("dialogPage." + i + ".xswtFile = " + xswtFileName + "\n");
            i++;
        }
        buf.append("\n\n");

        return buf.toString();
    }

}
