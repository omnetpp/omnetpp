/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.ISharedImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.ChartPage;
import org.omnetpp.scave.editors.ui.ScaveEditorPage;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * Saves a temporary chart.
 *
 * @author tomi, andras
 */
//TODO: when creating a "filename(...)" filter in the "Add" node, use relative path if
// possible (if this file and the result file are in the same project)
public class SaveTempChartAction extends AbstractScaveAction {

    public SaveTempChartAction() {
        setText("Save Chart");
        setDescription("Save temporary chart to Charts page");
        setImageDescriptor(ScavePlugin.getSharedImageDescriptor(ISharedImages.IMG_ETOOL_SAVE_EDIT)); //TODO
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
        Chart chart = getActiveTemporaryChart(scaveEditor);
        if (chart != null) {
            chart.setTemporary(false);
            Command command = AddCommand.create(scaveEditor.getEditingDomain(), scaveEditor.getAnalysis().getCharts(), ScaveModelPackage.eINSTANCE.getCharts_Items(), chart, -1);
            scaveEditor.executeCommand(command);
            scaveEditor.showAnalysisItem(chart);

//TODO offer dialog to simplify input filters
//                final Dataset dataset = ScaveModelUtil.findEnclosingDataset(chart);
//
//                final CreateChartTemplateDialog dialog = new CreateChartTemplateDialog(scaveEditor.getSite().getShell());
//                dialog.setChartName(chart.getName());
//                dialog.setDatasetName(chart.getName());
//
//                if (dialog.open() == Window.OK) {
//                    EditingDomain domain = scaveEditor.getEditingDomain();
//                    ScaveModelPackage pkg = ScaveModelPackage.eINSTANCE;
//                    final ResultFileManager manager = scaveEditor.getResultFileManager();
//                    Pair<Collection<Add>,Collection<Add>> adds =
//                        ResultFileManager.callWithReadLock(manager, new Callable<Pair<Collection<Add>,Collection<Add>>>() {
//                            public Pair<Collection<Add>, Collection<Add>> call() {
//                                ResultType type = resultTypeForChart(chart);
//                                IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, null, type);
//                                ResultItem[] items = ScaveModelUtil.getResultItems(idlist, manager);
//                                Collection<Add> origAdds = getOriginalAdds(dataset);
//                                Collection<Add> adds = ScaveModelUtil.createAddsWithFields(items, dialog.getFilterFields());
//                                return pair(origAdds, adds);
//                            }
//                        });
//
//                    CompoundCommand command = new CompoundCommand();
//                    command.append(SetCommand.create( // set dataset name
//                                        domain,
//                                        dataset,
//                                        pkg.getDataset_Name(),
//                                        dialog.getDatasetName()));
//                    command.append(SetCommand.create( // set chart name
//                                        domain,
//                                        chart,
//                                        pkg.getChart_Name(),
//                                        dialog.getChartName()));
//                    command.append(RemoveCommand.create(domain, adds.first)); // change Add items
//                    command.append(AddCommand.create(
//                                        domain,
//                                        dataset,
//                                        pkg.getDataset_Items(),
//                                        adds.second,
//                                        0));
//                    command.append(RemoveCommand.create(domain, dataset)); // move Dataset
//                    command.append(AddCommand.create(
//                                        domain,
//                                        scaveEditor.getAnalysis().getDatasets(),
//                                        ScaveModelPackage.eINSTANCE.getDatasets_Datasets(),
//                                        dataset));
//                    scaveEditor.executeCommand(command);
//
//                    scaveEditor.showChartsPage();
//                }
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        return getActiveTemporaryChart(editor) != null;
    }

    private Chart getActiveTemporaryChart(ScaveEditor editor) {
        ScaveEditorPage page = editor.getActiveEditorPage();
        if (page != null && page instanceof ChartPage) {
            Chart chart = ((ChartPage)page).getChart();
            if (chart != null && chart.isTemporary())
                return chart;
        }
        return null;
    }

//    private ResultType resultTypeForChart(Chart chart) {
//        if (chart instanceof LineChart)
//            return ResultType.VECTOR_LITERAL;
//        else if (chart instanceof BarChart)
//            return ResultType.SCALAR_LITERAL;
//        else if (chart instanceof HistogramChart)
//            return ResultType.HISTOGRAM_LITERAL;
//        else
//            throw new IllegalArgumentException("Unknown chart type: " + chart.getClass().getName());
//    }
}
