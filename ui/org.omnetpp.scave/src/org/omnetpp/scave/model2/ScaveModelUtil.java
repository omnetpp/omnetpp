/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model2;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.apache.commons.lang3.ArrayUtils;
import org.apache.commons.lang3.StringUtils;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.scave.charttemplates.ChartTemplateRegistry;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Scave;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Chart.DialogPage;
import org.omnetpp.scave.model.ChartTemplate;
import org.omnetpp.scave.model.Folder;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave.model.ModelChangeEvent;
import org.omnetpp.scave.model.ModelObject;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.commands.AddChartCommand;
import org.omnetpp.scave.model.commands.AddInputFileCommand;
import org.omnetpp.scave.model.commands.CommandStack;
import org.omnetpp.scave.model.commands.CompoundCommand;
import org.omnetpp.scave.model.commands.ICommand;
import org.omnetpp.scave.model.commands.MoveItemsWithinParentCommand;
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
        chart.setSupportedResultTypes(template.getSupportedResultTypes());

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

    public static List<String> getResultTypesAsStringList(int resultTypes) {
        List<String> names = new ArrayList<>();

        if ((resultTypes & ResultFileManager.PARAMETER) != 0)
            names.add(Scave.PARAMETER);
        if ((resultTypes & ResultFileManager.SCALAR) != 0)
            names.add(Scave.SCALAR);
        if ((resultTypes & ResultFileManager.VECTOR) != 0)
            names.add(Scave.VECTOR);
        if ((resultTypes & ResultFileManager.STATISTICS) != 0)
            names.add(Scave.STATISTICS);
        if ((resultTypes & ResultFileManager.HISTOGRAM) != 0)
            names.add(Scave.HISTOGRAM);

        return names;
    }

    public static String getResultTypesAsString(int resultTypes) {
        return StringUtils.join(getResultTypesAsStringList(resultTypes), ",");
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

    public static void moveElements(CommandStack commandStack, Folder container, Object[] elements, int index) {
        // the elements[] array contains the elements in no particular order. We need to keep
        // the order they are in the model (Charts object). Plus, we need reverse order due to
        // the order we insert them back.
        List<AnalysisItem> itemsToMove = new ArrayList<>();
        for (AnalysisItem item : container.getItems())
            if (ArrayUtils.contains(elements, item))
                itemsToMove.add(0, item); // reverse order

        // do a series of "Move" commands
        CompoundCommand command = new CompoundCommand("Move analysis items");
        for (AnalysisItem item : itemsToMove)
            command.append(new MoveItemsWithinParentCommand(container, item, index));
        commandStack.execute(command);
    }

    public static void saveChart(CommandStack commandStack, Chart chart, String name, Folder folder) {
        CompoundCommand command = new CompoundCommand("Save chart");

        command.append(new SetChartIsTemporaryCommand(chart, false));
        command.append(new SetChartNameCommand(chart, name));
        command.append(new AddChartCommand(folder, chart));

        commandStack.execute(command);
    }

    public static String getDisplayFullPath(AnalysisItem item, String separator) {
        String chartName = StringUtils.defaultIfEmpty(item.getName(), "<Unnamed>");
        Folder folder = item.getParentFolder();
        if (folder != null && folder.getParentFolder() != null)  // not parentless temp chart, and not in the root folder
            chartName = folder.getPathAsString(separator) + separator + chartName;
        return chartName;
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

    public static <T extends ModelObject> T findEnclosing(ModelObject object, Class<T> type) {
        return findEnclosingOrSelf(object.getParent(), type);
    }

    public static void assignNewIdRec(AnalysisItem item) {
        item.assignNewId();
        if (item instanceof Folder)
            for (AnalysisItem child : ((Folder)item).getItems())
                assignNewIdRec(child);
    }

    public interface IChartFilter { boolean matches(Chart chart); }

    public static List<Chart> collectCharts(Folder folder) {
        return collectCharts(folder, null);
    }

    public static List<Chart> collectCharts(Folder folder, IChartFilter filter) {
        List<Chart> charts = new ArrayList<>();
        for (AnalysisItem item : folder.getItems())
            if (item instanceof Chart && (filter == null || filter.matches((Chart)item)))
                charts.add((Chart)item);
            else if (item instanceof Folder)
                charts.addAll(collectCharts((Folder)item, filter));
        return charts;
    }

    public static List<Chart> getChartsFromSelection(IStructuredSelection selection) {
        List<Chart> result = new ArrayList<Chart>();
        for (Object obj : selection.toArray())
            if (obj instanceof Chart)
                result.add((Chart)obj);
            else if (obj instanceof Folder)
                result.addAll(collectCharts((Folder)obj));
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

    public static ResultItem[] getResultItems(IDList idlist, ResultFileManager manager) {
        int size = idlist.size();
        ResultItem[] items = new ResultItem[size];
        for (int i = 0; i < size; ++i)
            items[i] = manager.getItem(idlist.get(i));
        return items;
    }

    public static List<String> getResultItemFields(IDList idlist, ResultFileManager manager) {
        List<String> fields = new ArrayList<>();
        fields.add(Scave.FILE);
        fields.add(Scave.RUN);
        fields.add(Scave.MODULE);
        fields.add(Scave.NAME);
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
            ResultFileManager.runWithReadLock(manager, () -> {
                for (int i = 0; i < idlist.size(); ++i) {
                    ResultItem r = manager.getItem(idlist.get(i));
                    Debug.println(
                            String.format("File: %s Run: %s Module: %s Name: %s",
                                    r.getFile().getFilePath(),
                                    r.getFileRun().getRun().getRunName(),
                                    r.getModuleName(),
                                    r.getName()));
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
        buf.append("resultTypes = " + ScaveModelUtil.getResultTypesAsString(chart.getSupportedResultTypes()) + "\n");
        // omitting score...
        buf.append("\n");

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
