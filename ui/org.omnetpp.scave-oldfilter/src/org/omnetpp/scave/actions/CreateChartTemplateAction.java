package org.omnetpp.scave.actions;

import java.util.ArrayList;
import java.util.Collection;

import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.command.RemoveCommand;
import org.eclipse.emf.edit.command.ReplaceCommand;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.ChartPage2;
import org.omnetpp.scave.editors.ui.CreateDatasetDialog;
import org.omnetpp.scave.editors.ui.DatasetDialog;
import org.omnetpp.scave.editors.ui.ScaveEditorPage;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.model.Add;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.DatasetManager;
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
				CreateDatasetDialog dialog = new CreateDatasetDialog(scaveEditor.getSite().getShell(), DatasetDialog.SHOW_SELECTION, "Create chart template");
				
				if (dialog.open() == Window.OK) {
					EditingDomain domain = scaveEditor.getEditingDomain();
					ScaveModelPackage pkg = ScaveModelPackage.eINSTANCE;
					ResultFileManager manager = scaveEditor.getResultFileManager();
					IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, null);
					ResultItem[] items = ScaveModelUtil.getResultItems(idlist, manager);
					Collection<Add> origAdds = getOriginalAdds(dataset);
					Collection<Add> adds = ScaveModelUtil.createAdds(items, dialog.getRunIdFields());
					
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
										dialog.getDatasetName()));
					command.append(RemoveCommand.create(domain, origAdds)); // change Add items
					command.append(AddCommand.create(
										domain,
										dataset,
										pkg.getDataset_Items(),
										adds,
										0));
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
	
	private Collection<Add> getOriginalAdds(Dataset dataset) {
		Collection<Add> adds = new ArrayList<Add>();
		for (Object obj : dataset.getItems()) {
			if (obj instanceof Add) {
				Add add = (Add)obj;
				if (add.getFilenamePattern() != null && add.getRunNamePattern() != null &&
					add.getModuleNamePattern() != null && add.getNamePattern() != null &&
					add.getExperimentNamePattern() == null && add.getMeasurementNamePattern() == null &&
					add.getReplicationNamePattern() == null) {
						adds.add(add);
						continue;
				}
			}
			break;
		}
		return adds;
	}
}
