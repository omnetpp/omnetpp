/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import static org.omnetpp.common.image.ImageFactory.TOOLBAR_IMAGE_TEMPLATE;
import static org.omnetpp.common.util.Pair.pair;

import java.util.ArrayList;
import java.util.Collection;
import java.util.concurrent.Callable;

import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.command.RemoveCommand;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.util.Pair;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.ChartPage;
import org.omnetpp.scave.editors.ui.CreateChartTemplateDialog;
import org.omnetpp.scave.editors.ui.ScaveEditorPage;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.model.Add;
import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.HistogramChart;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.DatasetManager;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Creates a dataset from a temporary chart.
 *
 * @author tomi
 */
//TODO: when creating a "filename(...)" filter in the "Add" node, use relative path if
// possible (if this file and the result file are in the same project)
public class CreateChartTemplateAction extends AbstractScaveAction {

    public CreateChartTemplateAction() {
        setText("Convert to Dataset...");
        setDescription("Create dataset from a temporary chart");
        setImageDescriptor(ImageFactory.global().getDescriptor(TOOLBAR_IMAGE_TEMPLATE));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
        ScaveEditorPage page = scaveEditor.getActiveEditorPage();
        if (page != null && page instanceof ChartPage) {
            final Chart chart = ((ChartPage)page).getChart();
            if (ScaveModelUtil.isTemporaryChart(chart, scaveEditor)) {
                final Dataset dataset = ScaveModelUtil.findEnclosingDataset(chart);

                final CreateChartTemplateDialog dialog = new CreateChartTemplateDialog(scaveEditor.getSite().getShell());
                dialog.setChartName(chart.getName());
                dialog.setDatasetName(chart.getName());

                if (dialog.open() == Window.OK) {
                    EditingDomain domain = scaveEditor.getEditingDomain();
                    ScaveModelPackage pkg = ScaveModelPackage.eINSTANCE;
                    final ResultFileManager manager = scaveEditor.getResultFileManager();
                    Pair<Collection<Add>,Collection<Add>> adds =
                        ResultFileManager.callWithReadLock(manager, new Callable<Pair<Collection<Add>,Collection<Add>>>() {
                            public Pair<Collection<Add>, Collection<Add>> call() {
                                ResultType type = resultTypeForChart(chart);
                                IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, null, type);
                                ResultItem[] items = ScaveModelUtil.getResultItems(idlist, manager);
                                Collection<Add> origAdds = getOriginalAdds(dataset);
                                Collection<Add> adds = ScaveModelUtil.createAddsWithFields(items, dialog.getFilterFields());
                                return pair(origAdds, adds);
                            }
                        });

                    CompoundCommand command = new CompoundCommand();
                    command.append(SetCommand.create( // set dataset name
                                        domain,
                                        dataset,
                                        pkg.getDataset_Name(),
                                        dialog.getDatasetName()));
                    command.append(SetCommand.create( // set chart name
                                        domain,
                                        chart,
                                        pkg.getChart_Name(),
                                        dialog.getChartName()));
                    command.append(RemoveCommand.create(domain, adds.first)); // change Add items
                    command.append(AddCommand.create(
                                        domain,
                                        dataset,
                                        pkg.getDataset_Items(),
                                        adds.second,
                                        0));
                    command.append(RemoveCommand.create(domain, dataset)); // move Dataset
                    command.append(AddCommand.create(
                                        domain,
                                        scaveEditor.getAnalysis().getDatasets(),
                                        ScaveModelPackage.eINSTANCE.getDatasets_Datasets(),
                                        dataset));
                    scaveEditor.executeCommand(command);

                    scaveEditor.showDatasetsPage();
                }
            }
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        ScaveEditorPage page = editor.getActiveEditorPage();
        return page != null && page instanceof ChartPage &&
                ScaveModelUtil.isTemporaryChart(((ChartPage)page).getChart(), editor);
    }

    private Collection<Add> getOriginalAdds(Dataset dataset) {
        Collection<Add> adds = new ArrayList<Add>();
        for (Object obj : dataset.getItems()) {
            if (obj instanceof Add) {
                Add add = (Add)obj;
                if (add.getFilterPattern() != null) {
                    adds.add(add); //XXX revise
                    continue;
                }
            }
            break;
        }
        return adds;
    }

    private ResultType resultTypeForChart(Chart chart) {
        if (chart instanceof LineChart)
            return ResultType.VECTOR_LITERAL;
        else if (chart instanceof BarChart)
            return ResultType.SCALAR_LITERAL;
        else if (chart instanceof HistogramChart)
            return ResultType.HISTOGRAM_LITERAL;
        else
            throw new IllegalArgumentException("Unknown chart type: " + chart.getClass().getName());
    }
}
