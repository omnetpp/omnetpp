package org.omnetpp.scave.actions;

import static org.omnetpp.common.image.ImageFactory.TOOLBAR_IMAGE_TEMPLATE;

import java.util.ArrayList;
import java.util.Collection;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.ChartPage;
import org.omnetpp.scave.editors.ui.ScaveEditorPage;
import org.omnetpp.scave.model.Add;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model2.ScaveModelUtil;

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
		if (page != null && page instanceof ChartPage) {
			Chart chart = ((ChartPage)page).getChart();
			if (isTemporaryChart(chart, scaveEditor)) {
				Dataset dataset = ScaveModelUtil.findEnclosingDataset(chart);
				System.out.println("TODO implement"); //FIXME TODO implement
				
//				CreateDatasetDialog dialog = new CreateDatasetDialog(scaveEditor.getSite().getShell(), DatasetDialog.SHOW_SELECTION, "Create chart template");
//				
//				if (dialog.open() == Window.OK) {
//					EditingDomain domain = scaveEditor.getEditingDomain();
//					ScaveModelPackage pkg = ScaveModelPackage.eINSTANCE;
//					ResultFileManager manager = scaveEditor.getResultFileManager();
//					IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, null);
//					ResultItem[] items = ScaveModelUtil.getResultItems(idlist, manager);
//					Collection<Add> origAdds = getOriginalAdds(dataset);
//					Collection<Add> adds = ScaveModelUtil.createAdds(items, dialog.getRunIdFields());
//					
//					CompoundCommand command = new CompoundCommand();
//					command.append(SetCommand.create( // set dataset name
//										domain,
//										dataset,
//										pkg.getDataset_Name(),
//										dialog.getDatasetName()));
//					command.append(SetCommand.create( // set chart name
//										domain,
//										chart,
//										pkg.getChart_Name(),
//										dialog.getDatasetName()));
//					command.append(RemoveCommand.create(domain, origAdds)); // change Add items
//					command.append(AddCommand.create(
//										domain,
//										dataset,
//										pkg.getDataset_Items(),
//										adds,
//										0));
//					command.append(RemoveCommand.create(domain, dataset));
//					command.append(AddCommand.create(
//										domain,
//										scaveEditor.getAnalysis().getDatasets(),
//										ScaveModelPackage.eINSTANCE.getDatasets_Datasets(),
//										dataset));
//					scaveEditor.executeCommand(command);
//				}
			}
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		ScaveEditorPage page = editor.getActiveEditorPage();
		return page != null && page instanceof ChartPage &&
				isTemporaryChart(((ChartPage)page).getChart(), editor);
	}
	
	private boolean isTemporaryChart(Chart chart, ScaveEditor editor) {
		return ScaveModelUtil.findEnclosingObject(chart, Analysis.class) == editor.getTempAnalysis();
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
}
