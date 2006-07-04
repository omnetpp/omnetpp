package org.omnetpp.scave2.actions;

import static org.omnetpp.scave2.model.RunAttribute.EXPERIMENT;
import static org.omnetpp.scave2.model.RunAttribute.MEASUREMENT;
import static org.omnetpp.scave2.model.RunAttribute.REPLICATION;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.editors.treeproviders.GenericTreeNode;
import org.omnetpp.scave2.editors.ui.BrowseDataPage;
import org.omnetpp.scave2.model.FilterParams;
import org.omnetpp.scave2.model.RunAttribute;

public class SetFilterAction extends AbstractScaveAction {
	
	public SetFilterAction() {
		setText("Set filter");
		setToolTipText("Set the filter on the dataset page from the current selection.");
	}

	@Override
	protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
		Object selected = selection.getFirstElement();
		FilterParams filter = null;
		
		if (selected instanceof InputFile) {
			InputFile inputFile = (InputFile)selected;
			filter = new FilterParams();
			filter.setFileNamePattern(inputFile.getName());
		}
		else if (selected instanceof GenericTreeNode) {
			GenericTreeNode node = (GenericTreeNode)selected;
			Object payload = node.getPayload();
			
			if (payload instanceof ResultFile) {
				ResultFile resultFile = (ResultFile)payload;
				filter = new FilterParams();
				filter.setFileNamePattern(resultFile.getFilePath());
			}
			else if (payload instanceof Run) {
				Run run = (Run)payload;
				filter = new FilterParams();
				filter.setRunNamePattern(run.getRunName());
			}
			else if (payload instanceof RunAttribute) {
				RunAttribute attr = (RunAttribute)payload;
				String name = attr.getName();
				String value = attr.getValue();
				filter = new FilterParams();
				if (EXPERIMENT.equals(name))
					filter.setExperimentNamePattern(value);
				else if (MEASUREMENT.equals(name))
					filter.setMeasurementNamePattern(value);
				else if (REPLICATION.equals(name))
					filter.setReplicationNamePattern(name);
				else
					filter = null;
			}
		}
		
		if (filter != null) {
			BrowseDataPage page = scaveEditor.getBrowseDataPage();
			page.getScalarsPanel().setFilterParams(filter);
			page.getVectorsPanel().setFilterParams(filter);
			scaveEditor.showPage(page);
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor,
			IStructuredSelection selection) {
		return true;
	}
}
