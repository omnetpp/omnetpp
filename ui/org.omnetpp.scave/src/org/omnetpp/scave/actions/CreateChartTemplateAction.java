package org.omnetpp.scave.actions;

import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.command.RemoveCommand;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.ChartPage2;
import org.omnetpp.scave.editors.ui.CreateDatasetDialog;
import org.omnetpp.scave.editors.ui.ScaveEditorPage;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.FilterHints;
import org.omnetpp.scave.model2.ScaveModelUtil;

import static org.omnetpp.common.image.ImageFactory.*;

/**
 * Creates a chart template from a temporary chart.
 *
 * @author tomi
 */
public class CreateChartTemplateAction extends AbstractScaveAction {

	public CreateChartTemplateAction() {
		setText("Create template");
		setDescription("Create a chart template from a temporary chart");
		setImageDescriptor(ImageFactory.getDescriptor(TOOLBAR_IMAGE_TEMPLATE));
	}

	@Override
	protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
		ScaveEditorPage page = scaveEditor.getActiveEditorPage();
		if (page != null && page instanceof ChartPage2) {
			Chart chart = ((ChartPage2)page).getChart();
			if (isTemporaryChart(chart, scaveEditor)) {
				Dataset dataset = ScaveModelUtil.findEnclosingDataset(chart);
				FilterHints hints = new FilterHints(scaveEditor.getResultFileManager(), dataset.getType());
				CreateDatasetDialog dialog = new CreateDatasetDialog(scaveEditor.getSite().getShell(), "Create chart template");
				dialog.setFilterHints(hints);
				
				if (dialog.open() == Window.OK) {
					EditingDomain domain = scaveEditor.getEditingDomain();
					ScaveModelPackage pkg = ScaveModelPackage.eINSTANCE;
					CompoundCommand command = new CompoundCommand();
					command.append(SetCommand.create(
										domain,
										dataset,
										pkg.getDataset_Name(),
										dialog.getDatasetName()));
					command.append(SetCommand.create(
										domain,
										chart,
										pkg.getChart_Name(),
										dialog.getDatasetName()));
					command.append(RemoveCommand.create(domain, dataset));
					command.append(AddCommand.create(
										domain,
										scaveEditor.getAnalysis().getDatasets(),
										ScaveModelPackage.eINSTANCE.getDatasets_Datasets(),
										dataset));
					scaveEditor.executeCommand(command);
				}
			}
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		ScaveEditorPage page = editor.getActiveEditorPage();
		return page != null && page instanceof ChartPage2 &&
				isTemporaryChart(((ChartPage2)page).getChart(), editor);
	}
	
	private boolean isTemporaryChart(Chart chart, ScaveEditor editor) {
		return ScaveModelUtil.findEnclosingObject(chart, Analysis.class) == editor.getTempAnalysis();
	}
}
